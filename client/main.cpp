#include <cstring>
#include <iostream>
#include <Ws2tcpip.h>
#include <winsock2.h>
#include <unistd.h>

using namespace std;

int main(int argc, char * argv[])
{
    char ipaddr[144] = "127.0.0.1";
    int port = 55555;
    if(argc >= 3) {
        strcpy(ipaddr, argv[1]);
        port = atoi(argv[2]);
        printf("%s\n%d\n", ipaddr, port);
    }
    WSADATA wsaData;
    int wserr;
    WORD wVersionRequested = MAKEWORD(2, 2);
    wserr = WSAStartup(wVersionRequested, &wsaData);

    // Check for initialization success
    if (wserr != 0) {
        std::cout << "The winsock dll not found" << std::endl;
        return 0;
    } else {
        std::cout << "The Winsock dll found" << std::endl;
        std::cout << "The status: " << wsaData.szSystemStatus << std::endl;
    }

    // Create a socket
    SOCKET clientSocket;
    clientSocket = INVALID_SOCKET;
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (clientSocket == INVALID_SOCKET) {
        std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 0;
    } else {
        std::cout << "Socket is OK!" << std::endl;
    }

    sockaddr_in clientService;
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr(ipaddr);  
    clientService.sin_port = htons(port);  

    // Use the connect function
    if (connect(clientSocket, reinterpret_cast<SOCKADDR*>(&clientService), sizeof(clientService)) == SOCKET_ERROR) {
        std::cout << "Client: connect() - Failed to connect: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 0;
    } else {
        std::cout << "Client: Connect() is OK!" << std::endl;
        std::cout << "Client: Can start sending and receiving data..." << std::endl;
    }
    while(true) {
        // Sending data to the server
        char buffer[200];
        std::cout << "Enter the message: ";
        std::cin.getline(buffer, 200);
        if(strstr(buffer, "^") != NULL) {
            return 0;
        }
        int sbyteCount = send(clientSocket, buffer, 200, 0);
        if (sbyteCount == SOCKET_ERROR) {
            std::cout << "Client send error: " << WSAGetLastError() << std::endl;
            return -1;
        }

        // Receiving data from the server
        char receiveBuffer[200];
        int rbyteCount = recv(clientSocket, receiveBuffer, 200, 0);
        if (rbyteCount < 0) {
            std::cout << "Client recv error: " << WSAGetLastError() << std::endl;
            return 0;
        } else {
            std::cout << "Server response: " << receiveBuffer << std::endl;
        }
    }
    close(clientSocket);

    return 0;
}
