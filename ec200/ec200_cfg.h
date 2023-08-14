#ifndef SERIALPORT_EC200_CFG_H
#define SERIALPORT_EC200_CFG_H

#include "stdint.h"
#include "stdarg.h"



#define EC200_DEBUG_ENABLE 1
#define INCLUDE_NET 1





#if INCLUDE_NET
    #define MAX_SOCKET 3
#endif





#if EC200_DEBUG_ENABLE
    void print_arr_hex(uint8_t *buffer, int len);
    void print_arr_norm(uint8_t *buffer, int len);
    void print_hex(uint8_t *buffer, int len);
    void print_norm(uint8_t *buffer, int len);
    void print_f(const char *format, ...);
    #define _printf(...) print_f(DEBUG_PREFIX  __VA_ARGS__)
    #define _print_no_prefix(...) print_f(__VA_ARGS__)
    #define _print_arr_hex(...) print_arr_hex(__VA_ARGS__)
    #define _print_arr_norm(...) print_arr_norm(__VA_ARGS__)
    #define _print_hex(...) print_hex(__VA_ARGS__) 
    #define _print_norm(...) __dprint_normbg(__VA_ARGS__) 
#else
    #define _print(...) 
    #define _print_no_prefix(...)
    #define _print_arr_hex(...) 
    #define _print_arr_norm(...) 
    #define _print_hex(...)
    #define _print_norm(...) 
#endif

#endif //SERIALPORT_EC200_CFG_H
