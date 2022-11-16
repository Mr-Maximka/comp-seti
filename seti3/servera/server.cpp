#include <iostream>
#include <winsock2.h>
#include <string>
#include <windows.h>
#include <WS2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")
//#pragma warning(disable: 4996)
using namespace std;
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define SRV_PORT 999 //server port
#define BUF_SIZE 64 //buffer size

struct OrderAndCost
{
    int productid;
    int amount;
};

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

    SOCKET ServSock = socket(AF_INET, SOCK_DGRAM, 0);
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
    servInfo.sin_addr.s_addr = INADDR_ANY; //Для подробностей https://stackoverflow.com/questions/16508685/understanding-inaddr-any-for-socket-programming
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

    OrderAndCost* msg = new OrderAndCost;
    //msg->amount = 0; msg->productid = 0;
    float result;



    short packet_size = 0;

    char buf[BUF_SIZE] = { 0 };
    while (true) {
        sockaddr_in clientInfo;
        int clientInfo_size = sizeof(clientInfo);

        packet_size = recvfrom(ServSock, (char*)&buf, BUF_SIZE, 0, (sockaddr *)&clientInfo, &clientInfo_size);
        if (packet_size == SOCKET_ERROR) {
            cout << "Can't send message. Error # " << WSAGetLastError() << endl;
            closesocket(ServSock);
            WSACleanup();
            return 1;
        }

        char ipstr[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientInfo.sin_addr, (PSTR)ipstr, sizeof(ipstr));
        cout << "Message was recieved from client: " << ipstr << ":" << ntohs(clientInfo.sin_port) << endl;

        buf[packet_size] = 0;
        memcpy(msg, buf, sizeof(msg));
        cout << "Message Recieved, Calculating The Result from: " << msg->productid << " " << msg->amount << endl;

        if (msg->amount >= 0 && msg->productid >= 0)
        {
            result = msg->amount * msg->productid;
            cout << result << endl;
        }
        else result = -1;

        packet_size = sendto(ServSock, (char*)&result, sizeof(result), 0, (sockaddr*)&clientInfo, sizeof(clientInfo));
        if (packet_size == SOCKET_ERROR) {
            cout << "Can't send message. Error # " << WSAGetLastError() << endl;
            closesocket(ServSock);
            WSACleanup();
            return 1;
        }
    }
    return 0;
}