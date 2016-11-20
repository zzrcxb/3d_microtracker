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


DWORD WINAPI ThreadProcClient(void* lpParam) {
    NetPara* netpara = (NetPara*)lpParam;
    char* ipv4;
    char* port;
    const string EXIT_FLAG = "DISCONNECT";
    WSADATA wsaData;
    int err;
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    SOCKET ConnectSocket = INVALID_SOCKET;
    string sendflag;

    // Copy string
    ipv4 = new char[strlen(netpara->ipv4) + 1];
    port = new char[strlen(netpara->port) + 1];
    strcpy_s(ipv4, strlen(netpara->ipv4) + 1, netpara->ipv4);
    strcpy_s(port, strlen(netpara->port) + 1, netpara->port);

    // cout << netpara->ipv4 << endl;
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
        if (renable) {
            EnterCriticalSection(&shared_buffer_lock);
            renable = false;
            // cout << renable << endl;
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
            recho = true;
            LeaveCriticalSection(&shared_buffer_lock);

            if (!sendflag.compare(EXIT_FLAG))
                break;
        }
        Sleep(1);
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

    delete[] ipv4;
    delete[] port;
    delete[] netpara->ipv4;
    delete[] netpara->port;
    delete netpara;

    return 0;
}


int startsocketclient(char* ipv4, char* port) {
    NetPara *netpara = new NetPara;
    netpara->ipv4 = new char[strlen(ipv4) + 1];
    netpara->port = new char[strlen(port) + 1];

    strcpy_s(netpara->ipv4, strlen(ipv4) + 1, ipv4);
    strcpy_s(netpara->port, strlen(port) + 1, port);

    DWORD dwThreadId;

    ghThreads_c = CreateThread(
        0,
        0,
        ThreadProcClient,
        (void*)netpara,
        0,
        &dwThreadId
    );

    cout << "Here 4" << endl;

    if (ghThreads_c == NULL) {
        cerr << "[Error]Cannot start a new thread" << endl;
        return -2;
    }
    else
        return 0;
}

