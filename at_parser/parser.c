#include "parser.h"
#include <stdarg.h>
#include "parser_dbg.h"


typedef struct
{
    uint8_t *Buff;
    uint16_t Size;
    uint16_t Len;
    uint16_t PendingBytes;
    uint8_t InReceive;
} ATResponse;

typedef struct
{
    Parser_RxState rxState;
    Parser_TxState txState;
    UARTStream *stream;
    Parser_CommandStatus retCode;
    uint32_t timeout;
    uint32_t millis;
    ATResponse *tokenizerBuffer;
    ATResponse *comulativeBuffer;
    cb_onCommandFinish cb;
    void *args;
    uint16_t Ignorebytes;
} Parser_Obj;
 Parser_Obj obj;

static uint8_t tokenizerBuffer[512];
static uint8_t comulativeBuffer[512];


static const char *TERMINATION_LIST[] = {
    "OK", "ERROR", "SEND OK", "+CME ERROR:", "+CMS ERROR:"};
static const int TERMINATION_LIST_SIZE = sizeof(TERMINATION_LIST) / sizeof(char *);



ATResponse response = {
    .Buff = (uint8_t *)tokenizerBuffer,
    .Size = sizeof(tokenizerBuffer),
};
ATResponse comulative = {
    .Buff = (uint8_t *)comulativeBuffer,
    .Size = sizeof(comulativeBuffer),
};

// only call when on Command is false
static void initCommandMode()
{
    obj.retCode = Parser_busy;
    comulative.Buff[0] = '\0';
    comulative.Len = 0;
}

bool parser_scanf(const char* format, ...) {
    char *bf = (char* )comulative.Buff;
    // Get the number of expected variables from the format string
    int num_vars = 0;
    const char* f = format;
    while (*f) {
        if (*f++ == '%') {
            if (*f == '*') {
                f++; // Skip the '*' modifier
            } else {
                num_vars++;
            }
        }
    }
    va_list args;
    va_start(args, format);
    while(*bf){
       
        int matched = vsscanf(bf, format, args);
        bf++;
        if(matched == num_vars)
        {
            va_end(args);
            return true;
        }
    }
    va_end(args);
    return false;
}


void parser_setOnCommandFinish(cb_onCommandFinish cb, void *args)
{   if(parser_isBusy()){
        return;
    }
    obj.cb = cb;
    obj.args = args;
}
void parser_init(UARTStream *stream)
{
    obj.rxState = Parser_RxState_None;
    obj.txState = Parser_TxState_None;
    obj.stream = stream;
    obj.tokenizerBuffer = &response;
    obj.comulativeBuffer = &comulative;

    obj.retCode = Parser_Idle;
}
int parser_getRetCode(){
    return obj.retCode;
}
uint8_t* parser_getComuBuffer(){
    return comulative.Buff;
}
bool parser_sendCommand_dataMode(const char *cmd, const char *data, uint32_t timeout)
{
    if (parser_isBusy())
    {
        return false;
    }
    initCommandMode();
    obj.timeout = 150;
    obj.millis = millis();
    OStream_writeStr(&obj.stream->Output, cmd);
    OStream_flush(&obj.stream->Output);
    obj.rxState = Parser_RxState_inRec;
    while (obj.retCode == Parser_busy)
    {
        parser_handle();
        if (obj.comulativeBuffer->Len != 0)
            break;
    }
    if (obj.retCode != Parser_busy)
    { // timeout or return code
        return false;
    }
    // > is recevided now
    obj.timeout = timeout;
    obj.millis = millis();
    OStream_writeStr(&obj.stream->Output, data);
    OStream_writeChar(&obj.stream->Output, 0x1a);
    OStream_flush(&obj.stream->Output);
    #if IGNORE_DATA_ECHO 
        obj.Ignorebytes =  strlen(data);
    #endif
    return true;
}
bool parser_sendCommand_dataMode_f(const char *data, uint32_t timeout,const char *format,...){
    if (parser_isBusy())
    {
        return false;
    }
    va_list args;
    va_start(args, format);
    int written_chars = vsnprintf(comulative.Buff,comulative.Size,format,args);
    va_end(args);
    OStream_writeStr(&obj.stream->Output, comulative.Buff);
    return parser_sendCommand_dataMode("",data,timeout);

}

bool parser_sendCommand(const char *cmd, uint32_t timeout)
{
    if (parser_isBusy())
    {
        return false;
    }
    initCommandMode();
    obj.timeout = timeout;
    obj.millis = millis();
    OStream_writeStr(&obj.stream->Output, cmd);
    OStream_flush(&obj.stream->Output);
    obj.rxState = Parser_RxState_inRec;
    return true;
}
bool parser_sendCommand_f(uint32_t timeout,const char* format,...)
{
    if (parser_isBusy())
    {
        return false;
    }
    va_list args;
    va_start(args, format);
    int written_chars = vsnprintf(comulative.Buff,comulative.Size,format,args);
    va_end(args);
    OStream_writeStr(&obj.stream->Output, comulative.Buff);
    return parser_sendCommand("",timeout);
}


