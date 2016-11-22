#include "3d_tracker_socket.h"
#include <string>
#include <iostream>

using namespace std;


int main() {
    WinSockClient winsockclient("27015", "127.0.0.1", 512);
    
    winsockclient.startsocketclient();


    if (!*winsockclient.error) {
        winsockclient.sendmessage("12.934 890");
        winsockclient.sendmessage("4");
        winsockclient.sendmessage("DISCONNECT");

        winsockclient.stopsocketclient();
    }

    return 0;
}