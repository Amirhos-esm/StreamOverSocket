#include "ec200.h"
#include "ec200_dbg.h"



void ec200_init(UARTStream* stream){
     parser_init(stream);
#if INCLUDE_NET
    _ec200_sock_init();
#endif
}
void ec200_handle(){
    parser_handle();
#if INCLUDE_NET
    _ec200_sock_handle();
#endif
}