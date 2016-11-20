#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>
#include "3d_tracker_socket.h"


#pragma comment(lib, "Ws2_32.lib")

using namespace std;
const string EXIT_FLAG = "DISCONNECT";

DWORD WINAPI ThreadProcServer(void* lpPara) {
    NetPara *netpara = (NetPara*)lpPara;
    WSADATA wsaData;
    int err;
    int recvbuflen = netpara->recvbuflen;
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;
    const string EXIT_FLAG = "DISCONNECT";
    char* recvbuf = new char[recvbuflen];
    char* port;

    port = new char[strlen(netpara->port) + 1];
    strcpy_s(port, strlen(netpara->port) + 1, netpara->port);
    //Initialize recvbuf
    for (int i = 0; i < recvbuflen; i++)
        recvbuf[i] = '\0';

    //WASStartup
    if ((err = WSAStartup(MAKEWORD(2, 2), &wsaData))) {
        cerr << "[Error]WSAStartup failed. Error code:" << err << endl;
        return -1;
    }
    else {
        cout << "[Info]WSAStartup success!" << endl;
    }

    //Get address info
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    if ((err = getaddrinfo(NULL, port, &hints, &result))) {
        cerr << "[Error]getaddrinfo failed. Error code:" << err << endl;
        WSACleanup();
        return -1;
    }
    else {
        cout << "[Info]addrinfo() done!" << endl;
    }

    //Creating a socket
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        cerr << "[Error]socket() error. Error code:" << WSAGetLastError() << endl;
        freeaddrinfo(result);
        WSACleanup();
        return -1;
    }
    else {
        cout << "[Info]Server socket created!" << endl;
    }

    // Binding a socket
    if (bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen) == SOCKET_ERROR) {
        cerr << "[Error]bind() error. Error code:" << WSAGetLastError() << endl;
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return -1;
    }
    else {
        cout << "[Info]Socket binded" << endl;
    }

    // Free resources
    freeaddrinfo(result);

    // Listening on a socket
    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
        cerr << "[Error]listen() failed. Error code:" << WSAGetLastError() << endl;
        closesocket(ListenSocket);
        WSACleanup();
        return -1;
    }
    else {
        cout << "[Info]Listening on a socket." << endl;
    }

    while (1) {
        string recvstr;
        vector<double>recvdata;

        //Accepting a connection
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            cerr << "[Error]accept() failed. Error code:" << WSAGetLastError() << endl;
            closesocket(ListenSocket);
            WSACleanup();
            return -1;
        }
        else {
            cout << "[Info]Connection accepted." << endl;
        }

        do {
            if ((err = recv(ClientSocket, recvbuf, recvbuflen, 0)) > 0) {
                cout << "[Info]recieved: " << recvbuf << endl;
                recvstr = string(recvbuf);

                if (!recvstr.compare(EXIT_FLAG)) {
                    cout << "[Info]Connection closed, bye" << endl;
                    break;
                }

                recvdata.clear();

                if (split2double(recvbuf, recvdata, ' ') == FAIL) {
                    cerr << "[Error]Cannot convert string to double" << endl;
                }

                EnterCriticalSection(&shared_buffer_lock);
                
                if (recvdata.size() == 1) {
                    rdata.dz = recvdata[0];
                    cout << "[Info]Received z data, dz=" << recvdata[0] << endl;
                }
                else if (recvdata.size() == 2) {
                    rdata.dx = recvdata[0];
                    rdata.dy = recvdata[1];
                    cout << "[Info]Received x-y data, dx=" << recvdata[0]
                        << "  dy=" << recvdata[1] << endl;
                }
                else {
                    cerr << "[Error]Receive data error. size = " << recvdata.size() << endl;
                }
                wecho = true;

                LeaveCriticalSection(&shared_buffer_lock);

                for (int i = 0; i < recvbuflen; i++)
                    recvbuf[i] = '\0';
            }
            else if (err == 0 || !recvstr.compare(EXIT_FLAG)) {
                cout << "[Info]Connection closed, bye" << endl;
            }
            else {
                cerr << "[Error]recieve failed. Error code:" << WSAGetLastError() << endl;
                closesocket(ListenSocket);
                WSACleanup();
                return -1;
            }
        } while (err > 0);

        if (!recvstr.compare(EXIT_FLAG)) {
            break;
        }
    }

    delete[] port;
    delete[] netpara->port;
    delete[] recvbuf;
    delete netpara;

    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}



int startlocalserver(char* port, int recvbuflen=512) {
    NetPara *netpara = new NetPara;
    netpara->port = new char[strlen(port) + 1];
    strcpy_s(netpara->port, strlen(port) + 1, port);
    netpara->recvbuflen = recvbuflen;

    DWORD dwThreadId;

    ghThreads_s = CreateThread(
        0,
        0,
        ThreadProcServer,
        (void*)netpara,
        0,
        &dwThreadId
    );

    cout << "Here 4" << endl;

    if (ghThreads_s == NULL) {
        cerr << "[Error]Cannot start a new thread" << endl;
        return -2;
    }
    else
        return 0;
}
