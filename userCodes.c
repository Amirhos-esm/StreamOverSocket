#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "UARTStream.h"
#include "Str.h"
#include "arduino.h"


uint8_t rxBuf1[64];
uint8_t txBuf1[64];
UARTStream uart1;


void setup()
{
    millis(); // reference it from now
    setbuf(stdout, NULL); // Disable buffering for stdout
    bool ret = UARTStream_init(
        &uart1, "127.0.0.1", 8888, "COM16",
        rxBuf1, sizeof(rxBuf1),
        txBuf1, sizeof(txBuf1));
    if (ret == false)
    {
        printf("ret == false");
        exit(0);
    }


    OStream_flush(&uart1.Output);
    IStream_receive(&uart1.Input);
    // IStream_receiveBytes(&uart1.Input, STR("\r\n+SSS: hello world\r\n\r\nOK\r\n\r\n+COPS: hello\r\n\r\n"));

//
//    ec200_sock_init(sock2,callbacks);
//    ec200_sock_connect(sock2,"80.253.141.222",10000);

//    memset(data,'a',sizeof(data));
}
void loop()
{
    if(IStream_available(&uart1.Input)) {
        OStream_writeStream(&uart1.Output, &uart1.Input, IStream_available(&uart1.Input));
        OStream_flush(&uart1.Output);
    }

}

