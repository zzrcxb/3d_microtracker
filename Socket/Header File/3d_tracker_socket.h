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


typedef struct resdata {
    double dx, dy, dz;
} resdata;


class WinSockServer {
public:
    //Server shared variable
    SOCKET ListenSocket;

    // Server state:
    bool error, ready;

    // Server property:
    char* port;
    int recvbuflen;
    char split;
    
    //Receive data
    resdata rdata;
    int recvsize;

    //Muiltithreads
    CRITICAL_SECTION shared_buffer_lock;
    HANDLE ghThreads_s;
    bool wecho;


    WinSockServer() {}
    WinSockServer(char* port, int recvbuflen, char split) {
        InitializeCriticalSection(&shared_buffer_lock);

        ListenSocket = INVALID_SOCKET;

        this->port = new char[strlen(port) + 1];
        strcpy_s(this->port, strlen(port) + 1, port);

        wecho = false;
        error = false;
        ready = false;

        this->recvbuflen = recvbuflen;
        this->split = split;
        recvsize = 0;

        startlocalserver();
    }

    ~WinSockServer() {
        WaitForSingleObject(ghThreads_s, INFINITE);

        delete[] this->port;

        DeleteCriticalSection(&shared_buffer_lock);
    }

    int startlocalserver();
    int recv();
};


int fChanger(const char* str, double &res);
int charChanger(double num, char* &res);
int intChanger(const char* str, int &res);
int split2double(const char* str, std::vector<double> &data, char ch);
int receive(WinSockServer* winsock_s);

class WinSockClient {
public:
    // Client shared variables
    struct addrinfo* ptr;

    // Socket state
    bool error, ready, done;

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
        done = false;

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
