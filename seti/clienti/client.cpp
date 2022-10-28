#include <iostream>
#include <winsock2.h>
#include <string>
#include <windows.h>
#include <WS2tcpip.h>
#include <vector>
//#pragma warning(disable: 4996)
#pragma comment (lib, "Ws2_32.lib")
using namespace std;
//#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define SRV_HOST "localhost" //server name
#define SRV_PORT 1234 //server port
#define CLNT_PORT 1235 //client port
#define BUF_SIZE 64 //buffer size

int main()
{
    //interface realization
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

    SOCKET ClntSock = socket(AF_INET, SOCK_STREAM, 0);
    if (ClntSock == INVALID_SOCKET) {
        cout << "Error initialization socket # " << WSAGetLastError() << endl;
        closesocket(ClntSock);
        WSACleanup();
        return 1;
    }
    else
        cout << "Client socket initialization is OK" << endl;

    sockaddr_in clntInfo;
    ZeroMemory(&clntInfo, sizeof(clntInfo));

    clntInfo.sin_family = AF_INET;
    clntInfo.sin_addr.s_addr = 0;
    clntInfo.sin_port = htons(CLNT_PORT);

    erStat = bind(ClntSock, (sockaddr*)&clntInfo, sizeof(clntInfo));
    if (erStat != 0) {
        cout << "Error Socket binding to client. Error # " << WSAGetLastError() << endl;
        closesocket(ClntSock);
        WSACleanup();
        return 1;
    }
    else
        cout << "Binding socket to Client info is OK" << endl;

    in_addr ip_to_num;
    erStat = inet_pton(AF_INET, "127.0.0.1", &ip_to_num);
    if (erStat <= 0) {
        cout << "Error in IP translation to special numeric format" << endl;
        return 1;
    }

    sockaddr_in servInfo;
    ZeroMemory(&servInfo, sizeof(servInfo));

    servInfo.sin_family = AF_INET;
    servInfo.sin_addr = ip_to_num;
    servInfo.sin_port = htons(SRV_PORT);

    erStat = connect(ClntSock, (sockaddr*)&servInfo, sizeof(servInfo));
    if (erStat != 0) {
        cout << "Connection to Server is FAILED. Error # " << WSAGetLastError() << endl;
        closesocket(ClntSock);
        WSACleanup();
        return 1;
    }
    else
        cout << "Connection established SUCCESSFULLY. Ready to send a message to Server"
        << endl;

    short packet_size = 0;

    string mst;
    char buf[BUF_SIZE]={0};
    do
    {
        cout << "Your (client) message to server: ";
        getline(cin, mst);
        packet_size = send(ClntSock, (char*)&mst[0], mst.size(), 0);

        if (packet_size == SOCKET_ERROR) {
            cout << "Can't send message to Server. Error # " << WSAGetLastError() << endl;
            closesocket(ClntSock);
            WSACleanup();
            return 1;
        }

        packet_size = recv(ClntSock, (char*)&buf, BUF_SIZE, 0);
        buf[packet_size] = 0;
        mst = (string)buf;
        cout << "Recieved message from server: " << mst << endl;

    } while (mst != "Bye");

    cout << "exit" << endl;
    closesocket(ClntSock);
    WSACleanup();

    return 0;
}