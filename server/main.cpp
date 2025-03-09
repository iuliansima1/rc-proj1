
#define _WIN32_WINNT _WIN32_WINNT_WIN8 
#define WIN32_LEAN_AND_MEAN
#include <bits/stdc++.h>
#include <cstring>
#include <iostream>
#include <Ws2tcpip.h>
#include <winsock2.h>
#include <unistd.h>
#include <windows.h>
#include <map>

#define BUFFER_SIZE 1024

using namespace std;

map<SOCKET, sockaddr_in> sockets_map;

DWORD WINAPI handle_client(LPVOID client_socket_ptr) {
    SOCKET client_socket = *(SOCKET*)client_socket_ptr;
    char buffer[BUFFER_SIZE];

    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);

        if (bytes_received <= 0) {
            printf("Client IP:%d.%d.%d.%d:%d disconnected\n",
              int(sockets_map[client_socket].sin_addr.s_addr&0xFF),
              int((sockets_map[client_socket].sin_addr.s_addr&0xFF00)>>8),
              int((sockets_map[client_socket].sin_addr.s_addr&0xFF0000)>>16),
              int((sockets_map[client_socket].sin_addr.s_addr&0xFF000000)>>24), (int) ntohs(sockets_map[client_socket].sin_port));
            break;
        }

        std::cout << "Received: " << buffer << std::endl;
        std::cout << "From: "<<int(sockets_map[client_socket].sin_addr.s_addr&0xFF)<<"."<<
        int((sockets_map[client_socket].sin_addr.s_addr&0xFF00)>>8)<<"."<<int((sockets_map[client_socket].sin_addr.s_addr&0xFF0000)>>16)<<"."<<int((sockets_map[client_socket].sin_addr.s_addr&0xFF000000)>>24)<<
        ":"<<(int) ntohs(sockets_map[client_socket].sin_port) << std::endl;

        // Send confirmation of deliver back to the client
        send(client_socket, "Message delivered", 200, 0);
    }

    closesocket(client_socket);
    return 0;
}
int main()
{
    // Initialize WSA variables

    WSADATA wsaData;
    int wsaerr;
    WORD wVersionRequested = MAKEWORD(2, 2);
    wsaerr = WSAStartup(wVersionRequested, &wsaData);

    if (wsaerr != 0) {
        std::cout << "The Winsock dll not found!" << std::endl;
        return 0;
    }

   // Create a socket
    SOCKET serverSocket;
    serverSocket = INVALID_SOCKET;
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (serverSocket == INVALID_SOCKET) {
        std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 0;
    } else {
        std::cout << "Socket is OK!" << std::endl;
    }

    // Bind the socket to an IP address and port number
    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr("127.0.0.1");  
    service.sin_port = htons(55555); 

    // Use the bind function
    if (bind(serverSocket, reinterpret_cast<SOCKADDR*>(&service), sizeof(service)) == SOCKET_ERROR) {
        std::cout << "bind() failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 0;
    } else {
        std::cout << "bind() is OK!" << std::endl;
    }
    if (listen(serverSocket, 1) == SOCKET_ERROR) {
    std::cout << "listen(): Error listening on socket: " << WSAGetLastError() << std::endl;
    } else {
        std::cout << "listen() is OK! I'm waiting for new connections..." << std::endl;
    }


    while(true) {
        SOCKET acceptSocket;
        sockaddr_in client_addr;
        int clen = 200;
        acceptSocket = accept(serverSocket, (struct sockaddr *)&client_addr, &clen);
        sockets_map[acceptSocket] = client_addr;
        printf("Connection from IP:%d.%d.%d.%d:%d accepted\n",
          int(client_addr.sin_addr.s_addr&0xFF),
          int((client_addr.sin_addr.s_addr&0xFF00)>>8),
          int((client_addr.sin_addr.s_addr&0xFF0000)>>16),
          int((client_addr.sin_addr.s_addr&0xFF000000)>>24), (int) ntohs(client_addr.sin_port));
        HANDLE thread = CreateThread(NULL, 0, handle_client, &acceptSocket, 0, NULL);
        if (thread == NULL) {
            std::cerr << "Failed to create thread!" << std::endl;
            closesocket(acceptSocket);
        } else {
            CloseHandle(thread);
        }
    }

    return 0;
}
