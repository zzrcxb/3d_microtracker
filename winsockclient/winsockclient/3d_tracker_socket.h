#pragma once
#include <string>

typedef struct network {
    char* port, *ipv4;
    int recvbuflen;
} NetPara;

int startlocalserver(char* port, int recvbuflen);

int startsocketclient(char* ipv4, char* port);

extern bool wenable = true;
extern char shared_buffer[512] = { 0 };
