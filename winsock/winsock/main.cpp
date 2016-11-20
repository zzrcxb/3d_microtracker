#include <iostream>
#include <string>
#include <vector>
#include "3d_tracker_socket.h"

using namespace std;

HANDLE ghThreads_s;
resdata rdata;
CRITICAL_SECTION shared_buffer_lock;
bool wenable;
bool wecho;

int main() {
    InitializeCriticalSection(&shared_buffer_lock);
    
    startlocalserver("27015", 512);
    while (!wecho);
    wecho = false;
    cout << rdata.dx << "\t" << rdata.dy << endl;
    WaitForSingleObject(ghThreads_s, INFINITE);

    DeleteCriticalSection(&shared_buffer_lock);

    return 0;
}