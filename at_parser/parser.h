#ifndef __PARSER__H
#define __PARSER__H
#ifdef __cplusplus
extern "C"{
#endif

#include "UARTStream.h"
#include "arduino.h"
#include "stdbool.h"
#include "stdint.h"
#include "parser_const.h"
#include "Str.h"
#include "parser_urc.h"

typedef enum {
  Parser_TxState_None,
  Parser_TxState_Sendو
} Parser_TxState;

typedef enum {
  Parser_RxState_None, 
  Parser_RxState_inRec, 
} Parser_RxState;

typedef enum{
    Parser_Ok = 0,
    Parser_Error,
    Parser_Idle = 253,
    Parser_busy = 254,
    Parser_timeout = 255 ,
}Parser_CommandStatus;

typedef void (*cb_onCommandFinish)(void * args);

void parser_init(UARTStream* stream);
void parser_handle();
void parser_setOnCommandFinish(cb_onCommandFinish cb,void* args);
bool parser_sendCommand(const char* cmd,uint32_t timeout);
bool parser_sendCommand_dataMode(const char* cmd,const char* data,uint32_t timeout);
bool parser_isBusy();
bool parser_isOnCommand();
int parser_getRetCode();
bool parser_scanf(const char* format, ...);
uint8_t* parser_getComuBuffer();

#ifdef __cplusplus
}
#endif
#endif