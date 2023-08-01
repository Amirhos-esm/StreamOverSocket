#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "UARTStream.h"
#include "Str.h"
#include "general.h"

uint8_t rxBuf1[1024];
uint8_t txBuf1[1024];
UARTStream uart1;

#define STR(X) X, strlen(X)

void setup()
{
    setbuf(stdout, NULL); // Disable buffering for stdout
    bool ret = UARTStream_init(
        &uart1, "127.0.0.1", 8888, "COM",
        rxBuf1, sizeof(rxBuf1),
        txBuf1, sizeof(txBuf1));
    if (ret == false)
    {
        exit(0);
    }

    OStream_writeStr(&uart1.Output, "AT\r\n");
    OStream_flush(&uart1.Output);
    IStream_receive(&uart1.Input);
    IStream_receiveBytes(&uart1.Input, STR("xT\r\n+CMTI: hello world\r\n\r\nOK\r\n\r\n+QIURC: \"recv\",0,5\r\nhello\r\n"));
}

typedef struct
{
    uint8_t *Buff;
    uint16_t Size;
    uint16_t Len;
    uint16_t PendingBytes;
    uint8_t InReceive;
} ATResponse;

static char responseBuff[4096];
static ATResponse response = {
    .Buff = (uint8_t *)responseBuff,
    .Size = sizeof(responseBuff),
};



int i = 0;
void print_hex(uint8_t *buffer, int len)
{
    printf("array(");
    for (int i = 0; i < len; i++)
    {
        printf("%02X", buffer[i]);
        if (i != len - 1)
        {
            printf(" ");
        }
    }
    printf(")\n");
}
int x = 0;
void loop()
{
    // OStream_writeStr(&uart1.Output,"AT+IPR=?\r\n");
    // OStream_flush(&uart1.Output);
    delay(100);
    // printf("%d stream:%d\n",x++,IStream_available(&uart1.Input ));
    //  if(IStream_available(&uart1.Input))
    //  {
    //      int len = IStream_available(&uart1.Input);
    //      uint8_t buffer[1024];
    //      Stream_Result res = IStream_readBytes(&uart1.Input,buffer,len);

    //      if(res == Stream_Ok)
    //      print_hex(buffer,len);
    //  }
    // delay(1000);
    // printf("stream:%d\n",IStream_available(&uart1.Input));
    // uint8_t salam[24];
    // int len = IStream_getBytes(&uart1.Input,salam,IStream_findPattern(&uart1.Input,"\r\n",2));
    // // int len = IStream_getBytesUntilPattern(&uart1.Input,"\r\n",2,salam,24);
    // print_hex(salam,len);
    if (ATResponse_Parse(&uart1.Input, &response) > 0)
    {
        print_hex(response.Buff, response.Len);
    }
}
void onEnd()
{
    UARTStream_deInit(&uart1);
}
