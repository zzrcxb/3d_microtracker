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
    WinSockClient* winsock_c = (WinSockClient*)lpParam;
    char *ipv4 = winsock_c->ipv4, *port = winsock_c->port;
    bool* renable = winsock_c->renable, *recho = winsock_c->recho;
    bool* error = winsock_c->error, *ready = winsock_c->ready;
    CRITICAL_SECTION &shared_buffer_lock = winsock_c->shared_buffer_lock;
    char* shared_buffer = winsock_c->shared_buffer;

    const string EXIT_FLAG = "DISCONNECT";
    WSADATA wsaData;
    int err;
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    SOCKET ConnectSocket = INVALID_SOCKET;
    string sendflag;

    // WSAStartup
    if ((err = WSAStartup(MAKEWORD(2, 2), &wsaData))) {
        cerr << "[Error]WSAStartup failed. Error code:" << err << endl;
        *error = true;
        *ready = true;
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
        *error = true;
        *ready = true;
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
        *error = true;
        *ready = true;
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
        *error = true;
        *ready = true;
        return -1;
    }
    else {
        cout << "[Info]Socket connected" << endl;
    }

    *ready = true;

    while (1) {
        char echo[10] = { 0 };
        
        while(*renable) {
            EnterCriticalSection(&shared_buffer_lock);

            *renable = false;
            sendflag = string(shared_buffer);

            if (send(ConnectSocket, shared_buffer, (int)strlen(shared_buffer), 0) == SOCKET_ERROR) {
                cerr << "[Error]send() failed. Error code:" << WSAGetLastError() << endl;
                closesocket(ConnectSocket);
                WSACleanup();
                *error = true;
                return -1;
            }
            else {
                cout << "[Info]Data sent, " << shared_buffer << endl;
            }

            do {
                if (recv(ConnectSocket, echo, 3, 0) > 0) {
                    if (strcmp(echo, "ACK") == 0) {
                        cout << "[Info]Server accepted" << endl;
                        break;
                    }
                }
                else {
                    cerr << "[Error]Cannot get reply from server." << endl;
                    break;
                }
            } while (1);

            *recho = true;  
            LeaveCriticalSection(&shared_buffer_lock);
        }

        if (!sendflag.compare(EXIT_FLAG))
            break;

        Sleep(1);
    }

    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}


int WinSockClient::startsocketclient() {
    DWORD dwThreadId;
 
    ghThreads_c = CreateThread(
        0,
        0,
        ThreadProcClient,
        this,
        0,
        &dwThreadId
    );

    while (!*ready);

    if (ghThreads_c == NULL) {
        cerr << "[Error]Cannot start a new thread" << endl;
        return -2;
    }
    else
        return 0;
}


int WinSockClient::sendmessage(char* bufstr) {
    EnterCriticalSection(&shared_buffer_lock);
    strcpy_s(shared_buffer, strlen(bufstr) + 1, bufstr);
    LeaveCriticalSection(&shared_buffer_lock);

    *renable = true;
    while (!*recho);
    *recho = false;
    return 0;
}


int WinSockClient::stopsocketclient() {
    WaitForSingleObject(ghThreads_c, 5000);

    return 0;
}

