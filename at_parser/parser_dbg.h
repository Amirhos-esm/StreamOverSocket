#ifndef __PARSER_DBG__H
#define __PARSER_DBG__H
#ifdef __cplusplus
extern "C"{
#endif
#include "stdint.h"
#include "stdarg.h"

#define DEBUG_PREFIX "parser | "




#if PARSER_DEBUG_ENABLE
    void print_arr_hex(uint8_t *buffer, int len);
    void print_arr_norm(uint8_t *buffer, int len);
    void print_hex(uint8_t *buffer, int len);
    void print_norm(uint8_t *buffer, int len);
    void print_f(const char *format, ...);
    #define _print(...) print_f(DEBUG_PREFIX  __VA_ARGS__)
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





#ifdef __cplusplus
}
#endif
#endif