#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "UARTStream.h"
#include "Str.h"
#include "arduino.h"
#include "at_parser/parser.h"

uint8_t rxBuf1[1024];
uint8_t txBuf1[1024];
UARTStream uart1;

int f;
void onRespone(void* args);
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
    parser_init(&uart1);
    f=1;
    parser_setOnCommandFinish(onRespone,&f);
    printf("at: %d\n",parser_sendCommand_f(3000,"AT+CMGF=%d\r\n",1));
    
}


void onRespone(void* args){
    if(*((int*)args) == 1){
        printf("on response 1 %d\n",parser_getRetCode());
       f=2;
       printf("at: %d\n",parser_sendCommand_dataMode_f("hello world",3000,"at+cmgs=\"%s\"\r\n","09150077970"));
    }
    else {
        printf("on response 2 %d\n",parser_getRetCode());
        int a;
        printf("scanf: %d %d",a,parser_scanf("+CMGS: %d",&a));
    }
}
void loop()
{
    
    delay(1);
    parser_handle();
    
}
void onEnd()
{
    UARTStream_deInit(&uart1);
}
