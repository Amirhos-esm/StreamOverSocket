#include "parser.h"
void Q_Callback_CLIP(const URC* urc, Str* input){

}
void Q_Callback_CMT(const URC* urc, Str* input){
   
}
void Q_Callback_COPS(const URC* urc, Str* input){
 printf("urc find! ");
}

void Q_Callback_CPIN_NOT_INSERTED(const URC* urc, Str* input){

}
void Q_Callback_CPIN_NOT_READY(const URC* urc, Str* input){
    
}
void Q_Callback_CPIN_READY(const URC* urc, Str* input){

}



static Mem_CmpResult URC_compareStr(const void* str, const void* urc, Mem_LenType len);
static Mem_CmpResult StrConst_compare(const void* str1, const void* str2, Mem_LenType len);

Mem_LenType URC_find(const URC* urcs, uint16_t len, Str* str) {
    Mem_LenType urcIndex = Mem_binarySearch(urcs, len, sizeof(urcs[0]), str, URC_compareStr);
    if (urcIndex >= 0) {
        if (urcs[urcIndex].cb) {
            urcs[urcIndex].cb(&urcs[urcIndex], str);
        }
    } 
    return urcIndex;
}

static Mem_CmpResult URC_compareStr(const void* str, const void* urc, Mem_LenType len) {
    const URC* u = (const URC*) urc;
    Str* s = (Str*) str;
    return Mem_compare(
        s->Text,
        u->Response.Text,
        u->Response.Len
    );
}

