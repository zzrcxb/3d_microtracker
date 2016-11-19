#pragma once
#include <string>

int startlocalserver(char* port, int recvbuflen);

int startsocketclient(char* ipv4, char* port);