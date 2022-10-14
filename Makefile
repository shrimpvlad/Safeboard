all: build_server build_client

build_server:
	g++-12 -std=c++17 -arch arm64 -fopenmp -o server server.cpp

build_client:
	g++-12 -std=c++17 -arch arm64 -fopenmp -o client client.cpp

