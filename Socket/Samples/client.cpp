// Include header file
#include "3d_tracker_socket.h"
#include <iostream>

// Please choose Release x64, place .lib ans your codes in the same directory.
// Also, you can set c++ linker to this lib
#pragma comment(lib, "WinSockClient.lib")

using namespace std;

int main() {
    // Create an instance of WinSockClient
    //                Server Port|Server ipv4|Received buffer size
    WinSockClient win_c("27015", "127.0.0.1", 512);

    if (!win_c.error) {
        // Sending message
        win_c.send("12.89 53");

        // Waiting for the previous job
        while (!win_c.done) Sleep(5);
        win_c.done = false;
        win_c.send("23");

        while (!win_c.done) Sleep(5);
        win_c.done = false;
        win_c.send("DISCONNECT"); // Shutdown server
    }
    
    return 0;
}
