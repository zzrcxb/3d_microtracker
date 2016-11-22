#include "3d_tracker_socket.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int WinSockServer::startlocalserver() {
    WSADATA wsaData;
    int err;
    struct addrinfo *result = NULL, hints;

    //WASStartup
    if ((err = WSAStartup(MAKEWORD(2, 2), &wsaData))) {
        cerr << "[Error]WSAStartup failed. Error code:" << err << endl;
        *error = true;
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
        *error = true;
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
        *error = true;
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
        *error = true;
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
        *error = true;
        return -1;
    }
    else {
        cout << "[Info]Listening on a socket." << endl;
    }

    return 0;
}

DWORD WINAPI ThreadProcRecv(void* lpPara) {
    WinSockServer* winsock_s = (WinSockServer*)lpPara;
    receive(winsock_s);
    return 0;
}

int receive(WinSockServer* winsock_s) {
    int recvbuflen = winsock_s->recvbuflen;
    char split = winsock_s->split;

    CRITICAL_SECTION &shared_buffer_lock = winsock_s->shared_buffer_lock;
    bool* wecho = winsock_s->wecho;
    bool* error = winsock_s->error;
    bool* ready = winsock_s->ready;
    int* recvsize = winsock_s->recvsize;
    resdata &rdata = *winsock_s->rdata;

    SOCKET ClientSocket = INVALID_SOCKET;
    SOCKET ListenSocket = winsock_s->ListenSocket;
    char* recvbuf = new char[recvbuflen];
    vector<double>recvdata;
    int err;

    while (1) {
        //Accepting a connection
        cout << "[Info]Waiting for a connection" << endl;
        *ready = true;
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            cerr << "[Error]accept() failed. Error code:" << WSAGetLastError() << endl;
            closesocket(ListenSocket);
            WSACleanup();
            *error = true;
            return -1;
        }
        else {
            cout << "[Info]Connection accepted." << endl;
        }

        do {
            err = recv(ClientSocket, recvbuf, recvbuflen, 0);
            recvbuf[err] = '\0';

            if (err > 0) {
                cout << "[Info]recieved: " << recvbuf << endl;

                if (send(ClientSocket, ACPT_FLAG, 3, 0) == SOCKET_ERROR) {
                    cerr << "[Error]Cannot reply, error code:" << WSAGetLastError() << endl;
                    closesocket(ClientSocket);
                    WSACleanup();
                    *error = true;
                    return -1;
                }
                else {
                    cout << "[Info]Replied \"ACK\"" << endl;
                }

                if (strcmp(recvbuf, EXIT_FLAG) == 0) {
                    cout << "[Info]Shutting down, bye" << endl;
                    err = 0;
                }
                else {
                    recvdata.clear();

                    if (split2double(recvbuf, recvdata, split) == FAIL) {
                        cerr << "[Error]Cannot convert string to double" << endl;
                    }
                    EnterCriticalSection(&shared_buffer_lock);

                    *recvsize = recvdata.size();
                    if (*recvsize == 1) {
                        rdata.dz = recvdata[0];
                        cout << "[Info]Received z data, dz=" << recvdata[0] << endl;
                    }
                    else if (*recvsize == 2) {
                        rdata.dx = recvdata[0];
                        rdata.dy = recvdata[1];
                        cout << "[Info]Received x-y data, dx=" << recvdata[0]
                            << "  dy=" << recvdata[1] << endl;
                    }
                    else {
                        cerr << "[Error]Receive data error. size = " << recvdata.size() << endl;
                    }
                }
            }
            else if (err == 0) {
                cout << "[Info]Connection closed" << endl;
                if (send(ClientSocket, ACPT_FLAG, 3, 0) == SOCKET_ERROR) {
                    cerr << "[Error]Cannot reply, error code:" << WSAGetLastError() << endl;
                    closesocket(ClientSocket);
                    WSACleanup();
                    *error = true;
                    return -1;
                }
                else {
                    cout << "[Info]Replied \"Closed\"\n" << endl;
                }

                if (shutdown(ClientSocket, SD_SEND) == SOCKET_ERROR) {
                    cout << "[Error]Shutdown failed with error. Error code:" << WSAGetLastError() << endl;
                    closesocket(ClientSocket);
                    WSACleanup();
                    *error = true;
                    return -1;
                }
            }
            else {
                cerr << "[Error]recieve failed. Error code:" << WSAGetLastError() << endl;
                closesocket(ListenSocket);
                WSACleanup();
                *error = true;
                return -1;
            }
        } while (err > 0);

        // *wecho = true;
        if (strcmp(recvbuf, EXIT_FLAG) == 0) {
            break;
        }
    }

    delete[] recvbuf;
    closesocket(ClientSocket);
    WSACleanup();
    return 0;
}

int WinSockServer::recv() {
    DWORD dwThreadId;

    ghThreads_s = CreateThread(
        0,
        0,
        ThreadProcRecv,
        this,
        0,
        &dwThreadId
    );

    if (ghThreads_s == NULL) {
        cerr << "[Error]Cannot start a new thread to receive data" << endl;
        *error = true;
        return -2;
    }
    else {
        while (1) {
            if (*error) {
                return -1;
            }
            if (*ready) {
                return 0;
            }
        }
    }
}
