#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "UARTStream.h"
#include "Str.h"
#include "arduino.h"
#include "at_parser/parser.h"
#include "ec200.h"

uint8_t rxBuf1[1024];
uint8_t txBuf1[1024];
UARTStream uart1;

AT_Socket *sock;

void onConnectError(void* args,int error,const char* error_str){
    printf("error: %d %s\r\n",error,error_str);
    AT_Socket* socket = args;
    socket->state = Socket_connected;
    ec200_sock_close(socket);

}
Socket_callbacks callbacks = {
    .onConnectError = onConnectError 
};
void setup()
{
    setbuf(stdout, NULL); // Disable buffering for stdout
    bool ret = UARTStream_init(
        &uart1, "127.0.0.1", 8888, "COM",
        rxBuf1, sizeof(rxBuf1),
        txBuf1, sizeof(txBuf1));
    if (ret == false)
    {
        printf("ret == false");
        exit(0);
    }
   
    // OStream_writeStr(&uart1.Output, "AT\r\n");
    // OStream_flush(&uart1.Output);
    IStream_receive(&uart1.Input);
    // IStream_receiveBytes(&uart1.Input, STR("\r\n+SSS: hello world\r\n\r\nOK\r\n\r\n+COPS: hello\r\n\r\n"));
    ec200_init(&uart1);  
    sock = ec200_sock_get(0);
    ec200_sock_init(sock,callbacks); 
    ec200_sock_connect(sock,"80.253.141.222",10000);
}

uint32_t _millis;
void loop()
{
    ec200_handle();
    delay(1);
    if(millis() - _millis >2000 && sock->ackData.notAcked == 0){
        static int i;
        _millis = millis();
        ec200_sock_send_f(sock,"hello world %d\r\n",i++);
    }
    
}
void onEnd()
{
    UARTStream_deInit(&uart1);
}
