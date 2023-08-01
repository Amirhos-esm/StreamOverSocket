#include <winsock2.h>
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
// bool dumpData = false;
typedef struct
{
    SOCKET sock;
    struct sockaddr_in server;
    HANDLE recThread;
    HANDLE sendThread;
    volatile bool isRunnig;
    const char *name;
    volatile uint8_t *txBuffer;
    volatile uint8_t *rxBuffer;
    volatile size_t inTx;
    volatile size_t inRx;
    void (*txComp)(void *);
    void (*rxComp)(void *);
    void *args;
} UARTStreamOverSocket;

unsigned __stdcall SendThread(void *data)
{
    UARTStreamOverSocket *stream = (UARTStreamOverSocket *)data;
    SOCKET s = stream->sock;
    while (1)
    {
        if (stream->inTx != 0)
        {
            int sent = 0;
            if ((sent = send(s, stream->txBuffer, stream->inTx, 0)) < 0)
            {
                printf("%s->Send failed\n", stream->name);
                break;
            }
            // stream->inTx  -= sent;
            stream->inTx = 0;
            if (stream->txComp != NULL)
            {
                stream->txComp(stream->args);
            }
        }

        if (!stream->isRunnig)
            break;
        Sleep(1);
    }
    printf("exit on line: %d",  __LINE__);
    exit(0);
    return 0;
}

// Function executed by the receiving thread
unsigned __stdcall ReceiveThread(void *data)
{
    UARTStreamOverSocket *stream = (UARTStreamOverSocket *)data;
    SOCKET s = stream->sock;
    char server_reply[1024];
    int recv_size;
    stream->isRunnig = true;

    // while ((recv_size = recv(s, server_reply, sizeof(server_reply) - 1, 0)) > 0)
    // {
    //     // server_reply[recv_size] = '\0';
    //     // if (dumpData)
    //     // {
    //     //     printf("\n%s->rec:\"", stream->name);
    //     //     printf(server_reply);
    //     //     printf("\"");
    //     // }
    //     if (stream->inRx != 0)
    //     {
    //         printf("rec:%d %d\n",recv_size,stream->inRx);
    //         memcpy(stream->rxBuffer, server_reply, recv_size);
    //         stream->inRx -= recv_size;
    //         stream->rxBuffer = &stream->rxBuffer[recv_size];
    //         if (stream->inRx == 0)
    //         {
    //             if (stream->rxComp != NULL)
    //             {
    //                 stream->rxComp(stream->args);
    //             }
    //         }
    //     }
    // }
    while (1)
    {
        if (stream->inRx != 0)
        {
            if ((recv_size = recv(s, stream->rxBuffer, stream->inRx, 0)) > 0)
                {
                    stream->inRx -= recv_size;
                    stream->rxBuffer = &stream->rxBuffer[recv_size];
                    if (stream->inRx == 0)
                    {
                        if (stream->rxComp != NULL)
                        {
                            stream->rxComp(stream->args);
                        }
                    }
                }
            if (recv_size == 0)
            {
                printf("%s->Server disconnected\n", stream->name);
                stream->isRunnig = false;
                break;
            }
        }
        else
        {
            Sleep(1);
        }
    }
   printf("exit on line: %d",  __LINE__);
    exit(0);

    return 0;
}
size_t socketinReceive(void *obj)
{
    UARTStreamOverSocket *stream = (UARTStreamOverSocket *)obj;
    return stream->inRx;
}
void socketReceive(void *obj, uint8_t *buffer, size_t len)
{
    UARTStreamOverSocket *stream = (UARTStreamOverSocket *)obj;
    stream->rxBuffer = buffer;
    stream->inRx = len;
}
void socketTransmit(void *obj, uint8_t *buffer, size_t len)
{
    UARTStreamOverSocket *stream = (UARTStreamOverSocket *)obj;
    stream->txBuffer = buffer;
    stream->inTx = len;
}
void *socketInit(void *args, const char *ip, uint16_t port, const char *name, void (*txComp)(void *), void (*rxComp)(void *))
{
    UARTStreamOverSocket *stream = malloc(sizeof(UARTStreamOverSocket));
    if (stream == NULL)
    {
        printf("%s->cannot alloc memory\n", name);
        return NULL;
    }
    // stream->HUART = huart;
    stream->name = name;
    stream->isRunnig = false;
    stream->txComp = txComp;
    stream->rxComp = rxComp;
    stream->args = args;
    stream->inTx = stream->inRx = 0;
    // Create a socket
    if ((stream->sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        printf("%s->Could not create socket : %d\n", stream->name, WSAGetLastError());
        return NULL;
    }
    printf("%s->Socket created.\n", stream->name);

    stream->server.sin_addr.s_addr = inet_addr(ip); // Replace with the IP address of your server
    stream->server.sin_family = AF_INET;
    stream->server.sin_port = htons(port); // Replace with the port number of your server

    // Connect to remote server
    if (connect(stream->sock, (struct sockaddr *)&stream->server, sizeof(stream->server)) < 0)
    {
        printf("%s->Connect error\n", stream->name);
        return NULL;
    }

    printf("%s->Connected\n", stream->name);

    stream->recThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReceiveThread, stream, 0, NULL);
    if (stream->recThread == (HANDLE)NULL)
    {
        printf("%s->Failed to create thread\n", stream->name);
        return NULL;
    }
    Sleep(1);
    stream->sendThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)SendThread, stream, 0, NULL);
    if (stream->sendThread == (HANDLE)NULL)
    {
        printf("%s->Failed to create thread\n", stream->name);
        return NULL;
    }
    return stream;
}
void socketDeInit(void *obj)
{
    closesocket(((UARTStreamOverSocket *)obj)->sock);
    free(obj);
}
