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

AT_Socket *sock1,*sock2;

void onConnectError(void* args,int error,const char* error_str){
    printf("error: %d %s\r\n",error,error_str);
    AT_Socket* socket = args;
    socket->state = Socket_connected;
    ec200_sock_close(socket);

}
void onData(void* args,Str str){
    print_arr_norm(str.Text,str.Len);
    print_f("\n");
}
Socket_callbacks callbacks = {
    .onConnectError = onConnectError ,
    .onData = onData 
};
uint8_t data[250];

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
    sock1 = ec200_sock_get(0);
    sock2 = ec200_sock_get(1);
    ec200_sock_init(sock1,callbacks); 
    ec200_sock_connect(sock1,"80.253.141.222",10000);
    while(parser_isBusy()){
        ec200_handle();
    }

    ec200_sock_init(sock2,callbacks); 
    ec200_sock_connect(sock2,"80.253.141.222",10000);

//    memset(data,'a',sizeof(data));
}
uint32_t _millis,_millis2;
void loop()
{
    ec200_handle();
    if(!parser_isBusy()) {
        if (millis() - _millis > 250 && sock1->ackData.notAcked == 0) {
            static int i;
//            ec200_sock_send_f(sock1, "this is a test data it should have at list= %d\r\n", millis() - _millis);
            ec200_sock_send(sock1, (Str){
                .Text = data,
                .Len = 250
            });
            _millis = millis();
        }
        if (millis() - _millis2 > 10000) {
            static int x;
            _millis2 = millis();

            if (ec200_sock_isConnected(sock2)) {
                ec200_sock_send_f(sock2, "hello world from sock 2.counter= %d\r\n", x++);
            } else {
                ec200_sock_connect(sock2, "80.253.141.222", 10000);
            }
        }
    }
    
}
void onEnd()
{
    UARTStream_deInit(&uart1);
}
