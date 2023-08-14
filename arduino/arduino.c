#include "arduino.h"
#include <windows.h>

void delay(uint32_t time)
{
    Sleep(time);
}

uint32_t millis()
{
    return GetTickCount();
}

void replaceSpecialChar(char ch, uint8_t *buffer)
{
    switch (ch)
    {
    case '\r':
        snprintf(buffer, 6, "\\r", ch);
        break;
    case '\n':
        snprintf(buffer, 6, "\\n", ch);
        break;
    case '\t':
        snprintf(buffer, 6, "\\t", ch);
        break;
    case '\0':
        snprintf(buffer, 6, "\\0", ch);
        break;
    default:
        snprintf(buffer, 6, "%c", ch); // Return the original character if it's not a special character
    }
}
void print_arr_hex(uint8_t *buffer, int len)
{
    printf("array(%d)[ ", len);
    for (int i = 0; i < len; i++)
    {
        printf("%02X", buffer[i]);
        if (i != len - 1)
        {
            printf(" ");
        }
    }
    printf(" ]");
}
void print_arr_norm(uint8_t *buffer, int len)
{
    uint8_t temp[6];
    printf("array(%d)[ ", len);
    for (int i = 0; i < len; i++)
    {
        replaceSpecialChar(buffer[i], temp);
        printf("%s", temp);
        if (i != len - 1)
        {
        }
    }
    printf(" ]");
}
void print_hex(uint8_t *buffer, int len)
{
    for (int i = 0; i < len; i++)
    {
        printf("%02X", buffer[i]);
        if (i != len - 1)
        {
            printf(" ");
        }
    }
}
void print_norm(uint8_t *buffer, int len)
{
    uint8_t temp[6];
    for (int i = 0; i < len; i++)
    {
        replaceSpecialChar(buffer[i], temp);
        printf("%s", temp);
        if (i != len - 1)
        {
        }
    }
}

void print_f(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

size_t print_str(const char *str)
{
    return printf("%s", str);
}

size_t print_char(char ch)
{
    return printf("%c", ch);
}

size_t print_uchar(unsigned char num)
{
    return printf("%hhu", num);
}

size_t print_int(int num)
{
    return printf("%d", num);
}

size_t print_uint(unsigned int num)
{
    return printf("%u", num);
}

size_t print_long(long num)
{
    return printf("%ld", num);
}

size_t print_ulong(unsigned long num)
{
    return printf("%lu", num);
}

size_t print_llong(long long num)
{
    return printf("%lld", num);
}

size_t print_ullong(unsigned long long num)
{
    return printf("%llu", num);
}

size_t print_float(float num)
{
    return printf("%f", num);
}

size_t print_double(double num)
{
    return printf("%lf", num);
}

size_t print_str_ln(const char *str)
{
    return printf("%s\n", str);
}

size_t print_char_ln(char ch)
{
    return printf("%c\n", ch);
}

size_t print_uchar_ln(unsigned char num)
{
    return printf("%hhu\n", num);
}

size_t print_int_ln(int num)
{
    return printf("%dv", num);
}

size_t print_uint_ln(unsigned int num)
{
    return printf("%u\n", num);
}

size_t print_long_ln(long num)
{
    return printf("%ld\n", num);
}

size_t print_ulong_ln(unsigned long num)
{
    return printf("%lu\n", num);
}

size_t print_llong_ln(long long num)
{
    return printf("%lld\n", num);
}

size_t print_ullong_ln(unsigned long long num)
{
    return printf("%llu\n", num);
}

size_t print_float_ln(float num)
{
    return printf("%f\n", num);
}

size_t print_double_ln(double num)
{
    return printf("%lf\n", num);
}
