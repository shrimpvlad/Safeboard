#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <filesystem>
#include <ctime>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <vector>
#include <array>

using namespace std;

enum return_codes{
    not_suspicious_file,
    suspicious_file_js,
    suspicious_file_unix,
    suspicious_file_macos,
    error_code_
};

string suspicious_line_js = "<script>evil_script()</script>";
string suspicious_line_unix = "rm -rf ~/Documents";
string suspicious_line_macos = "system(\"launchctl load /Library/LaunchAgents/com.malware.agent\")";

int Check_JS (string filepath)
{
    ifstream js_file(filepath);
    if (js_file.fail())
        return error_code_;
    string line;
    while ( getline (js_file,line) )
    {
        if (line == suspicious_line_js)
            return suspicious_file_js;
    }
    return not_suspicious_file;
}

int Check_UNIX_macOS (string filepath)
{
    ifstream um_file(filepath);
    if (um_file.fail())
        return error_code_;
    string line;
    while ( getline (um_file,line) )
    {
        if (line == suspicious_line_unix)
            return suspicious_file_unix;
        else if (line == suspicious_line_macos)
            return suspicious_file_macos;
    }
    return not_suspicious_file;
}

array<size_t, 7> Scaner(string path)
{
    size_t count_js = 0;
    size_t count_unix = 0;
    size_t count_macos = 0;
    size_t count_errors = 0;
    size_t count = 0;
    time_t start_time, end_time;
    time(&start_time);
    bool is_directory_exist = filesystem::is_directory(path);
    
    if (is_directory_exist) {
        vector<pair<string,string>> files;
        for (const auto &file: filesystem::directory_iterator(path))
            files.push_back({file.path().string(), file.path().extension()});
#pragma omp parallel for
            for (const auto & entry : files)
            {
                count++;
                if (entry.second == ".js")
                {
                    switch (Check_JS(entry.first)) {
                        case suspicious_file_js:
                            count_js++;
                            break;
                        case error_code_:
                            count_errors++;
                            break;
                    }
                }
                else
                {
                    switch (Check_UNIX_macOS(entry.first)) {
                        case suspicious_file_unix:
                            count_unix++;
                            break;
                        case suspicious_file_macos:
                            count_macos++;
                            break;
                        case error_code_:
                            count_errors++;
                            break;
                    }
                }
            }
    }
    time(&end_time);
    time_t time_of_working = end_time - start_time;

    return {static_cast<size_t>(is_directory_exist), count, count_js,
        count_unix, count_macos, count_errors, static_cast<size_t>(time_of_working)};
}


int main(int argc, char **argv)
{
    int MasterSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (MasterSocket < 0)
    {
        cout<<"Can't create socket"<<endl;
        return 0;
    }
    struct sockaddr_in SockAddr;
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_port = htons(12345);
    SockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(MasterSocket, (struct sockaddr *)&SockAddr, sizeof(SockAddr))<0)
    {
        cout<<"Can't bind socket"<<endl;
        return 0;
    }
    
    listen(MasterSocket, SOMAXCONN);
    
    while (true) {
        int SlaveSocket = accept(MasterSocket, 0, 0);
        if (SlaveSocket<0)
        {
            cout<<"Can't accept"<<endl;
            break;
        }
        char Buffer[1024];
        recv(SlaveSocket, Buffer, 1024, MSG_NOSIGNAL);
        cout<<"Scan the directory: "<< Buffer<<endl;
        auto tmp = Scaner(Buffer);
        send(SlaveSocket, to_string(tmp[0]).c_str(), 1024, MSG_NOSIGNAL);
        send(SlaveSocket, to_string(tmp[1]).c_str(), 1024, MSG_NOSIGNAL);
        send(SlaveSocket, to_string(tmp[2]).c_str(), 1024, MSG_NOSIGNAL);
        send(SlaveSocket, to_string(tmp[3]).c_str(), 1024, MSG_NOSIGNAL);
        send(SlaveSocket, to_string(tmp[4]).c_str(), 1024, MSG_NOSIGNAL);
        send(SlaveSocket, to_string(tmp[5]).c_str(), 1024, MSG_NOSIGNAL);
        send(SlaveSocket, to_string(tmp[6]).c_str(), 1024, MSG_NOSIGNAL);
        shutdown(SlaveSocket, SHUT_RDWR);
        close(SlaveSocket);
    }
}
