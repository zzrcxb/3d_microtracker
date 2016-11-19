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

int startlocalserver(char* port, int recvbuflen=512) {
    WSADATA wsaData;
    int err;
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;
    const string EXIT_FLAG = "DISCONNECT";
    char* recvbuf = new char[recvbuflen];
    vector<string> resdata;

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
                resdata.push_back(recvstr);

                for (int i = 0; i < recvbuflen; i++)
                    recvbuf[i] = '\0';
            }
            else if (err == 0) {
                cout << "[Info]Connection closed, bye" << endl;
            }
            else {
                cerr << "[Error]recieve failed. Error code:" << WSAGetLastError() << endl;
                closesocket(ListenSocket);
                WSACleanup();
                return -1;
            }
        } while (err > 0);

        if (!recvstr.compare(EXIT_FLAG))
            break;
    }

    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}
