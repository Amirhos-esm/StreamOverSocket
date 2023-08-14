#ifndef SERIALPORT_EC200_H
#define SERIALPORT_EC200_H
#ifdef __cplusplus
extern "C"{
#endif
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "UARTStream.h"
#include "Str.h"
#include "arduino.h"
#include "../at_parser/parser.h"
#include "ec200_cfg.h"


#if INCLUDE_NET
    #include "ec200_sock.h"
#endif


void ec200_init(UARTStream* stream);
void ec200_handle();

#ifdef __cplusplus
}
#endif
#endif //SERIALPORT_EC200_H
