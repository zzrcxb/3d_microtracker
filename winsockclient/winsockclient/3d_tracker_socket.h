#pragma once
#include <windows.h>
#include <cstring>
#include <vector>
#include <string>
#define FAIL -1


typedef struct resdata {
    double dx, dy, dz;
} resdata;


class WinSockClient {
public:
    char *port, *ipv4;
    int recvbuflen;
    char* shared_buffer;
    CRITICAL_SECTION shared_buffer_lock;

    HANDLE ghThreads_c;
    bool *renable, *recho, *isEnter, *error, *ready;

    WinSockClient(){}
    WinSockClient(char* port, char* ipv4, int recvbuflen) {
        InitializeCriticalSection(&shared_buffer_lock);

        this->port = new char[strlen(port) + 1];
        this->ipv4 = new char[strlen(ipv4) + 1];
        renable = new bool;
        recho = new bool;
        isEnter = new bool;
        error = new bool;
        ready = new bool;
        shared_buffer = new char[recvbuflen];

        *recho = false;
        *renable = false;
        *isEnter = false;
        *error = false;
        *ready = false;

        strcpy_s(this->port, strlen(port) + 1, port);
        strcpy_s(this->ipv4, strlen(ipv4) + 1, ipv4);

        for (int i = 0; i < recvbuflen; i++)
            shared_buffer[i] = 0;
    }

    ~WinSockClient() {
        delete[] this->port;
        delete[] this->ipv4;
        delete this->renable;
        delete this->recho;
        delete[] this->shared_buffer;
        delete this->error;
        delete this->ready;
        DeleteCriticalSection(&shared_buffer_lock);
    }

    int startsocketclient();
    int sendmessage(char* bufstr);
    int stopsocketclient();
};

int fChanger(const char* str, double &res);
int charChanger(double num, char* &res);
int intChanger(const char* str, int &res);
int split2double(const char* str, std::vector<double> &data, char ch);
