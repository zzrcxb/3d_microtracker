#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <iostream>
#include <string>
#include "3d_tracker_socket.h"

#pragma comment(lib, "Ws2_32.lib")


using namespace std;
const string EXIT_FLAG = "DISCONNECT";

DWORD WINAPI ThreadProcClient(LPVOID lpParam) {
    NetPara* netpara = (NetPara*)lpParam;
    char* ipv4 = netpara->ipv4;
    char* port = netpara->port;




}


int startsocketclient(char* ipv4, char* port) {
    WSADATA wsaData;
    int err;
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    SOCKET ConnectSocket = INVALID_SOCKET;
    string sendflag;
    
    // WSAStartup
    if ((err = WSAStartup(MAKEWORD(2, 2), &wsaData))) {
        cerr << "[Error]WSAStartup failed. Error code:" << err << endl;
        return -1;
    }
    else {
        cout << "[Info]WSAStartup success!" << endl;
    }

    // Get address info
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if ((err = getaddrinfo(ipv4, port, &hints, &result))) {
        cerr << "[Error]getaddrinfo failed. Error code:" << err << endl;
        WSACleanup();
        return -1;
    }
    else {
        cout << "[Info]addrinfo() done!" << endl;
    }

    // Creating a socket
    ptr = result;
    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        cerr << "[Error]socket() error. Error code:" << WSAGetLastError() << endl;
        freeaddrinfo(result);
        WSACleanup();
        return -1;
    }
    else {
        cout << "[Info]Client socket created!" << endl;
    }

    //Connecting to socket
    if (connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR) {
        cerr << "[Error]connect() error. Error code:" << WSAGetLastError() << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(result);
        WSACleanup();
        return -1;
    }
    else {
        cout << "[Info]Socket connected" << endl;
    }

    freeaddrinfo(result);

    while (1) {
        if (wenable) {
            sendflag = string(shared_buffer);

            if (send(ConnectSocket, shared_buffer, (int)strlen(shared_buffer), 0) == SOCKET_ERROR) {
                cerr << "[Error]send() failed. Error code:" << WSAGetLastError() << endl;
                closesocket(ConnectSocket);
                WSACleanup();
                return -1;
            }
            else {
                cout << "[Info]Data sent" << endl;
            }

            if (!sendflag.compare(EXIT_FLAG))
                break;
        }
    }

    // Close send
    if (shutdown(ConnectSocket, SD_SEND) == SOCKET_ERROR) {
        cerr << "[Error]shutdown send() failed. Error code:" << WSAGetLastError() << endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return -1;
    }
    else {
        cout << "[Info]No more data will be sent" << endl;
    }

    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}

