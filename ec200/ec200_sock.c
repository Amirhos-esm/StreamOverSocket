#include "ec200_sock.h"
#include "stdarg.h"
#include "ec200_dbg.h"
#if INCLUDE_NET

static const uint16_t checkSendIntervals[] = {
     250, 500, 1000, 2000};
static const uint8_t checkSendIntervalsSize = sizeof(checkSendIntervals) / sizeof(uint16_t);

#define _Fire_(SOCK, NAME, ...)            \
    if (SOCK->callbacks.NAME != NULL)      \
    {                                      \
        SOCK->callbacks.NAME(__VA_ARGS__); \
    }                                      \
    else                                   \
    {                                      \
        _print("onCb: " #NAME "\n");     \
    }

AT_Socket sockets[MAX_SOCKET] = {
#if MAX_SOCKET >= 1
    {.sock_id = 0}
#endif
#if MAX_SOCKET >= 2
    ,
    {.sock_id = 1}
#endif
#if MAX_SOCKET >= 3
    ,
    {.sock_id = 2}
#endif
};
static void onResponse(void *args)
{
    parser_setOnCommandFinish(NULL, NULL);
    AT_Socket *sock = args;
    int rCode = parser_getRetCode();
    if (rCode == Parser_timeout)
    {
        _Fire_(sock, onTimeout, sock);
        return;
    }
    switch (sock->state)
    {
    case Socket_idle:
        /* code */
        break;
    case Socket_connecting: // call after qiopen
        if (sock->rxStatus == Socket_Rx_connecting_1)
        {
            sock->rxStatus = Socket_Rx_none;
            if (rCode == Parser_Ok)
            {
                parser_setOnCommandFinish(onResponse, sock);
                if (parser_sendCommand_f(5000, "AT+QIOPEN=1,%d,\"TCP\",\"%s\",%d,0,1\r\n", sock->sock_id, sock->domain, sock->port))
                {
                    sock->rxStatus = Socket_Rx_connecting_2;
                }
                else
                {
                    parser_setOnCommandFinish(NULL, NULL);
                    sock->state = Socket_idle;
                    _Fire_(sock, onConnectError, sock, rCode, "can`t connect");
                }
            }
            else
            {
                sock->state = Socket_idle;
                _Fire_(sock, onConnectError, sock, rCode, "can`t close socket before connect");
            }
        }
        else if (sock->rxStatus == Socket_Rx_connecting_2)
        {
            sock->rxStatus = Socket_Rx_none;
            if (rCode == Parser_Ok)
            {
                // wait for QIOPEN URC
            }
            else
            {
                // // sock->rxStatus = Socket_Rx_none;
                // sock->state = Socket_connected; // because sock_close function only act when state is connected
                // ec200_sock_close(sock);
                sock->state = Socket_idle;
                _Fire_(sock, onConnectError, sock, rCode, "can`t open socket call AT+QIGETERROR for more detail");
            }
        }

        /* code */
        break;
    case Socket_connected:
        /* code */
        if (sock->rxStatus == Socket_Rx_sending)
        {
            sock->rxStatus = Socket_Rx_none;
            // sock->millis = millis();
            if (rCode == 2 /*SEND OK*/)
            {
                if(sock->ackData.checkNeeded == false){
                    sock->ackData.millis = millis();
                    sock->ackData.checkNeeded = true;
                    sock->ackData.timerIndex = 0;
                }
                _Fire_(sock, onSend, sock);
            }
            else
            {
                _Fire_(sock, onSendError, sock, rCode,"AT+QISEND failed");
            }
            //     sock->rxStatus = Socket_Rx_none;
        }
        else if (sock->rxStatus == Socket_Rx_connected_ackcheck)
        {
            sock->rxStatus = Socket_Rx_none;
            if (rCode == Parser_Ok)
            {

                if (parser_scanf("+QISEND: %d,%d,%d", &sock->ackData.totalSend, &sock->ackData.acked, &sock->ackData.notAcked))
                {
                    if (sock->ackData.notAcked == 0)
                    {
                        sock->ackData.checkNeeded = false;
                        _Fire_(sock, onSendAcked, sock);
                    }
                }
            }
            //     sock->rxStatus = Socket_Rx_none;
        }
        break;
    case Socket_closing:
        /* code */
        if (rCode == Parser_Ok || rCode == Parser_Error)
        {
            sock->state = Socket_closed;
            // sock->rxStatus = Socket_Rx_none;
            // sock->millis = millis();
            // IStream_clear(&sock->stream.Input);
            // OStream_clear(&sock->stream.Output);
            _Fire_(sock, onClosed, sock);
        }
        break;
    case Socket_closed:
        /* code */
        break;
    default:
        break;
    }

    parser_setOnCommandFinish(NULL, NULL);
}

static bool ack_send_check(AT_Socket *sock)
{
    if (parser_isBusy())
    {
        return false;
    }
    if (sock->state != Socket_connected)
        return false;

    if (sock->ackData.checkNeeded &&
        millis() - sock->ackData.millis > checkSendIntervals[sock->ackData.timerIndex])
    {
        // printf("ack check in %d\n",checkSendIntervals[sock->ackData.timerIndex]);
        sock->ackData.millis = millis();
        sock->ackData.timerIndex++;
        if(sock->ackData.timerIndex >=checkSendIntervalsSize){
            sock->ackData.timerIndex = checkSendIntervalsSize -1 ;
        }
        // sock->ackData.timerIndex %= checkSendIntervalsSize;
        parser_setOnCommandFinish(onResponse, sock);
        if (parser_sendCommand_f(1000, "AT+QISEND=%d,%d\r\n", sock->sock_id, 0))
        {
            sock->rxStatus = Socket_Rx_connected_ackcheck;
            return true;
        }
        else
        {
            parser_setOnCommandFinish(NULL, NULL);
            return false;
        }
    }
}
static void init_sock(AT_Socket *sock)
{
    int id = sock->sock_id;
    memset(sock, 0, sizeof(AT_Socket));
    sock->sock_id = id;
}
static void handle_sock(AT_Socket *sock)
{
    if (!parser_isBusy())
    {
        ack_send_check(sock);
        // if (sock->rxStatus != Socket_Rx_none && millis() - sock->millis > 20000)
        // {
        // }
        // else if (sock->state == Socket_connected && sock->rxStatus == Socket_Rx_none)
        // {
        // }
    }
}
void ec200_sock_init(AT_Socket *sock, Socket_callbacks callbacks)
{

    init_sock(sock);

    sock->callbacks = callbacks;
    // Socket_Stream *stream = &sock->stream;
    // stream->parent = sock;

    // IStream_init(&stream->Input, NULL, rxBuff, rxBuffSize);
    // IStream_setCheckReceive(&stream->Input, NULL);
    // IStream_setArgs(&stream->Input, stream);

    // OStream_init(&stream->Output, sock_transmit, txBuff, txBuffSize);
    // OStream_setCheckTransmit(&stream->Output, NULL);
    // OStream_setArgs(&stream->Output, stream);
}

// check isBusy before
bool ec200_sock_connect(AT_Socket *sock, const char *domain, uint16_t port)
{
    if (parser_isBusy())
    {
        return false;
    }
    if (sock->state != Socket_idle && sock->state != Socket_closed)
    {
        return false;
    }
    sock->port = port;
    strncpy(sock->domain, domain, sizeof(sock->domain) - 1);

    parser_setOnCommandFinish(onResponse, sock);
    if (parser_sendCommand_f(5000, "AT+QICLOSE=%d,%d\r\n", sock->sock_id, 2))
    {
        sock->state = Socket_connecting;
        sock->rxStatus = Socket_Rx_connecting_1;
        // sock->millis = millis();
    }
    else
    {
        parser_setOnCommandFinish(NULL, NULL);
        return false;
    }
}
// check isBusy before
bool ec200_sock_close(AT_Socket *sock)
{
    if (parser_isBusy())
    {
        return false;
    }
    if (sock->state != Socket_connected)
    {
        return false;
    }
    parser_setOnCommandFinish(onResponse, sock);
    if (parser_sendCommand_f(2200, "AT+QICLOSE=%d,%d\r\n", sock->sock_id, 2))
    {
        sock->state = Socket_closing;
        // sock->rxStatus = Socket_Rx_closing;
        // sock->millis = millis();
    }
    else
    {
        parser_setOnCommandFinish(NULL, NULL);
        return false;
    }
}

// check isBusy before
bool ec200_sock_send(AT_Socket *sock, Str str)
{
    if (parser_isBusy())
    {
        return false;
    }
    if (sock->state != Socket_connected)
    {
        return false;
    }
    if (str.Len == 0)
    {
        return false;
    }
    parser_setOnCommandFinish(onResponse, sock);
    if (parser_sendCommand_dataMode_f(str.Text, 2200, "AT+QISEND=%d,%d\r\n", sock->sock_id, str.Len))
    {
        sock->rxStatus = Socket_Rx_sending;
        // sock->millis = millis();
    }
    else
    {
        parser_setOnCommandFinish(NULL, NULL);
        return false;
    }
}
bool ec200_sock_send_f(AT_Socket *sock,const char *format,...){
     if (parser_isBusy())
    {
        return false;
    }
    if (sock->state != Socket_connected)
    {
        return false;
    }
    if (strlen(format) == 0)
    {
        return false;
    }
    uint8_t b[128];

    va_list args;
    va_start(args, format);
    int written_chars = vsnprintf(b,sizeof(b),format,args);
    va_end(args);
    parser_setOnCommandFinish(onResponse, sock);
    if (parser_sendCommand_dataMode_f(b, 2200, "AT+QISEND=%d,%d\r\n", sock->sock_id, strlen(b)))
    {
        sock->rxStatus = Socket_Rx_sending;
        // sock->millis = millis();
    }
    else
    {
        parser_setOnCommandFinish(NULL, NULL);
        return false;
    }

}
void _ec200_sock_handle()
{
    for (int i = 0; i < MAX_SOCKET; i++)
    {
        handle_sock(&sockets[i]);
    }
}
void _ec200_sock_init()
{
    // for(int i=0;i<MAX_SOCKET; i++){
    //     init_sock(&sockets[i]);
    // }
}
AT_Socket *ec200_sock_get(int sock_id)
{
    if (sock_id < 0 && sock_id >= MAX_SOCKET)
        return NULL;
    return &sockets[sock_id];
}
void Q_Callback_QIOPEN(const URC *urc, Str *input)
{
    int sock_id, err;
    if (sscanf(input->Text, "+QIOPEN: %d,%d", &sock_id, &err) == 2)
    {
        if (sock_id < 0 && sock_id >= MAX_SOCKET)
            return;
        AT_Socket *sock = &sockets[sock_id];

        if (sock->state == Socket_connecting)
        {
            if (err == 0)
            {
                sock->state = Socket_connected;
                // sock->rxStatus = Socket_Rx_none;
                _Fire_(sock, onConnect, sock);
            }
            else
            {
                sock->state = Socket_idle;
                _Fire_(sock, onConnectError, sock, err, "");
            }
        }
    }
}
void Q_Callback_QIURC_CLOSED(const URC *urc, Str *input)
{
    int sock_id;
    if (sscanf(input->Text, "+QIURC: \"closed\",%d", &sock_id) == 1)
    {

        if (sock_id < 0 && sock_id >= MAX_SOCKET)
            return;
        AT_Socket *sock = &sockets[sock_id];

        if (sock->state == Socket_connected)
        {
            sock->state = Socket_closed;
            _Fire_(sock, onClosed, sock);
        }
    }
}
void Q_Callback_QIURC_PDPDEACT(const URC *urc, Str *input)
{
}
void Q_Callback_QIURC_RECV(const URC *urc, Str *input)
{
    int sock_id, len;
    if (sscanf(input->Text, "+QIURC: \"recv\",%d,%d", &sock_id, &len) == 2)
    {

        if (sock_id < 0 && sock_id >= MAX_SOCKET)
            return;
        AT_Socket *sock = &sockets[sock_id];
        char *c = strchr(input->Text, '\n');
        if (c == NULL)
            return;
        c++;
        c[len] = '\0';
        if (sock->state == Socket_connected)
        {
            _Fire_(sock, onData, sock, (Str){.Len = len, .Text = c});
        }
    }
}

#endif
