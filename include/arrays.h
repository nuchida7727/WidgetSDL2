#pragma once
#include "strings.h"
typedef struct ArrayS {
    int rows;
    int columns;
    Strings **data;
    Strings *header;
    Strings **(*Get)(struct ArrayS *self);
    int (*GetRows)(struct ArrayS *self);
    int (*GetColumns)(struct ArrayS *self);
    char *(*GetHeader)(struct ArrayS *self);
    void (*Free)(struct ArrayS *self);
    void (*Print)(struct ArrayS *self);
    struct ArrayS *(*Duplicate)(struct ArrayS *self);
    void (*Copy)(struct ArrayS *self, struct ArrayS *arr);
    void (*Append)(struct ArrayS *self, char *item);
    void (*Sort)(struct ArrayS *self);
    void (*Utf8ToSjis)(struct ArrayS *self);
    void (*SjisToUtf8)(struct ArrayS *self);
    void (*StringsToCSV)(struct ArrayS *self, Strings *str);
    Strings *(*CSVToStrings)(struct ArrayS *self);
    void (*LoadCSV)(struct ArrayS *self, char *path);
    void (*SaveCSV)(struct ArrayS *self, char *path);
    Strings *(*ArraySToStrings)(struct ArrayS *self);
    void (*StringsToArrayS)(struct ArrayS *self, Strings *strings);
} ArrayS;
ArrayS *new_ArrayS(int rows, int columns, char **data, char *header);
void delete_ArrayS(ArrayS **arr);
Strings **ArraySGet(ArrayS *self) {
    Strings **output = self->data;
    return(output);
}
int ArraySGetRows(ArrayS *self) {
    int output = self->rows;
    return(output);
}
int ArraySGetColumns(ArrayS *self) {
    int output = self->columns;
    return(output);
}
char *ArraySGetHeader(ArrayS *self) {
    char *output = self->header->Get(self->header);
    return(output);
}
void ArraySFree(ArrayS *self) {
    if(self) {
        if(self->Get(self)) {
            for(int number = 0; number < self->GetRows(self) * self->GetColumns(self); ++number) {
                delete_Strings(&(self->Get(self)[number]));
            }
            free(self->Get(self));
        }
        delete_Strings(&(self->header));
        *self = (ArrayS) {
            0
        };
    }
    return;
}
void ArraySPrint(ArrayS *self) {
    if(!(self->GetRows(self) * self->GetColumns(self))) {
        return;
    }
    Strings *str = new_Strings(NULL);
    str->Set(str, (char*)"%srows = %d\n", str->Get(str), self->GetRows(self));
    str->Set(str, (char*)"%scolumns = %d\n", str->Get(str), self->GetColumns(self));
    if(self->header->Compare(self->header, NULL)) {
        str->Set(str, (char*)"%sheader = %s\n", str->Get(str), self->GetHeader(self));
    } else {
        str->Set(str, (char*)"%sheader = %s", str->Get(str), self->GetHeader(self));
    }
    str->Set(str, (char*)"%s---data---\n", str->Get(str));
    for(int row = 0; row < self->GetRows(self); ++row) {
        for(int column = 0; column < self->GetColumns(self); ++column) {
            if(self->Get(self)[row * self->GetColumns(self) + column]) {
                str->Set(str, (char*)"%s%s  ", str->Get(str), self->Get(self)[row * self->GetColumns(self) + column]->Get(self->Get(self)[row * self->GetColumns(self) + column]));
            } else {
                str->Set(str, (char*)"%s  ", str->Get(str));
            }
        }
        str->Set(str, (char*)"%s\n", str->Get(str));
    }
    str->Set(str, (char*)"%s----------\n", str->Get(str));
    printf(str->Get(str));
    delete_Strings(&str);
    return;
}
ArrayS *ArraySDuplicate(ArrayS *self) {
    ArrayS *output = new_ArrayS(self->GetRows(self), self->GetColumns(self), NULL, self->header->Get(self->header));
    for(int row = 0; row < self->GetRows(self); ++row) {
        for(int column = 0; column < self->GetColumns(self); ++column) {
            if(self->Get(self)[row * self->GetColumns(self) + column]) {
                Strings *str = self->Get(self)[row * self->GetColumns(self) + column];
                output->Get(output)[row * output->GetColumns(output) + column]->Set(output->Get(output)[row * output->GetColumns(output) + column], str->Get(str));
            }
        }
    }
    return(output);
}
void ArraySCopy(ArrayS *self, ArrayS *arr) {
    ArrayS *temp = arr->Duplicate(arr);
    self->Free(self);
    *self = *temp;
    free(temp);
    return;
}
void ArraySAppend(ArrayS *self, char *item) { // add column
    if(item) {
        ArrayS *arr = new_ArrayS(1, self->GetColumns(self) + 1, NULL, self->header->Get(self->header));
        for(int number = 0; number < self->GetColumns(self); ++number) {
            arr->Get(arr)[number]->Set(arr->Get(arr)[number], self->Get(self)[number]->Get(self->Get(self)[number]));
        }
        arr->Get(arr)[self->GetColumns(self)]->Set(arr->Get(arr)[self->GetColumns(self)], item);
        self->Copy(self, arr);
        delete_ArrayS(&arr);
    }
    return;
}
int ArraySCompare(const void *left, const void *right) {
    if(!((*(Strings**)left)->Get(*(Strings**)left)) || !((*(Strings**)right)->Get(*(Strings**)left))) {
        return(0);
    }
    int output = strcmp((*(Strings**)left)->Get((*(Strings**)left)), (*(Strings**)right)->Get((*(Strings**)right)));
    return(output);
}
void ArraySSort(ArrayS *self) {
    if(self->GetRows(self) != 1) {
        return;
    }
    for(int column = 0; column < self->GetColumns(self); ++column) {
        if(!(self->Get(self)[column]->Get(self->Get(self)[column]))) {
            return;
        }
    }
    qsort(self->Get(self), self->GetColumns(self), sizeof(self->Get(self)[0]), ArraySCompare);
    return;
}
void ArraySUtf8ToSjis(ArrayS *self) {
    for(int row = 0; row < self->GetRows(self); ++row) {
        for(int column = 0; column < self->GetColumns(self); ++column) {
            Strings *str = self->Get(self)[row * self->GetColumns(self) + column];
            str->Utf8ToSjis(str);
        }
    }
}
void ArraySSjisToUtf8(ArrayS *self) {
    for(int row = 0; row < self->GetRows(self); ++row) {
        for(int column = 0; column < self->GetColumns(self); ++column) {
            Strings *str = self->Get(self)[row * self->GetColumns(self) + column];
            str->SjisToUtf8(str);
        }
    }
}
void ArraySStringsToCSV(ArrayS *self, Strings *str) {
    str->Replace(str, (char*)"\r", NULL);
    char *newline = (char*)"\n";
    char *separate = (char*)",";
    int rows = 0;
    int columns = 0;
    for(int number = 0; str->Get(str)[number] != '\0'; ++number) {
        if(str->Get(str)[number] == '\n') {
            rows += 1;
        }
    }
    if(str->Get(str)[str->GetSizes(str) - 1] != '\n') {
        rows += 1;  // add row when last char is not newline;
    }
    str->Replace(str, (char*)",,", (char*)",\"NULL_ArraySStringsToCSV\",");
    str->Replace(str, (char*)",\n", (char*)",\"NULL_ArraySStringsToCSV\"\n");
    rows = rows - 2;  // delete comment and column_name;
    Strings *header = new_Strings(NULL);
    Strings *str_temp = new_Strings(str->Get(str));
    char *tok = NULL;
    tok = strtok(str_temp->Get(str_temp), newline);  // comment
    header->Set(header, (char*)"%s%s\n", header->Get(header), tok);
    header->Replace(header, ",", NULL);
    tok = strtok(NULL, newline);  // column name
    header->Set(header, (char*)"%s%s\n", header->Get(header), tok);
    char *tok_name = strtok(tok, separate);
    while(tok_name) {
        columns += 1;
        tok_name = strtok(NULL, separate);
        if(columns >= 1 << 16) {
            printf((char*)"File too big (ArraySStringsToCSV)");
            ArrayS *arr = new_ArrayS(0, 0, NULL, NULL);
            self->Copy(self, arr);
            delete_ArrayS(&arr);
            return;
        }
    }
    header->Replace(header, (char*)"\"NULL_ArraySStringsToCSV\"", NULL);
    header->Replace(header, (char*)"\r", NULL);
    ArrayS *arr = new_ArrayS(rows, columns, NULL, header->Get(header));
    delete_Strings(&str_temp);
    delete_Strings(&header);
    strtok(str->Get(str), newline);  // comment
    strtok(NULL, newline);  // column name
    Strings *separate_new = new_Strings(NULL);
    separate_new->Set(separate_new, (char*)"%s%s", separate, newline);
    tok = strtok(NULL, separate_new->Get(separate_new));
    for(int number = 0; number < rows * columns; ++number) {
        arr->Get(arr)[number]->Set(arr->Get(arr)[number], tok);
        arr->Get(arr)[number]->Replace(arr->Get(arr)[number], (char*)"\"NULL_ArraySStringsToCSV\"", NULL);
        arr->Get(arr)[number]->Replace(arr->Get(arr)[number], (char*)"\r", NULL);
        tok = strtok(NULL, separate_new->Get(separate_new));
    }
    delete_Strings(&separate_new);
    self->Copy(self, arr);
    delete_ArrayS(&arr);
    return;
}
Strings *ArraySCSVToStrings(ArrayS *self) {
    Strings *output = new_Strings(self->GetHeader(self));
    for(int row = 0; row < self->GetRows(self); ++row) {
        output->Set(output, (char*)"%s%s", output->Get(output), self->Get(self)[row * self->GetColumns(self) + 0]->Get(self->Get(self)[row * self->GetColumns(self) + 0]));
        for(int column = 1; column < self->GetColumns(self); ++column) {
            output->Set(output, (char*)"%s,%s", output->Get(output), self->Get(self)[row * self->GetColumns(self) + column]->Get(self->Get(self)[row * self->GetColumns(self) + column]));
        }
        output->Set(output, (char*)"%s\n", output->Get(output));
    }
    output->Replace(output, (char*)"\n", (char*)"\r\n");
    output->Replace(output, (char*)"\r", (char*)"\r\n");
    output->Replace(output, (char*)"\n\n", (char*)"\n");
    output->Replace(output, (char*)"\r\r", (char*)"\r");
    return(output);
}
void ArraySLoadCSV(ArrayS *self, char *path) {
    Strings *str = new_Strings(NULL);
    str->Load(str, path);
    str->Compare(str, NULL);
    if(str->Compare(str, NULL)) {
        printf((char*)"Cannot open file(ArraySLoadCSV): %s\n", path);
        delete_Strings(&str);
        return;
    }
    self->StringsToCSV(self, str);
    delete_Strings(&str);
    return;
}
void ArraySSaveCSV(ArrayS *arr, char *path) {
    Strings *str = arr->CSVToStrings(arr);
    str->Save(str, path);
    delete_Strings(&str);
    return;
}
Strings *ArraySArraySToStrings(ArrayS *self) {
    Strings *output = new_Strings(NULL);
    output->Set(output, (char*)"%s\r\n", self->Get(self)[0]->Get(self->Get(self)[0]));
    for(int column = 1; column < self->GetColumns(self); ++column) {
        output->Set(output, (char*)"%s%s\r\n", output->Get(output), self->Get(self)[column]->Get(self->Get(self)[column]));
    }
    return(output);
}
void ArraySStringsToArrayS(ArrayS *self, Strings *str) {
    if(str->Compare(str, NULL)) {
        return;
    }
    str->Replace(str, (char*)"\r", NULL);
    Strings *temp = new_Strings(str->Get(str));
    int columns = 0;
    for(int length = 0; length < str->GetSizes(str); ++length) {
        if(str->Get(str)[length] == '\n') {
            columns += 1;
        }
    }
    if(str->Get(str)[str->GetSizes(str) - 1] != '\n' && str->Get(str)[str->GetSizes(str) - 1] != '\r') {
        columns += 1;  // add row when last char is not newline;
    }
    ArrayS *arr = new_ArrayS(1, columns, NULL, NULL);
    char *str_line = strtok(temp->Get(temp), (char*)"\n");
    for(int column = 0; column < columns; ++column) {
        arr->Get(arr)[column]->Set(arr->Get(arr)[column], str_line);
        str_line = strtok(NULL, (char*)"\n");
    }
    self->Copy(self, arr);
    delete_ArrayS(&arr);
    delete_Strings(&temp);
    return;
}
ArrayS *new_ArrayS(int rows, int columns, char **data, char *header) {
    ArrayS *output = (ArrayS*)malloc(sizeof(ArrayS));
    *output = (ArrayS) {
        0
    };
    output->Get = ArraySGet;
    output->GetRows = ArraySGetRows;
    output->GetColumns = ArraySGetColumns;
    output->GetHeader = ArraySGetHeader;
    output->Free = ArraySFree;
    output->Print = ArraySPrint;
    output->Duplicate = ArraySDuplicate;
    output->Copy = ArraySCopy;
    output->Append = ArraySAppend;
    output->Sort = ArraySSort;
    output->SjisToUtf8 = ArraySSjisToUtf8;
    output->Utf8ToSjis = ArraySUtf8ToSjis;
    output->StringsToCSV = ArraySStringsToCSV;
    output->CSVToStrings = ArraySCSVToStrings;
    output->LoadCSV = ArraySLoadCSV;
    output->SaveCSV = ArraySSaveCSV;
    output->ArraySToStrings = ArraySArraySToStrings;
    output->StringsToArrayS = ArraySStringsToArrayS;
    output->rows = rows;
    output->columns = columns;
    output->data = (Strings**)malloc(sizeof(Strings*) * (rows * columns));
    output->header = new_Strings(header);
    if(data) {
        for(int number = 0; number < (rows * columns); ++number) {
            output->Get(output)[number] = new_Strings(data[number]);
        }
    } else {
        for(int number = 0; number < (rows * columns); ++number) {
            output->Get(output)[number] = new_Strings(NULL);
        }
    }
    return(output);
}
void delete_ArrayS(ArrayS **arr) {
    if(*arr) {
        (*arr)->Free(*arr);
        free(*arr);
    }
    *arr = NULL;
    return;
}
