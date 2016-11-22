#include "3d_tracker_socket.h"
#include <string>
#include <iostream>

using namespace std;


int main() {
    WinSockClient winsockclient("27015", "127.0.0.1", 512);


    if (!winsockclient.error) {
        winsockclient.send("90 798.0");
        winsockclient.send("DISCONNECT");
    }

    return 0;
}