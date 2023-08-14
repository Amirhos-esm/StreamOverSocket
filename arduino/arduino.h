#ifndef SERIALPORT_GENERAL_H
#define SERIALPORT_GENERAL_H
#include "stdint.h"
#include "stdio.h"
#include "stdarg.h"

#define STR(X) X, strlen(X)
#define STR_C(X) X, sizeof(X)

void delay(uint32_t time);
uint32_t millis();
void print_arr_hex(uint8_t *buffer, int len);
void print_arr_norm(uint8_t *buffer, int len);
void print_hex(uint8_t *buffer, int len);
void print_norm(uint8_t *buffer, int len);
void print_f(const char *format, ...);
// Generic macro for print
#define print(value) _Generic((value), \
    const char*: print_str, \
    char*: print_str, \
    uint8_t*: print_str, \
    const uint8_t*: print_str, \
    char: print_char, \
    unsigned char: print_uchar, \
    int: print_int, \
    unsigned int: print_uint, \
    long: print_long, \
    unsigned long: print_ulong, \
    long long: print_llong, \
    unsigned long long: print_ullong, \
    float: print_float, \
    double: print_double \
)(value)

// Generic macro for print
#define println(value) _Generic((value), \
    const char*: print_str_ln, \
    char*: print_str_ln, \
    uint8_t*: print_str_ln, \
    const uint8_t*: print_str_ln, \
    char: print_char_ln, \
    unsigned char: print_uchar_ln, \
    int: print_int_ln, \
    unsigned int: print_uint_ln, \
    long: print_long_ln, \
    unsigned long: print_ulong_ln, \
    long long: print_llong_ln, \
    unsigned long long: print_ullong_ln, \
    float: print_float_ln, \
    double: print_double_ln \
)(value)

size_t print_str(const char* str);
size_t print_char(char ch);
size_t print_uchar(unsigned char num);
size_t print_int(int num);
size_t print_uint(unsigned int num);
size_t print_long(long num);
size_t print_ulong(unsigned long num);
size_t print_llong(long long num);
size_t print_ullong(unsigned long long num);
size_t print_float(float num);
size_t print_double(double num);

size_t print_str_ln(const char* str);
size_t print_char_ln(char ch);
size_t print_uchar_ln(unsigned char num);
size_t print_int_ln(int num);
size_t print_uint_ln(unsigned int num);
size_t print_long_ln(long num);
size_t print_ulong_ln(unsigned long num);
size_t print_llong_ln(long long num);
size_t print_ullong_ln(unsigned long long num);
size_t print_float_ln(float num);
size_t print_double_ln(double num);


#endif //SERIALPORT_GENERAL_H
