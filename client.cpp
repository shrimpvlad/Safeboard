#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <iostream>
#include <iomanip>
using namespace std;

int main(int argc, char **argv)
{
    if (!argv[1])
    {
        cout<<"Forgot dirictory!!!\nTry again"<<endl;
        return 0;
    }
    int Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (Socket < 0)
    {
        cout<<"Can't open socket"<<endl;
        return 0;
    }
    struct sockaddr_in SockAddr;
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_port = htons(12345);
    SockAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (connect(Socket, (struct sockaddr *)(&SockAddr), sizeof(SockAddr))<0)
    {
        cout<<"Can't connect to socket"<<endl;
        return 0;
    }
    string s = argv[1];
    char Buffer[s.length() + 1];
    strcpy(Buffer, s.c_str());
    
    send(Socket, Buffer, 1024, MSG_NOSIGNAL);
    cout<<"====== Scan result ======"<<"\n";
    char exist[1024];
    char count[1024];
    char count_js[1024];
    char count_unix[1024];
    char count_macos[1024];
    char count_errors[1024];
    char count_time[1024];
    recv(Socket, exist, 1024, MSG_NOSIGNAL);
    if (exist[0] == '1')
    {
        recv(Socket, count, 1024, MSG_NOSIGNAL);
        
        recv(Socket, count_js, 1024, MSG_NOSIGNAL);
        
        recv(Socket, count_unix, 1024, MSG_NOSIGNAL);
        
        recv(Socket, count_macos, 1024, MSG_NOSIGNAL);
        
        recv(Socket, count_errors, 1024, MSG_NOSIGNAL);
        
        recv(Socket, count_time, 1024, MSG_NOSIGNAL);
        
        cout<<"exist: "<<exist<<endl;
        cout<<"Processed files: "<<count<<endl;
        cout<<"JS detects: "<<count_js<<endl;
        cout<<"Unix detects: "<<count_unix<<endl;
        cout<<"macOS detects: "<<count_macos<<endl;
        cout<<"Errors: "<<count_errors<<endl;
        cout << "Exection time: " <<count_time<<" seconds"<<endl;
        cout<<"========================="<<endl;
    }
    else
    {
        cout<<"No such directory!\nTry again"<<endl;
    }
    shutdown(Socket, SHUT_RDWR);
    close(Socket);
    
//    printf("%s\n", Buffer);
}
