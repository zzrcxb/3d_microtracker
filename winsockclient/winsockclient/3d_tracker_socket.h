#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

#define FAIL -1
#define EXIT_FLAG "DISCONNECT"
#define ACPT_FLAG "ACK"
#define LOSE_FLAG "CLOSED"
#define DEFAULT_BUFLEN 512


class WinSockClient {
public:
    // Client shared variables
    struct addrinfo* ptr;

    // Socket state
    bool error, ready;

    // Server info
    char *port, *ipv4;

    // Client properties
    int sendbuflen;
    char* msg;
    
    // Muiltithread
    CRITICAL_SECTION shared_buffer_lock;
    HANDLE ghThreads_c;

    WinSockClient(){}
    WinSockClient(char* port, char* ipv4, int sendbuflen) {
        InitializeCriticalSection(&shared_buffer_lock);

        ptr = NULL;

        this->port = new char[strlen(port) + 1];
        this->ipv4 = new char[strlen(ipv4) + 1];
        this->sendbuflen = sendbuflen;

        msg = new char[sendbuflen];

        error = false;
        ready = false;

        strcpy_s(this->port, strlen(port) + 1, port);
        strcpy_s(this->ipv4, strlen(ipv4) + 1, ipv4);

        startsocketclient();
    }

    ~WinSockClient() {
        WaitForSingleObject(ghThreads_c, INFINITE);

        delete[] this->port;
        delete[] this->ipv4;
        delete[] this->msg;

        DeleteCriticalSection(&shared_buffer_lock);
    }

    int startsocketclient();
    int send(char* bufstr);
    int stopsocketclient();
};

int sendmessage(WinSockClient* winsock_c, char* msg);