Stream_LenType ATResponse_Tokenizer(IStream *stream, ATResponse *response)
{
    Stream_LenType buffIndex = 0;
    Stream_LenType index;

    if (IStream_available(stream) > 0)
    {
        if(obj.Ignorebytes != 0){
            if(obj.Ignorebytes <= IStream_available(stream) ){
                IStream_ignore(stream, obj.Ignorebytes);
                obj.Ignorebytes = 0;
            }
            return 0;
        }
        if (response->InReceive == 0)
        {
            response->Len = 0;
            response->PendingBytes = 0;
            // Check send
            if (IStream_compareAt(stream, 0, (uint8_t *)Q_RES_SEND, sizeof(Q_RES_SEND) - 1) == 0)
            {
                response->Len = sizeof(Q_RES_SEND) - 1;
                IStream_readBytes(stream, response->Buff, response->Len);
                response->Buff[response->Len] = '\0';
                return response->Len;
            }
            // Read line
            while ((index = IStream_findPattern(stream, CRLF, sizeof(CRLF) - 1)) >= 0)
            {
                if (index > 0)
                {
                    index += 2;
                    if (IStream_compareAt(stream, 0, (uint8_t *)AT, sizeof(AT) - 1) != 0)
                    {
                        char *pBuffer = (char *)&response->Buff[response->Len];
                        IStream_readBytes(stream, &response->Buff[response->Len], index);
                        response->Len += index;
                        // Check Countinue
                        if (Str_compareFix(pBuffer, Q_RES_QIURC_RECV, sizeof(Q_RES_QIURC_RECV) - 1) == 0)
                        {
                            // read data bytes
                            char *pNum = Str_indexOfAt((const char *)pBuffer, ',', 2);
                            if (pNum)
                            {
                                unsigned int bytes;
                                const char *p;
                                Str_getUNum(pNum, &bytes, &p);
                                response->PendingBytes = bytes;
                                response->InReceive = 1;
                            }
                            return 0;
                        }
                        else if (Str_compareFix(pBuffer, Q_RES_CMT, sizeof(Q_RES_CMT) - 1) == 0)
                        {
                            // read data bytes
                            char *pNum = Str_indexOfAt((const char *)pBuffer, ',', 1);
                            if (pNum)
                            {
                                unsigned int bytes;
                                const char *p;
                                Str_getUNum(pNum, &bytes, &p);
                                bytes = bytes * 2 + 16;
                                response->PendingBytes = bytes;
                                response->InReceive = 1;
                            }
                            return 0;
                        }
                        else
                        {
                            response->Buff[response->Len] = '\0';
                            return response->Len;
                        }
                    }
                    else
                    {
                        IStream_ignore(stream, index);
                    }
                }
                else
                {
                    IStream_ignore(stream, sizeof(CRLF) - 1);
                }
            }
        }
        else
        {
            if ((index = IStream_findPatternAt(stream, response->PendingBytes, CRLF, sizeof(CRLF)-1)) >= 0)
            {
                index += 2;
                IStream_readBytes(stream, &response->Buff[response->Len], index);
                response->Len += index;
                response->Buff[response->Len] = '\0';
                response->InReceive = 0;
                return response->Len;
            }
        }
    }

    return 0;
}

bool parser_isBusy()
{
    if (obj.txState == Parser_TxState_None && obj.rxState == Parser_RxState_None)
    {
        return false;
    }
    return true;
}

// bool parser_isBusy_real()
// {
//     if (!parser_isBusy() &&
//         response.InReceive == 0 && IStream_available(&obj.stream->Input) == 0)
//     {
//         return false;
//     }
//     return true;
// }
int check_termination(const uint8_t *data, int *index)
{
    for (int i = 0; i < TERMINATION_LIST_SIZE; i++)
    {
        if (strstr(data, TERMINATION_LIST[i]) == (char *)data)
        {
            // str start with termination list
            *index = i;
            return true;
        }
    }
    return false;
}
bool parser_checkUrc()
{
    Mem_LenType URC_find(const URC *urcs, uint16_t len, Str *str);
    Str str = {
        .Text = (char *)response.Buff,
        .Len = response.Len,
    };
    if (URC_find(Q_URC, Q_URC_LEN, &str) >= 0)
    {
        // found
        return true;
    }
    return false;
}

static void recv_handle()
{
    int len = ATResponse_Tokenizer(&obj.stream->Input, &response);
    if (len > 0)
    {
        if (parser_checkUrc())
        {
            _print("urc: ");
            _print_arr_norm(response.Buff, response.Len);
            _print_no_prefix("\n");
        }
        else if (parser_isBusy())
        {
            _print("busy: ");
            _print_arr_norm(response.Buff, response.Len);
            _print_no_prefix("\n");
            if (comulative.Size - comulative.Len - 1 >= len)
            {
                strcat(comulative.Buff, response.Buff);
                comulative.Len += len;
                obj.millis = millis();
            }
            else
            {
                // handle if buffer get full
                strcat_s(comulative.Buff, comulative.Size - comulative.Len - 1, response.Buff);
                comulative.Len = comulative.Size;
                obj.millis = millis();
            }

            if (check_termination(response.Buff, (int *)&obj.retCode))
            {
                obj.txState = Parser_TxState_None;
                obj.rxState = Parser_RxState_None;
                if (obj.cb != NULL)
                {
                    obj.cb(obj.args);
                }
                return;
            }
        }
        else
        { // not suppose to be here
        }
    }
    else
    {
        if (parser_isBusy())
        {
            // check timeout
            if (millis() - obj.millis >= obj.timeout)
            {
                // timeout happend;
                obj.txState = Parser_TxState_None;
                obj.rxState = Parser_RxState_None;
                obj.retCode = Parser_timeout;
                obj.Ignorebytes = 0;
                if (obj.cb != NULL)
                {
                    obj.cb(obj.args);
                }
            }
        }
    }
}

void parser_handle()
{
    static uint32_t bufferCheckStart = 0;
    if (millis() - bufferCheckStart >= 2)
    {
        bufferCheckStart = millis();
        recv_handle();
    }
}

