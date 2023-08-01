#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <process.h>
#include "stdint.h"
#include <windows.h>


#pragma comment(lib, "ws2_32.lib")

void setup();
void loop();
void onEnd();


unsigned __stdcall LoopThread(void *data)
{
    int sleepIndexer=0;
    while (1)
    {
        loop();

        sleepIndexer++;
        if(sleepIndexer == 1000)
        {
            sleepIndexer = 0;
            Sleep(1);
        }
    }

    return 0;
}


int main()
{
    WSADATA wsa;

    printf("Initializing Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Failed. Error Code: %d", WSAGetLastError());
        return 1;
    }

    printf("Initialized.\n");
    setup();


    HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)LoopThread, NULL, 0, NULL);
    if (hThread == NULL)
    {
        puts("Failed to create Loop thread\n");
        return 1;
    }

    // Wait for the thread to exit (optional)
    WaitForSingleObject(hThread, INFINITE);



    onEnd();
    // Close the socket
    // closesocket(s);
    WSACleanup();

    return 0;
}
