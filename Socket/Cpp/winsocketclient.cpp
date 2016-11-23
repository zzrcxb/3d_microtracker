#include <iostream>
#include <string>
#include "3d_tracker_socket.h"

using namespace std;

int WinSockClient::startsocketclient() {
    WSADATA wsaData;
    int err;
    struct addrinfo *result = NULL, hints;
    SOCKET ConnectSocket = INVALID_SOCKET;
    string sendflag;

    // WSAStartup
    if ((err = WSAStartup(MAKEWORD(2, 2), &wsaData))) {
        cerr << "[Error]WSAStartup failed. Error code:" << err << endl;
        error = true;
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
        error = true;
        return -1;
    }
    else {
        cout << "[Info]addrinfo() done!" << endl;
    }

    // Creating a socket
    ptr = result;
    return 0;
}


int sendmessage(WinSockClient* winsock_c) {
    char *ipv4 = winsock_c->ipv4, *port = winsock_c->port;
    bool &error = winsock_c->error, &ready = winsock_c->ready, &done = winsock_c->done;
    char* msg = winsock_c->msg;

    char echo[32] = { 0 };
    struct addrinfo *ptr = winsock_c->ptr;

    CRITICAL_SECTION &shared_buffer_lock = winsock_c->shared_buffer_lock;
    SOCKET ConnectSocket;
    int err;

    while (1) {
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            cerr << "[Error]socket() error. Error code:" << WSAGetLastError() << endl;
            WSACleanup();
            error = true;
            return -1;
        }
        else {
            cout << "[Info]Client socket created!" << endl;
        }

        //Connecting to socket
        if (connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen) == SOCKET_ERROR) {
            cerr << "[Error]connect() error. Error code:" << WSAGetLastError() << endl;
            closesocket(ConnectSocket);
            WSACleanup();
            error = true;
            return -1;
        }
        else {
            cout << "[Info]Socket connected" << endl;
        }

        ready = true;

        if (send(ConnectSocket, msg, (int)strlen(msg), 0) == SOCKET_ERROR) {
            cerr << "[Error]send() failed. Error code:" << WSAGetLastError() << endl;
            closesocket(ConnectSocket);
            WSACleanup();
            error = true;
            return -1;
        }
        else {
            cout << "[Info]Data sent, " << msg << endl;
        }

        if (shutdown(ConnectSocket, SD_SEND) == SOCKET_ERROR) {
            cout << "[Error]Shutdown failed with error. Error code:" << WSAGetLastError() << endl;
            closesocket(ConnectSocket);
            WSACleanup();
            error = true;
            return -1;
        }

        do {
            if ((err = recv(ConnectSocket, echo, 32, 0)) > 0) {
                echo[err] = '\0';
                cout << "[Info]Server replied " << echo << endl;
            }
            else if (err == 0) {
                cout << "[Info]Closed" << endl;
            }
            else {
                cerr << "[Error]Cannot get reply from server." << endl;
                break;
            }
        } while (err > 0);

        if (strcmp(echo, "ACK") == 0) {
            cout << "[Info]Server accepted" << endl;
            break;
        }
    }
    
    closesocket(ConnectSocket);
    done = true;
    return 0;
}

DWORD WINAPI ThreadProcSend(void* lpPara) {
    WinSockClient* winsock_c = (WinSockClient*)lpPara;
    sendmessage(winsock_c);
    return 0;
}

int WinSockClient::send(char* bufstr) {
    DWORD dwThreadId;
    int len = strlen(bufstr);
    if (len >= sendbuflen)
        return -3;

    strcpy_s(msg, len + 1, bufstr);

    ghThreads_c = CreateThread(
        0,
        0,
        ThreadProcSend,
        this,
        0,
        &dwThreadId
    );

    if (ghThreads_c == NULL) {
        cerr << "[Error]Cannot start a new thread to receive data" << endl;
        error = true;
        return -2;
    }
    else {
        while (1) {
            if (error) {
                return -1;
            }
            if (ready) {
                return 0;
            }
            Sleep(5);
        }
    }
}

int WinSockClient::stopsocketclient() {
    WSACleanup();
    return 0;
}
