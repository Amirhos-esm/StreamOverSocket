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

// typedef struct 
// {
//     void* parent; // to at Socket
//     IStream Input;
//     OStream Output;
    
// } Socket_Stream;

typedef enum{
    Socket_idle=0 ,
    Socket_connecting,
    Socket_connected,
    Socket_closing,
    Socket_closed,
} Socket_State;

typedef enum{
    Socket_Rx_none=0 ,
    Socket_Rx_connecting_1,
    Socket_Rx_connecting_2,
    Socket_Rx_connected,
    Socket_Rx_connected_ackcheck,
    Socket_Rx_sending,
    Socket_Rx_closing,
} Socket_RxStatus;

typedef struct {
    uint32_t totalSend;
    uint32_t acked;
    uint32_t notAcked;
    uint32_t millis;
    uint32_t lastAckedTime;
    uint8_t checkNeeded : 1;  
    uint8_t timerIndex : 7;  
} Ack_Data;

typedef void (*cb_socket_def)(void* args);
typedef void (*cb_socket_error)(void* args,int error,const char* error_str);
typedef void (*cb_socket_data)(void* args,Str str);
typedef struct{
    cb_socket_def onConnect;
    cb_socket_error onConnectError;
    cb_socket_def onClosed;
    cb_socket_def onSendTimeout; // timeout , socket close automatily
    cb_socket_def onSend; // send into tcp buffer not actuly received by server
    cb_socket_error onSendError; // send into tcp buffer error
    cb_socket_data onData;
    cb_socket_def onTimeout; // at_parser timeout
    cb_socket_error onError; 
}Socket_callbacks;
typedef struct
{
    void *args;
    // Socket_Stream stream;
    uint8_t sock_id;
    Socket_State state;
    // uint32_t millis;
    Ack_Data ackData;
    char domain[64];
    uint16_t port;
    Socket_RxStatus rxStatus;
    Socket_callbacks callbacks;
} AT_Socket;

void ec200_sock_init(AT_Socket *sock, Socket_callbacks callbacks/*,
                     uint8_t *rxBuff, Stream_LenType rxBuffSize,
                     uint8_t *txBuff, Stream_LenType txBuffSize*/);
void _ec200_sock_init();
void _ec200_sock_handle();
AT_Socket* ec200_sock_get(int sock_id);
bool ec200_sock_close(AT_Socket *sock);
bool ec200_sock_connect(AT_Socket *sock, const char *domain, uint16_t port);
bool ec200_sock_send(AT_Socket *sock, Str str);
bool ec200_sock_send_f(AT_Socket *sock,const char *format,...);
bool ec200_sock_isConnected(AT_Socket *sock);


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
