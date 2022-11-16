#include <iostream>
#include <winsock2.h>
#include <string>
#include <windows.h>
#include <WS2tcpip.h>
#include <vector>
#pragma comment (lib, "Ws2_32.lib")
using namespace std;
#define SRV_PORT 999 //server port
#define BUF_SIZE 64 //buffer size

struct OrderAndCost 
{
    int productid;
    int amount;
};

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

    SOCKET ClntSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (ClntSock == INVALID_SOCKET) {
        cout << "Error initialization socket # " << WSAGetLastError() << endl;
        closesocket(ClntSock);
        WSACleanup();
        return 1;
    }
    else
        cout << "Client socket initialization is OK" << endl;

    sockaddr_in srvInfo;
    ZeroMemory(&srvInfo, sizeof(srvInfo));

    
    //srvInfo.sin_addr.s_addr = 0; //Это для указания стандартного IP
    //srvInfo.sin_port = 0; //Это для задания порта сокета по умолчанию
    //srvInfo.sin_addr.s_addr = inet_addr(SERVERADDR);

    /*cout << "Enter server IP in string format (standart value is 127.0.0.1)" << endl;
    string adress;
    cin >> adress;

    PCSTR* a =(PCSTR*)& adress[0]; //Разыменовываем, чтобы получить переменную нужного типа*/

    erStat = inet_pton(AF_INET, "127.0.0.1", &srvInfo.sin_addr);
    if (erStat <= 0) {
        cout << "Error in IP translation to special numeric format" << endl;
        return 1;
    }
    srvInfo.sin_family = AF_INET;
    srvInfo.sin_port = htons(SRV_PORT); // Это если ты типо хочешь свой порт задавать

    short packet_size = 0;

    OrderAndCost mst;
    float* cost;
    char buf[BUF_SIZE]={0};
    do
    {
        cout << "Your (client) request to server, product id and its amount. If you wish to exit enter negative id:";
        cin >> mst.productid >> mst.amount;
        packet_size = sendto(ClntSock, (char*)&mst, sizeof(mst), 0, (sockaddr*)&srvInfo, sizeof(srvInfo));

        if (packet_size == SOCKET_ERROR) {
            cout << "Can't send message to Server. Error # " << WSAGetLastError() << endl;
            closesocket(ClntSock);
            WSACleanup();
            return 1;
        }

        sockaddr newInfo; //Поскольку мы работаем в дейтаграммных сокетах, технически мы можем слушать не только сервер, но и кого угодно, а потому создаём новый adr, который будет заполняться данными отправителя в recvfrom 
        int newInfoSize = sizeof(newInfo); //recvfrom требует именно указатель, поэтому приходится так явно объявлять

        packet_size = recvfrom(ClntSock, (char*)&buf, BUF_SIZE, 0, (sockaddr*)&newInfo, &newInfoSize); // !!
        if (packet_size == SOCKET_ERROR) {
            cout << "Can't recieve message from Server. Error # " << WSAGetLastError() << endl;
            closesocket(ClntSock);
            WSACleanup();
            return 1;
        }

        buf[packet_size] = 0;
        cost = (float*)&buf;
        cout << "Recieved message from server, the cost is: " << *cost << endl;

    } while (*cost >= 0);

    cout << "exit" << endl;
    closesocket(ClntSock);
    WSACleanup();

    return 0;
}