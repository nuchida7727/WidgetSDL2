#pragma once
#ifndef WINDOWMODE
#define CUI 1
#define WINDOWMODE CUI
#endif //  WINDOWMODE
#include "memoryleak.h"
#include "string.h"
#include "stdarg.h"
typedef struct Strings {
    int sizes;
    char *strings;
    char *(*Get)(struct Strings *self);
    int (*GetSizes)(struct Strings *self);
    void (*Set)(struct Strings *self, char *format, ...);
    void (*Free)(struct Strings *self);
    void (*Print)(struct Strings *self);
    bool (*Compare)(struct Strings *self, char *char_cmp);
    void (*Load)(struct Strings *self, char *path);
    void (*Save)(struct Strings *self, char *path);
    int (*Length)(struct Strings *self);
    void (*Backspace)(struct Strings *self);
    void (*Replace)(struct Strings *self, const char *char_before, const char *char_after);
    struct Strings *(*Tag)(struct Strings *self, const char *char_start, const char *char_last);
    void (*Password)(struct Strings *self);
    void (*SjisToUtf8)(struct Strings *self);
    void (*Utf8ToSjis)(struct Strings *self);
} Strings;
Strings *new_Strings(const char *str);
void delete_Strings(Strings **str);
char *StringsGet(Strings *self) {
    char *output = self->strings;
    return(output);
}
int StringsGetSizes(Strings *self) {
    int output = self->sizes;
    return(output);
}
void StringsSet(Strings *self, char *format, ...) {
    if(format) {
        va_list arg1;
        va_list arg2;
        va_start(arg1, format);
        va_copy(arg2, arg1);
        int length = vsnprintf(NULL, 0, format, arg1) + 1;
        char *buffer = (char *)malloc(sizeof(char) * length);
        if(!buffer) {
            printf((char*)"malloc error(StringsSet)\n");
        }
        vsnprintf(buffer, length, format, arg2);
        va_end(arg1);
        va_end(arg2);
        if(self->strings) {
            free(self->strings);
        }
        self->sizes = length - 1;
        self->strings = buffer;
    } else {
        if(self->strings) {
            free(self->strings);
        }
        self->sizes = 0;
        self->strings = (char*)malloc(sizeof(char));
        if(!(self->strings)) {
            printf((char*)"malloc error(StringsSet)\n");
        }
        self->strings[0] = 0;
    }
    return;
}
void StringsFree(Strings *self) {
    if(self) {
        free(self->strings);
    }
    return;
}
void StringsPrint(Strings *self) {
    if(!self->Get(self)) {
        return;
    }
    Strings *temp = new_Strings(self->Get(self));
    char *tok = strtok(temp->Get(temp), (char*)"\n");
    while(tok) {
        printf((char*)"%s\n", tok);
        tok = strtok(NULL, (char*)"\n");
    }
    printf((char*)"length = %d\n", self->GetSizes(self));
    delete_Strings(&temp);
    return;
}
bool StringsCompare(Strings *self, char *char_cmp) {
    if(!char_cmp) {
        if(!(self->Get(self)) || !strcmp(self->Get(self), "")) {
            return(true);
        } else {
            return(false);
        }
    }
    if(!strcmp(self->Get(self), char_cmp)) {
        return(true);
    } else {
        return(false);
    }
}
void StringsLoad(Strings *self, char *path) {
    FILE *file = fopen(path, (char*)"rb");
    if(!file) {
        printf((char*)"Cannot open file(StringsLoad): %s\n", path);
        return;
    }

    fseek(file, 0, SEEK_END);
    int filesize = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *char_temp = (char*)malloc(sizeof(char) * (filesize + 1));
    memset(char_temp, 0, sizeof(char) * filesize);
    fread(char_temp, sizeof(char), filesize, file);
    char_temp[filesize] = 0;
    if(self->Get(self)) {
        free(self->Get(self));
    }
    self->strings = char_temp;
    fclose(file);
    self->sizes = filesize;
    return;
}
void StringsSave(Strings *self, char *path) {
    FILE *file = fopen(path, (char*)"wb");
    if(!file) {
        printf((char*)"Cannot open file(StringsSave): %s\n", path);
        return;
    }
    fprintf(file, (char*)"%s", self->Get(self));
    fclose(file);
    return;
}
int StringsLength(Strings *self) {
    int output = 0;
    for(int number = 0; number < self->GetSizes(self); ++number) {
        if((self->Get(self)[number] & 0xc0) != 0x80) {
            output += 1;
        };
    }
    return(output);
}
void StringsBackspace(Strings *self) {
    for(int number = self->GetSizes(self) - 1; number >= 0; --number) {
        if(self->Get(self)[number] == 0) {
            continue;
        }
        if((self->Get(self)[number] & 0xc0) != 0x80) {
            self->Get(self)[number] = 0;
            break;
        } else {
            self->Get(self)[number] = 0;
        }
    }
    return;
}
void StringsReplace(Strings *self, const char *char_before, const char *char_after) {
    // http://creativeandcritical.net/str-replace-c
    if(!(self->Get(self)) || !char_before) {
        return;
    }
    char *char_after_temp = NULL;
    if(char_after) {
        char_after_temp = (char*)char_after;
    } else {
        char_after_temp = (char*)"";
    }
    size_t cache_sz_inc = 16;
    size_t cache_sz_inc_factor = 3;
    size_t cache_sz_inc_max = 1048576;
    char *pstr = self->Get(self);
    char *pstr2 = NULL;
    int counts = 0;
    ptrdiff_t *pos_cache = (ptrdiff_t*)malloc(0);
    int cache_sz = 0;
    int retlen = 0;
    int tolen = 0;
    int fromlen = strlen(char_before);
    while ((pstr2 = strstr(pstr, char_before)) != NULL) {
        counts += 1;
        if (cache_sz < counts) {
            cache_sz += cache_sz_inc;
            ptrdiff_t *pos_cache_temp = (ptrdiff_t*)realloc(pos_cache, sizeof(*pos_cache) * cache_sz);
            if (pos_cache_temp == NULL) {
                free(pos_cache);
                return;
            }
            pos_cache = pos_cache_temp;
            cache_sz_inc *= cache_sz_inc_factor;
            if (cache_sz_inc > cache_sz_inc_max) {
                cache_sz_inc = cache_sz_inc_max;
            }
        }
        pos_cache[counts - 1] = pstr2 - self->Get(self);
        pstr = pstr2 + fromlen;
    }
    int orglen = pstr - self->Get(self) + strlen(pstr);
    if (counts > 0) {
        tolen = strlen(char_after_temp);
        retlen = orglen + (tolen - fromlen) * counts;
    } else	retlen = orglen;
    char *ret = (char*)malloc(retlen + 1);
    if (ret == NULL) {
        free(pos_cache);
        return;
    }
    if (counts == 0) {
        strcpy(ret, self->Get(self));
    } else {
        char *pret = ret;
        memcpy(pret, self->Get(self), pos_cache[0]);
        pret += pos_cache[0];
        for (int number = 0; number < counts; number++) {
            memcpy(pret, char_after_temp, tolen);
            pret += tolen;
            pstr = self->Get(self) + pos_cache[number] + fromlen;
            int cpylen = (number == counts - 1 ? orglen : pos_cache[number + 1]) - pos_cache[number] - fromlen;
            memcpy(pret, pstr, cpylen);
            pret += cpylen;
        }
        ret[retlen] = '\0';
    }
    if(self->strings) {
        free(self->strings);
    }
    self->strings = ret;
    self->sizes = strlen(self->strings);
    free(pos_cache);
    return;
}
Strings *StringsTag(Strings *self, const char *char_start, const char *char_last) {
    if(!(self->Get(self)) || !char_start || !char_last) {
        return(NULL);
    }
    char *position_start = strstr(self->Get(self), char_start);
    position_start += strlen(char_start);
    self->Set(self, (char*)"%s", position_start);
    position_start = self->Get(self);
    Strings *output = new_Strings(self->Get(self));
    char *position_last = strstr(self->Get(self), char_last);
    output->Get(output)[(int)(position_last - position_start)] = 0;
    output->Set(output, output->Get(output));
    return(output);
}
void StringsPassword(Strings *self) {
    for(int number = 0; number < self->GetSizes(self); ++number) {
        self->Get(self)[number] ^= 7;
    }
    return;
}
#ifdef _WIN32
#include "windows.h"
void StringsSjisToUtf8(Strings *self) {
    if(!(self->Compare(self, NULL))) {
        int sizes = self->GetSizes(self) * 3;
        char *char_temp = NULL;
        wchar_t buffer_unicode[1 << 16] = {0};
        int length_unicode = MultiByteToWideChar(CP_ACP, 0, self->Get(self), self->GetSizes(self) + 1, NULL, 0);
        if(length_unicode <= (int)sizeof(buffer_unicode) / (int)sizeof(buffer_unicode[0])) {
            MultiByteToWideChar(CP_ACP, 0, self->Get(self), self->GetSizes(self) + 1, buffer_unicode, 1 << 16);
            int length_utf8 = WideCharToMultiByte(CP_UTF8, 0, buffer_unicode, length_unicode, NULL, 0, NULL, NULL);
            char_temp = (char*)malloc(sizeof(char) * length_utf8);
            WideCharToMultiByte(CP_UTF8, 0, buffer_unicode, length_unicode, char_temp, sizes, NULL, NULL);
        }
        self->Set(self, char_temp);
        free(char_temp);
    }
    return;
}
void StringsUtf8ToSjis(Strings *self) {
    if(!(self->Compare(self, NULL))) {
        int sizes = self->GetSizes(self) * 3;
        char *char_temp = NULL;
        wchar_t buffer_unicode[1 << 16] = {0};
        int length_unicode = MultiByteToWideChar(CP_UTF8, 0, self->Get(self), self->GetSizes(self) + 1, NULL, 0);
        if(length_unicode <= (int)sizeof(buffer_unicode) / (int)sizeof(buffer_unicode[0])) {
            MultiByteToWideChar(CP_UTF8, 0, self->Get(self), self->GetSizes(self) + 1, buffer_unicode, 1 << 16);
            int length_utf8 = WideCharToMultiByte(CP_ACP, 0, buffer_unicode, length_unicode, NULL, 0, NULL, NULL);
            char_temp = (char*)malloc(sizeof(char) * length_utf8);
            WideCharToMultiByte(CP_ACP, 0, buffer_unicode, length_unicode, char_temp, sizes, NULL, NULL);
        }
        self->Set(self, char_temp);
        free(char_temp);
    }
    return;
}
#else
void StringsSjisToUtf8(Strings *self) {
    return;
}
void StringsUtf8ToSjis(Strings *self) {
    return;
}
#endif  // _WIN32
Strings *new_Strings(const char *str) {
    Strings *output = (Strings*)malloc(sizeof(Strings));
    *output = (Strings) {
        0
    };
    output->Get = StringsGet;
    output->GetSizes = StringsGetSizes;
    output->Set = StringsSet;
    output->Free = StringsFree;
    output->Print = StringsPrint;
    output->Compare = StringsCompare;
    output->Load = StringsLoad;
    output->Save = StringsSave;
    output->Length = StringsLength;
    output->Backspace = StringsBackspace;
    output->Replace = StringsReplace;
    output->Tag = StringsTag;
    output->Password = StringsPassword;
    output->SjisToUtf8 = StringsSjisToUtf8;
    output->Utf8ToSjis = StringsUtf8ToSjis;
    output->Set(output, (char*)str);
    return(output);
}
void delete_Strings(Strings **str) {
    if(*str) {
        (*str)->Free(*str);
        free(*str);
    }
    *str = NULL;
    return;
}
