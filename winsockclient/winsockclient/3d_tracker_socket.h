#pragma once
#include <windows.h>
#include <vector>
#include <string>
#define FAIL -1

typedef struct network {
    char* port, *ipv4;
    int recvbuflen;
} NetPara;

typedef struct resdata {
    double dx, dy, dz;
} resdata;

int startlocalserver(char* port, int recvbuflen);

int startsocketclient(char* ipv4, char* port);

extern bool renable;
extern bool recho;
extern bool wenable;
extern bool wecho;
extern char shared_buffer[512];
extern CRITICAL_SECTION shared_buffer_lock;
extern HANDLE ghThreads_c;
extern HANDLE ghThreads_s;
extern resdata rdata;

int fChanger(const char* str, double &res);
int charChanger(double num, char* &res);
int intChanger(const char* str, int &res);
int split2double(const char* str, std::vector<double> &data, char ch);