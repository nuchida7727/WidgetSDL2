#pragma once
#ifndef WINDOWMODE
#define GUI 2
#define WINDOWMODE GUI
#endif //  WINDOWMODE
#include "memoryleak.h"
#include "guibasic.h"
Guibasic *Guiglobal = NULL;
void new_Guiglobal(int width, int height);
void delete_Guiglobal(void);
void GuiglobalPrintf(char *format, ...) {
    va_list arg1;
    va_list arg2;
    va_start(arg1, format);
    va_copy(arg2, arg1);
    int length = vsnprintf(NULL, 0, format, arg1) + 1;
    char *buffer = (char *)malloc(sizeof(char) * length);
    if(!buffer) {
        printf((char*)"malloc error(GuibasicPrintf)\n");
    }
    vsnprintf(buffer, length, format, arg2);
    va_end(arg1);
    va_end(arg2);
    Guiglobal->Printf(Guiglobal, buffer);
    free(buffer);
    return;
}
int GuiglobalCharGetting(void) {
    int output = Guiglobal->CharGetting(Guiglobal);
    return(output);
}
void GuiglobalCls(void) {
    Guiglobal->Cls(Guiglobal);
}
void new_Guiglobal(int width, int height) {
    Guiglobal = new_Guibasic(width, height);
}
void delete_Guiglobal(void) {
    delete_Guibasic(&Guiglobal);
    Guiglobal = NULL;
}
