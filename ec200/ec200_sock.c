#include "ec200_sock.h"
#if INCLUDE_NET

 AT_Socket sockets[MAX_SOCKET] = {
#if MAX_SOCKET >= 1 
    {.sock_id = 0}
#endif
#if MAX_SOCKET >= 2 
    ,{.sock_id = 0}
#endif
#if MAX_SOCKET >= 3 
    ,{.sock_id = 0}
#endif      
};
static void init_sock(AT_Socket* sock){
    int id = sock->sock_id;
    memset(sock,0,sizeof(AT_Socket));
    sock->sock_id = id;
}
static void handle_sock(AT_Socket* sock){

}
void ec200_sock_initStream(AT_Socket* sock,
	uint8_t* rxBuff, Stream_LenType rxBuffSize,
	uint8_t* txBuff, Stream_LenType txBuffSize){

    Socket_Stream* stream = sock->stream;
    stream->args = sock;
    
    IStream_init(&stream->Input, UARTStream_receive, rxBuff, rxBuffSize);
    IStream_setCheckReceive(&stream->Input, UARTStream_checkReceive);
    IStream_setArgs(&stream->Input, stream);

    OStream_init(&stream->Output, UARTStream_transmit, txBuff, txBuffSize);
    OStream_setCheckTransmit(&stream->Output, UARTStream_checkTransmit);
    OStream_setArgs(&stream->Output, stream);

}
void ec200_sock_connect(AT_Socket* sock){
    
}
void ec200_sock_handle(){
    for(int i=0;i<MAX_SOCKET; i++){
        handle_sock(&sockets[i]);
    }
}
void ec200_sock_init(){
    // for(int i=0;i<MAX_SOCKET; i++){
    //     init_sock(&sockets[i]);
    // }
}
#endif
