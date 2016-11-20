#pragma once
#include <windows.h>

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
extern resdata rdata;