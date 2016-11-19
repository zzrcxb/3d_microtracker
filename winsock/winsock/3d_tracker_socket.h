#pragma once
#include <string>

int startlocalserver(std::string port, int recvbuflen);

void startsocketclient(std::string ipv4, std::string port, std::string message);