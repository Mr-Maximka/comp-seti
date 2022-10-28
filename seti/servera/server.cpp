#include <iostream>
#include <winsock2.h>
#include <string>
#include <windows.h>
#include <WS2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")
//#pragma warning(disable: 4996)
using namespace std;
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define SRV_PORT 1234 //server port
#define BUF_SIZE 64 //buffer size

int main() {
    
    char buff[1024];

    WSADATA wsData;
    int erStat = WSAStartup(MAKEWORD(2, 2), &wsData);
    if (erStat != 0) {
        cout << "Error WinSock version initializaion # ";
        cout << WSAGetLastError();
        return 1;
    }
    else
        cout << "WinSock initialization is OK" << endl;

    SOCKET ServSock = socket(AF_INET, SOCK_STREAM, 0);
    if (ServSock == INVALID_SOCKET) {
        cout << "Error initialization socket # " << WSAGetLastError() << endl;
        closesocket(ServSock);
        WSACleanup();
        return 1;
    }
    else
        cout << "Server socket initialization is OK" << endl;

    sockaddr_in servInfo;
    ZeroMemory(&servInfo, sizeof(servInfo));

    servInfo.sin_family = AF_INET;
    servInfo.sin_addr.s_addr = 0;
    servInfo.sin_port = htons(SRV_PORT);

    erStat = bind(ServSock, (sockaddr*)&servInfo, sizeof(servInfo));
    if (erStat != 0) {
        cout << "Error Socket binding to server. Error # " << WSAGetLastError() << endl;
        closesocket(ServSock);
        WSACleanup();
        return 1;
    }
    else
        cout << "Binding socket to Server info is OK" << endl;

    erStat = listen(ServSock, 10);
    if (erStat != 0) {
        cout << "Can't start to listen to. Error # " << WSAGetLastError() << endl;
        closesocket(ServSock);
        WSACleanup();
        return 1;
    }
    else {
        cout << "Listening..." << endl;
    }

    string msg;

    sockaddr_in clientInfo;
    ZeroMemory(&clientInfo, sizeof(clientInfo));
    int clientInfo_size = sizeof(clientInfo);

    short packet_size = 0;

    char buf[BUF_SIZE] = { 0 };
    while (true) {
        SOCKET ClientConn = accept(ServSock, (sockaddr*)&clientInfo, &clientInfo_size);
        if (ClientConn == INVALID_SOCKET) {
            cout << "Client detected, but can't connect to a client. Error # " << WSAGetLastError() << endl;
            closesocket(ServSock);
            closesocket(ClientConn);
            WSACleanup();
            return 1;
        }
        else
            cout << "Connection to a client established successfully" << endl;

        while (true) {
            packet_size = recv(ClientConn, (char*)buf, BUF_SIZE, 0);
            buf[packet_size] = 0;
            msg = string(buf);
            cout << "Recieved message from client: " << buf << endl;
            if (msg == "Bye") break;

            cout << "Your (server) message to client: ";
            getline(cin, msg);
            packet_size = send(ClientConn, (char*)&msg[0], msg.size(), 0);
            if (packet_size == SOCKET_ERROR) {
                cout << "Can't send message to Server. Error # " << WSAGetLastError() << endl;
                closesocket(ClientConn);
                WSACleanup();
                return 1;
            }
        }
        cout << "The Client has disconnected" << endl;
        closesocket(ClientConn);
    }
    return 0;
}