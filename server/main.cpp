// C++ program to show the example of server application in
// socket programmin
#define _WIN32_WINNT _WIN32_WINNT_WIN8 // Windows 8.0
#define WIN32_LEAN_AND_MEAN

#include <cstring>
#include <iostream>
#include <Ws2tcpip.h>
#include <winsock2.h>
#include <unistd.h>

using namespace std;

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

    // Check for socket creation success
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
    service.sin_addr.s_addr = inet_addr("127.0.0.1");  // Replace with your desired IP address
    service.sin_port = htons(55555);  // Choose a port number

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

        // Check for successful connection
        if (acceptSocket == INVALID_SOCKET) {
            std::cout << "accept failed: " << WSAGetLastError() << std::endl;
            WSACleanup();
            return -1;
        } else {
            while(true) {
                char receiveBuffer[200];
                int rbyteCount = recv(acceptSocket, receiveBuffer, 200, 0);
                if (rbyteCount < 0) {
                    if(WSAGetLastError() != 10054 ) std::cout << "Server recv error: " << WSAGetLastError() << std::endl;
                    else close(acceptSocket);
                    break;
                } else if(rbyteCount != 0 && strlen(receiveBuffer) > 1) {
                    std::cout << "Received data: " << receiveBuffer << std::endl;
                    printf("IP:%d.%d.%d.%d | Port: %d\n",
                      int(client_addr.sin_addr.s_addr&0xFF),
                      int((client_addr.sin_addr.s_addr&0xFF00)>>8),
                      int((client_addr.sin_addr.s_addr&0xFF0000)>>16),
                      int((client_addr.sin_addr.s_addr&0xFF000000)>>24), (int) ntohs(client_addr.sin_port));

                }

                int sbyteCount = send(acceptSocket, "Message delivered", 200, 0);
                if (sbyteCount == SOCKET_ERROR) {
                    std::cout << "Server send error: " << WSAGetLastError() << std::endl;
                    return -1;
                }
            }
        }
    }

    // Receive data from the client
    return 0;
}
