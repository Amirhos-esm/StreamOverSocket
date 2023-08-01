#ifndef SERIALPORT_EC200_SOCK_H
#define SERIALPORT_EC200_SOCK_H
#ifdef __cplusplus
extern "C"
{
#endif
#include "ec200_cfg.h"
#if INCLUDE_NET

#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "InputStream.h"
#include "OutputStream.h"
#include "Str.h"
#include "arduino.h"
#include "../at_parser/parser.h"

    typedef struct 
    {
        void* args; // to AT_Socket
        IStream input;
        OStream output;
        
    } Socket_Stream;
    
    typedef struct
    {
        Socket_Stream stream;
        uint8_t sock_id;
        // bool isConnected;
        Socket_State state;
        void *args;
        uint32_t sended;
        uint32_t acked;
        uint32_t received;
        /* data */
    } AT_Socket;

// #if MAX_SOCKET == 0
// #error "MAX_SOCKET can`t be 0"
// #elif MAX_SOCKET == 1
//     extern AT_Socket socket1;
// #elif MAX_SOCKET == 2
//     extern AT_Socket socket1;
//     extern AT_Socket socket2;
// #elif MAX_SOCKET == 3
//     extern AT_Socket socket1;
//     extern AT_Socket socket2;
//     extern AT_Socket socket3;
// #else
// #error "MAX_SOCKET can`t be more than 3"
// #endif

#if MAX_SOCKET == 0
    #error "MAX_SOCKET can`t be 0"
#elif MAX_SOCKET <= 3
    extern AT_Socket sockets[MAX_SOCKET];
#else
    #error "MAX_SOCKET can`t be more than 3"
#endif

#endif
#ifdef __cplusplus
}
#endif
#endif // SERIALPORT_EC200_NET_H
