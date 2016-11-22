#include <iostream>
#include <string>
#include <vector>
#include "3d_tracker_socket.h"


using namespace std;


int main() {
    WinSockServer winsockserver("27015", 512, ' ');

    winsockserver.recv();   

    cout << "Here" << endl;
    return 0;
}