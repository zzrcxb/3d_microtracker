#include "3d_tracker_socket.h"
#include <string>
#include <iostream>

using namespace std;

bool renable;
bool recho;
char shared_buffer[512] = {0};
CRITICAL_SECTION shared_buffer_lock;
HANDLE ghThreads_c;

int main() {
    string buffer("HELLO");
    string buffer2("DISCONNECT");

    cout << "here 0" << endl;

    InitializeCriticalSection(&shared_buffer_lock);
    recho = false;
    
    cout << "here 1" << endl;

    startsocketclient("127.0.0.1", "27015");

    cout << "here 2" << endl;

    EnterCriticalSection(&shared_buffer_lock);
    renable = true;
    for (int i = 0; i < buffer.size(); i++)
        shared_buffer[i] = buffer[i];
    shared_buffer[buffer.size()] = 0;
    LeaveCriticalSection(&shared_buffer_lock);
    while (!recho); // Wait until finished.
    
    cout << "Here 3" << endl;
    Sleep(10);
    EnterCriticalSection(&shared_buffer_lock);
    renable = true;
    for (int i = 0; i < buffer2.size(); i++)
        shared_buffer[i] = buffer2[i];
    shared_buffer[buffer2.size()] = 0;
    LeaveCriticalSection(&shared_buffer_lock);
    

    WaitForSingleObject(ghThreads_c, INFINITE);

    DeleteCriticalSection(&shared_buffer_lock);
    return 0;
}