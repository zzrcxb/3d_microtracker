// Include header file
#include "3d_tracker_socket.h"
#include <iostream>

// Please choose Release x64, place .lib ans your codes in the same directory.
// Also, you can set c++ linker to this lib
#pragma comment(lib, "WinSockClient.lib")

using namespace std;

int main() {
    // Create an instance of WinSockServer
    //         Listening port | Received buffer size|Character to split received buffer
    WinSockServer win_s("27015", 512, ' ');

    if (!win_s.error) {
        win_s.recv(); // Start to receive data

        while (1) {
            if (win_s.error)
                break;
            if (win_s.isshutdown)
                break; // Has been closed

            // Income data!
            if (win_s.wecho) {
                win_s.wecho = false;
                if (win_s.recvsize == 2) {
                    cout << "dx=" << win_s.rdata.dx << "\tdy=" << win_s.rdata.dy << endl;
                }
                else if(win_s.recvsize == 1) {
                    cout << "dz=" << win_s.rdata.dz << endl;
                }
            }
            Sleep(1);
        }
    }

    return 0;
}