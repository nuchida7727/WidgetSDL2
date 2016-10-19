#pragma once
#include "stdlib.h"
#define _NORMAL_BLOCK true
#define _CRT_WARN true
#define _CRTDBG_MODE_FILE true
#define _CRTDBG_FILE_STDOUT true
#define _CRTDBG_ALLOC_MEM_DF true
#define _CRTDBG_DELAY_FREE_MEM_DF true
#define _CRTDBG_CHECK_ALWAYS_DF true
#define _CRTDBG_LEAK_CHECK_DF true
void* _malloc_dbg(int X, int normal_block, void* file, int line);
void* _CrtSetReportMode(int a, int b);
void* _CrtSetReportFile(int a, int b);
void* _CrtSetDbgFlag(int a);
void* _malloc_dbg(int X, int normal_block, void* file, int line) {
    return(malloc(X));
}
void* _CrtSetReportMode(int a, int b) {
    return((void*)NULL);
}
void* _CrtSetReportFile(int a, int b) {
    return((void*)NULL);
}
void* _CrtSetDbgFlag(int a) {
    return((void*)NULL);
}
