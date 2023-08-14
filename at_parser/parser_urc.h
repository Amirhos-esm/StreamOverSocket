#ifndef __PARSER_URC_H
#define __PARSER_URC__H
#ifdef __cplusplus
extern "C"{
#endif
#include "Str.h"

#define __URC_INIT(X,Y) { .Response = {.Text = Y, .Len = sizeof(Y) - 1}, .cb = Q_Callback_##X }


struct __URC;
typedef struct __URC URC;

typedef void (*URC_Callback)(const URC* urc, Str* input);

struct __URC {
    StrConst     Response;
    URC_Callback cb;
};

void Q_Callback_CLIP(const URC* urc, Str* input);
void Q_Callback_CMT(const URC* urc, Str* input);
void Q_Callback_COPS(const URC* urc, Str* input);
void Q_Callback_CPIN_NOT_INSERTED(const URC* urc, Str* input);
void Q_Callback_CPIN_NOT_READY(const URC* urc, Str* input);
void Q_Callback_CPIN_READY(const URC* urc, Str* input);
//void Q_Callback_CUSD(const URC* urc, Str* input);
//void Q_Callback_HTTP_POST_FILE(const URC* urc, Str* input);
//void Q_Callback_QIND_ACT_CDMA(const URC* urc, Str* input);
//void Q_Callback_QIND_ACT_EGPRS(const URC* urc, Str* input);
//void Q_Callback_QIND_ACT_EVDO(const URC* urc, Str* input);
//void Q_Callback_QIND_ACT_GSM(const URC* urc, Str* input);
//void Q_Callback_QIND_ACT_HDR(const URC* urc, Str* input);
//void Q_Callback_QIND_ACT_HSDPA_HSUPA(const URC* urc, Str* input);
//void Q_Callback_QIND_ACT_HSDPA(const URC* urc, Str* input);
//void Q_Callback_QIND_ACT_HSUPA(const URC* urc, Str* input);
//void Q_Callback_QIND_ACT_LTE(const URC* urc, Str* input);
//void Q_Callback_QIND_ACT_TD_SCDMA(const URC* urc, Str* input);
//void Q_Callback_QIND_ACT_UNKNOWN(const URC* urc, Str* input);
//void Q_Callback_QIND_ACT_WCDMA(const URC* urc, Str* input);
//void Q_Callback_QIND_CSQ_99_99(const URC* urc, Str* input);
//void Q_Callback_QIND_CSQ(const URC* urc, Str* input);
//void Q_Callback_QIND_CSQ(const URC* urc, Str* input);
//void Q_Callback_QIND_PB_DONE(const URC* urc, Str* input);
//void Q_Callback_QIND_SMS_DONE(const URC* urc, Str* input);

void Q_Callback_QIOPEN(const URC* urc, Str* input);
void Q_Callback_QIURC_CLOSED(const URC* urc, Str* input);
void Q_Callback_QIURC_PDPDEACT(const URC* urc, Str* input);
void Q_Callback_QIURC_RECV(const URC* urc, Str* input);



static const URC Q_URC[] = {
    __URC_INIT(CLIP,"+CLIP:"),
    __URC_INIT(CMT,"+CMT:"),
    __URC_INIT(COPS,"+COPS:"),
    __URC_INIT(CPIN_NOT_INSERTED,"+CPIN: NOT INSERTED"),
    __URC_INIT(CPIN_NOT_READY,"+CPIN: NOT READY"),
    __URC_INIT(CPIN_READY,"+CPIN: READY"),
    // __URC_INIT(CUSD),
    // __URC_INIT(HTTP_POST_FILE),
    // __URC_INIT(QIND_ACT_CDMA),
    // __URC_INIT(QIND_ACT_EGPRS),
    // __URC_INIT(QIND_ACT_EVDO),
    // __URC_INIT(QIND_ACT_GSM),
    // __URC_INIT(QIND_ACT_HDR),
    // __URC_INIT(QIND_ACT_HSDPA),
    // __URC_INIT(QIND_ACT_HSDPA_HSUPA),
    // __URC_INIT(QIND_ACT_HSUPA),
    // __URC_INIT(QIND_ACT_LTE),
    // __URC_INIT(QIND_ACT_TD_SCDMA),
    // __URC_INIT(QIND_ACT_UNKNOWN),
    // __URC_INIT(QIND_ACT_WCDMA),
    // __URC_INIT(QIND_CSQ),
    // __URC_INIT(QIND_CSQ_99_99),
    // __URC_INIT(QIND_PB_DONE),
    // __URC_INIT(QIND_SMS_DONE),

    __URC_INIT(QIOPEN,"+QIOPEN:"),
    __URC_INIT(QIURC_CLOSED,"+QIURC: \"closed\""),
    __URC_INIT(QIURC_PDPDEACT,"+QIURC: \"pdpdeact\""),
    __URC_INIT(QIURC_RECV,"+QIURC: \"recv\""),
};
static const uint16_t Q_URC_LEN = sizeof(Q_URC) / sizeof(Q_URC[0]);




#ifdef __cplusplus
}
#endif
#endif