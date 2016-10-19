#pragma once
#include "arrays.h"
#include "math.h"
#include "float.h"
#ifndef M_E
#define M_E 2.7182818284590452354
#endif
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif
int max_int(int x, int y) {
    int output = 0;
    if(x > y) {
        output = x;
    } else {
        output = y;
    }
    return(output);
}
int min_int(int x, int y) {
    int output = 0;
    if(x > y) {
        output = y;
    } else {
        output = x;
    }
    return(output);
}
double max_double(double x, double y) {
    double output = 0;
    if(x > y) {
        output = x;
    } else {
        output = y;
    }
    return(output);
}
double min_double(double x, double y) {
    double output = 0;
    if(x > y) {
        output = y;
    } else {
        output = x;
    }
    return(output);
}
int clamp_int(int min_i, int value, int max_i) {
    int output = 0;
    if((value - min_i) < 0) {
        output = min_i;
    } else if((value - max_i) > 0) {
        output = max_i;
    } else {
        output = value;
    }
    return(output);
}
double clamp_double(double min_d, double value, double max_d) {
    double output = 0;
    if((value - min_d) < 0) {
        output = min_d;
    } else if((value - max_d) > 0) {
        output = max_d;
    } else {
        output = value;
    }
    return(output);
}
typedef struct ArrayD {
    int rows;
    int columns;
    double *data;
    Strings *header;
    unsigned int seed;
    double *(*Get)(struct ArrayD *self);
    int (*GetRows)(struct ArrayD *self);
    int (*GetColumns)(struct ArrayD *self);
    char *(*GetHeader)(struct ArrayD *self);
    void (*SetHeader)(struct ArrayD *self, char *header);
    void (*Print)(struct ArrayD *self);
    struct ArrayD *(*Duplicate)(struct ArrayD *self);
    void (*Free)(struct ArrayD *self);
    void (*Copy)(struct ArrayD *self, struct ArrayD *arr);
    void (*Linspace)(struct ArrayD *self, double start, double last, int numbers);
    double (*Select)(struct ArrayD *self, int row, char *column);
    void (*RandomSeed)(struct ArrayD *self, unsigned int seed);
    void (*Random)(struct ArrayD *self);
    void (*Round)(struct ArrayD *self, bool round_up);
    void (*Sort)(struct ArrayD *self, char *column);
    void (*Transpose)(struct ArrayD *self);
    void (*Invert)(struct ArrayD *self);
    void (*Multiply)(struct ArrayD *self, struct ArrayD *arr);
    struct ArrayD *(*Solve)(struct ArrayD *self, struct ArrayD *arr);
    struct ArrayD *(*Histogram)(struct ArrayD *self, double x_min, double x_max, double interval);
    void (*Normalize)(struct ArrayD *self);
    double (*Simplex)(struct ArrayD *self, double (*func)(struct ArrayD *arr));
    struct ArrayD *(*Spline)(struct ArrayD *self, struct ArrayD *arr);
    void (*LoadCSV)(struct ArrayD *self, char *path);
    void (*SaveCSV)(struct ArrayD *self, char *path);
} ArrayD;
ArrayD *new_ArrayD(int rows, int columns, double *data, char *header);
void delete_ArrayD(ArrayD **arr);
double *ArrayDGet(ArrayD *self) {
    double *output = self->data;
    return(output);
}
int ArrayDGetRows(ArrayD *self) {
    int output = self->rows;
    return(output);
}
int ArrayDGetColumns(ArrayD *self) {
    int output = self->columns;
    return(output);
}
char *ArrayDGetHeader(ArrayD *self) {
    char *output = self->header->Get(self->header);
    return(output);
}
void ArrayDSetHeader(ArrayD *self, char *header) {
    self->header->Set(self->header, header);
    return;
}
void ArrayDPrint(ArrayD *self) {
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
            str->Set(str, (char*)"%s%g  ", str->Get(str), self->Get(self)[row * self->GetColumns(self) + column]);
        }
        str->Set(str, (char*)"%s\n", str->Get(str));
    }
    str->Set(str, (char*)"%s----------\n", str->Get(str));
    printf(str->Get(str));
    delete_Strings(&str);
    return;
}
ArrayD *ArrayDDuplicate(ArrayD *self) {
    ArrayD *arr = new_ArrayD(self->GetRows(self), self->GetColumns(self), self->Get(self), self->GetHeader(self));
    return(arr);
}
void ArrayDFree(ArrayD *self) {
    if(self->Get(self)) {
        free(self->Get(self));
    }
    delete_Strings(&(self->header));
    *self = (ArrayD) {
        0
    };
    return;
}
void ArrayDCopy(ArrayD *self, ArrayD *arr) {
    ArrayD *temp = arr->Duplicate(arr);
    self->Free(self);
    *self = *temp;
    free(temp);
    return;
}
void ArrayDLinspace(ArrayD *self, double start, double last, int numbers) {
    ArrayD *arr_new = new_ArrayD(numbers, 1, NULL, self->GetHeader(self));
    for(int number = 0; number < numbers; ++number) {
        arr_new->Get(arr_new)[number] = start + ((last - start) / (numbers - 1)) * number;
    }
    self->Copy(self, arr_new);
    delete_ArrayD(&arr_new);
    return;

}
int ArrayDSelect_column(ArrayD *self, char *column) {
    if(!column) {
        return(0);
    }
    Strings *str = new_Strings(NULL);
    str->Set(str, (char*)"%s", self->GetHeader(self));
    char *tok = strtok(str->Get(str), (char*)"\n");  // comment
    int output = 0;
    while(tok) {
        tok = strtok(NULL, (char*)",\n");
        if(tok) {
            output += 1;
            Strings *strings_tok = new_Strings(tok);
            if(strings_tok->Compare(strings_tok, column)) {
                delete_Strings(&strings_tok);
                break;
            }
            delete_Strings(&strings_tok);
        } else {
            printf((char*)"ArrayDSelect: not found\n");
            return(0);
        }
    }
    delete_Strings(&str);
    output -= 1;
    return(output);
}
double ArrayDSelect(ArrayD *self, int row, char *column) {
    if(row < 0) {
        return(0);
    }
    int number_column = ArrayDSelect_column(self, column);
    double output = self->Get(self)[row * self->GetColumns(self) + number_column];
    return(output);
}
void ArrayDRandomSeed(ArrayD *self, unsigned int seed) {
    if(seed == 0) {
        self->seed = (unsigned int)time(NULL) + 2463534242;
    } else {
        self->seed = seed;
    }
    return;
}
double ArrayDSetRandom(ArrayD *self) {
    // http://d.hatena.ne.jp/pashango_p/20090717/1247848900
    // http://www.jstatsoft.org/v08/i14/
    self->seed ^= self->seed << 13;
    self->seed ^= self->seed >> 17;
    self->seed ^= self->seed << 5;
    double output = (double)(self->seed / 4294967295.0);
    return(output);
}
void ArrayDRandom(ArrayD *self) {
    for(int row = 0; row < self->GetRows(self); ++row) {
        for(int column = 0; column < self->GetColumns(self); ++column) {
            self->Get(self)[row * self->GetColumns(self) + column] = ArrayDSetRandom(self);
        }
    }
    return;
}
double ArrayDSetRounding(double x, bool round_up) {
    if(fabs(x - (int)x) <= DBL_MIN) {
        return(x);
    }
    double output = x - (int)x;
    int digits = 0;
    if(x < 0) {
        output = -output;
    }
    while(true) {
        if(output >= 1) {
            break;
        }
        output *= 10;
        digits += 1;
    }
    output = (int)output;
    if((x < 0 && round_up == false) || (x > 0 && round_up == true)) {
        output += 1;
    }
    for(int digit = 0; digit < digits; ++digit) {
        output /= 10.0;
    }
    if(x < 0) {
        output = -output;
    }
    output += (int)x;
    return(output);
}
void ArrayDRound(ArrayD *self, bool round_up) {
    for(int row = 0; row < self->GetRows(self); ++row) {
        for(int column = 0; column < self->GetColumns(self); ++column) {
            self->Get(self)[row * self->GetColumns(self) + column] = ArrayDSetRounding(self->Get(self)[row * self->GetColumns(self) + column], round_up);
        }
    }
    return;
}
void ArrayDSort(ArrayD *self, char *column) {
    int number_column = ArrayDSelect_column(self, column);
    for(int i = self->GetRows(self) - 1; i > 0; --i) {
        for(int j = 0; j < i; ++j) {
            if(self->Get(self)[j * self->GetColumns(self) + number_column] > self->Get(self)[(j + 1) * self->GetColumns(self) + number_column]) {
                for(int column = 0; column < self->GetColumns(self); ++column) {
                    double temp = self->Get(self)[j * self->GetColumns(self) + column];
                    self->Get(self)[j * self->GetColumns(self) + column] = self->Get(self)[(j + 1) * self->GetColumns(self) + column];
                    self->Get(self)[(j + 1) * self->GetColumns(self) + column] = temp;
                }
            }
        }
    }
    return;
}
void ArrayDTranspose(ArrayD *self) {
    ArrayD *arr_new = new_ArrayD(self->GetColumns(self), self->GetRows(self), NULL, self->GetHeader(self));
    for(int row = 0; row < self->GetRows(self); ++row) {
        for(int column = 0; column < self->GetColumns(self); ++column) {
            arr_new->Get(arr_new)[column * arr_new->GetColumns(arr_new) + row] = self->Get(self)[row * self->GetColumns(self) + column];
        }
    }
    self->Copy(self, arr_new);
    delete_ArrayD(&arr_new);
    return;
}
void ArrayDInvert(ArrayD *self) {
    if(self->GetRows(self) != self->GetColumns(self)) {
        printf((char*)"matrix is not square matrix (ArrayDInvert)");
        return;
    }
    int msize = self->GetRows(self);
    ArrayD *arr_temp = self->Duplicate(self);
    ArrayD *arr_new = new_ArrayD(msize, msize, NULL, self->GetHeader(self));
    double buffer = 0;
    for(int row = 0; row < msize; ++row) {
        for(int column = 0; column < msize; ++ column) {
            arr_new->Get(arr_new)[row * msize + column] = (row == column) ? 1 : 0;
        }
    }
    for(int i = 0; i < msize; ++i) {
        if(arr_temp->Get(arr_temp)[i * msize + i]) {
            buffer = 1 / arr_temp->Get(arr_temp)[i * msize + i];
        } else {
            buffer = 0;
        }
        for(int j = 0; j < msize; ++j) {
            arr_temp->Get(arr_temp)[i * msize + j] *= buffer;
            arr_new->Get(arr_new)[i * msize + j] *= buffer;
        }
        for(int j = 0; j < msize; ++j) {
            if(i != j) {
                buffer = arr_temp->Get(arr_temp)[j * msize + i];
                for(int k = 0; k < msize; ++k) {
                    arr_temp->Get(arr_temp)[j * msize + k] -= arr_temp->Get(arr_temp)[i * msize + k] * buffer;
                    arr_new->Get(arr_new)[j * msize + k] -= arr_new->Get(arr_new)[i * msize + k] * buffer;
                }
            }
        }
    }
    self->Copy(self, arr_new);
    delete_ArrayD(&arr_temp);
    delete_ArrayD(&arr_new);
    return;
}
void ArrayDMultiply(ArrayD *self, ArrayD *arr) {
    if(self->GetColumns(self) != arr->GetRows(arr)) {
        printf((char*)"matrix is not same size (ArrayDMultiply)\n");
        return;
    }
    int x_rows = self->GetRows(self);
    int x_columns = self->GetColumns(self);
    int y_columns = arr->GetColumns(arr);
    ArrayD *arr_new = new_ArrayD(self->GetRows(self), arr->GetColumns(arr), NULL, self->GetHeader(self));
    for(int row = 0; row < x_rows; ++row) {
        for(int column = 0; column < y_columns; ++column) {
            for(int multi = 0; multi < x_columns; ++multi) {
                arr_new->Get(arr_new)[row * arr_new->GetColumns(arr_new) + column] += self->Get(self)[row * x_columns + multi] * arr->Get(arr)[multi * y_columns + column];
            }
        }
    }
    self->Copy(self, arr_new);
    delete_ArrayD(&arr_new);
    return;
}
ArrayD *ArrayDSolve(ArrayD *self, ArrayD *arr) {
    // INPUT
    int rows = self->GetRows(self);
    int columns = self->GetColumns(self);
    ArrayD *Edata = new_ArrayD(rows, columns + 1, NULL, NULL);
    for(int row = 0; row < rows; ++row) {
        for(int column = 0; column < columns; ++column) {
            Edata->Get(Edata)[row * Edata->GetColumns(Edata) + column] = self->Get(self)[row * self->GetColumns(self) + column];
        }
        Edata->Get(Edata)[row * Edata->GetColumns(Edata) + columns] = arr->Get(arr)[row];
    }
    ArrayD *Mdata = new_ArrayD(rows, columns + 1, NULL, NULL);
    // OUTPUT
    ArrayD *arr_new = new_ArrayD(self->GetRows(self), 1, NULL, NULL);
    double *output = arr_new->Get(arr_new);
    if(rows != columns) {
        printf((char*)"not square");
        return(arr_new);
    }
    // STEP 1
    for(int i = 0; i < rows; ++i) {
        int p = 0;
        bool flag = false;
        // STEP 2
        for(int j = i; j < rows; ++j) {
            if(!(fabs(Edata->Get(Edata)[j * Edata->GetColumns(Edata) + i]) < DBL_MIN)) {
                flag = true;
                p = j;
                break;
            }
        }
        if(flag) {
            // STEP 3
            if(p != i) {
                for(int k = 0; k < rows + 1; ++k) {
                    double temp = Edata->Get(Edata)[p * Edata->GetColumns(Edata) + k];
                    Edata->Get(Edata)[p * Edata->GetColumns(Edata) + k] = Edata->Get(Edata)[i * Edata->GetColumns(Edata) + k];
                    Edata->Get(Edata)[i * Edata->GetColumns(Edata) + k] = temp;
                }
            }
            // STEP 4
            for(int j = i + 1; j < rows; ++j) {
                // STEP 5
                Mdata->Get(Mdata)[j * Mdata->GetColumns(Mdata) + i] = Edata->Get(Edata)[j * Edata->GetColumns(Edata) + i] / Edata->Get(Edata)[i * Edata->GetColumns(Edata) + i];
                // STEP 6
                for(int k = 0; k < rows + 1; ++k) {
                    Edata->Get(Edata)[j * Edata->GetColumns(Edata) + k] = Edata->Get(Edata)[j * Edata->GetColumns(Edata) + k] - Mdata->Get(Mdata)[j * Mdata->GetColumns(Mdata) + i] * Edata->Get(Edata)[i * Edata->GetColumns(Edata) + k];
                }
            }
        } else {
            printf((char*)"No unique solution exists.\n");
            return(arr_new);
        }
    }
    // STEP 7
    if(fabs(self->Get(self)[(rows - 1) * self->GetColumns(self) + (rows - 1)]) < DBL_MIN) {
        printf((char*)"No unique solution exists.\n");
        return(arr_new);
    }
    // STEP 8
    output[rows - 1] = Edata->Get(Edata)[(rows - 1) * Edata->GetColumns(Edata) + rows] / Edata->Get(Edata)[(rows - 1) * Edata->GetColumns(Edata) + (rows - 1)];
    // STEP 9
    for(int i = rows - 2; i >= 0; --i) {
        double sum = 0;
        for(int j = i; j <= rows - 1; ++j) {
            sum = sum + Edata->Get(Edata)[i * Edata->GetColumns(Edata) + j] * output[j];
        }
        output[i] = (Edata->Get(Edata)[i * Edata->GetColumns(Edata) + rows] - sum) / Edata->Get(Edata)[i * Edata->GetColumns(Edata) + i];
    }
    delete_ArrayD(&Edata);
    delete_ArrayD(&Mdata);
    return(arr_new);
}
ArrayD *ArrayDHistogram(ArrayD *self, double x_min, double x_max, double interval) {
    int rows = self->GetRows(self);
    int columns = self->GetColumns(self);
    ArrayD *arr_new = new_ArrayD((int)((x_max - x_min) / interval + 2), 2, NULL, (char*)"histogram\nmin,frequency\n");
    arr_new->Get(arr_new)[0 * arr_new->GetColumns(arr_new) + 0] = x_min;
    arr_new->Get(arr_new)[(arr_new->GetRows(arr_new) - 1) * arr_new->GetColumns(arr_new) + 0] = x_max;
    for(int row = 1; row < arr_new->GetRows(arr_new) - 1; ++row) {
        arr_new->Get(arr_new)[row * arr_new->GetColumns(arr_new) + 0] = x_min + interval * (row - 1);
    }
    for(int row = 0; row < rows; ++row) {
        for(int column = 0; column < columns; ++column) {
            if(x_min > self->Get(self)[row * columns + column]) {
                arr_new->Get(arr_new)[0 * arr_new->GetColumns(arr_new) + 1] += 1;
            } else if(x_max <= self->Get(self)[row * columns + column]) {
                arr_new->Get(arr_new)[(arr_new->GetRows(arr_new) - 1) * arr_new->GetColumns(arr_new) + 1] += 1;
            } else {
                for(int i = 1; i < arr_new->GetRows(arr_new) - 1; ++i) {
                    if((x_min + interval * (i - 1)) >= self->Get(self)[row * columns + column]) {
                        arr_new->Get(arr_new)[i * arr_new->GetColumns(arr_new) + 1] += 1;
                        break;
                    }
                }
            }
        }
    }
    return(arr_new);
}
void ArrayDNormalize(ArrayD *self) {
    for(int column = 0; column < self->GetColumns(self); ++column) {
        double sum = 0;
        double sum2 = 0;
        for(int row = 0; row < self->GetRows(self); ++row) {
            sum += self->Get(self)[row * self->GetColumns(self) + column];
            sum2 += self->Get(self)[row * self->GetColumns(self) + column] * self->Get(self)[row * self->GetColumns(self) + column];
        }
        double average = sum / self->GetRows(self);
        double stdev = sqrt((sum2 - sum * sum / self->GetRows(self)) / (self->GetRows(self) - 1));
        for(int row = 0; row < self->GetRows(self); ++row) {
            self->Get(self)[row * self->GetColumns(self) + column] = (self->Get(self)[row * self->GetColumns(self) + column] - average) / stdev;
        }
    }
    return;
}
double ArrayDSimplex(ArrayD *self, double (*func)(ArrayD *arr)) {
    // INPUT
    const int times = 1000;  // times
    const double k = 1;  // coefficent of initial value
    const double r1 = 0.7;  // reduce ratio
    const double r2 = 1.5;  // expand ratio
    const double TOL = DBL_MIN;  // tol
    int numbers = self->GetColumns(self);  // dimension
    double *x = self->Get(self);
    ArrayD *arr_x = new_ArrayD(1, numbers, NULL, NULL);
    // OUTPUT
    ArrayD *output_x = new_ArrayD(1, numbers, NULL, self->GetHeader(self));
    double output_y = 0;
    double yn = 0;
    ArrayD *xx = new_ArrayD(numbers + 1, numbers, NULL, NULL);
    ArrayD *yy = new_ArrayD(numbers + 1, 1, NULL, NULL);
    ArrayD *xg = new_ArrayD(1, numbers, NULL, NULL);
    ArrayD *xr = new_ArrayD(1, numbers, NULL, NULL);
    ArrayD *xn = new_ArrayD(1, numbers, NULL, NULL);
    int fh = -1;  // max
    int fg = -1;  // second max
    int fl = -1;  // min
    // initialize
    for(int i = 0; i < numbers + 1; ++i) {
        for(int j = 0; j < numbers; ++j) {
            xx->Get(xx)[i * xx->GetColumns(xx) + j] = x[j];
        }
        if(i > 0) {
            xx->Get(xx)[i * xx->GetColumns(xx)+ (i - 1)] += k;
        }
        for(int j = 0; j < numbers; ++j) {
            arr_x->Get(arr_x)[j] = xx->Get(xx)[i * xx->GetColumns(xx) + j];
        }
        yy->Get(yy)[i] = (*func)(arr_x);
    }
    // calculate max, min value
    for(int i = 0; i < numbers + 1; ++i) {
        if(fh < 0 || yy->Get(yy)[i] > yy->Get(yy)[fh]) {
            fh = i;
        }
        if(fl < 0 || yy->Get(yy)[i] < yy->Get(yy)[fl]) {
            fl = i;
        }
    }
    for(int i = 0; i < numbers + 1; ++i) {
        if(i != fh && (fg < 0 || yy->Get(yy)[i] > yy->Get(yy)[fg])) {
            fg = i;
        }
    }
    // calculate min value
    int counts = 0;
    while(counts < times) {
        counts += 1;
        // calculate the center of gravity
        for(int i = 0; i < numbers; ++i) {
            xg->Get(xg)[i] = 0;
        }
        for(int i = 0; i < numbers + 1; ++i) {
            if(i != fh) {
                for(int j = 0; j < numbers; ++j) {
                    xg->Get(xg)[j] += xx->Get(xx)[i * xx->GetColumns(xx) + j];
                }
            }
        }
        for(int i = 0; i < numbers; ++i) {
            xg->Get(xg)[i] = xg->Get(xg)[i] / numbers;
        }
        // swap max value
        for(int i = 0; i < numbers; ++i) {
            xr->Get(xr)[i] = 2 * xg->Get(xg)[i] - xx->Get(xx)[fh * xx->GetColumns(xx) + i];
        }
        double yr = (*func)(xr);
        // reduce
        if(yr >= yy->Get(yy)[fh]) {
            for(int i = 0; i < numbers; ++i) {
                xr->Get(xr)[i] = (1 - r1) * xx->Get(xx)[fh * xx->GetColumns(xx) + i] + r1 * xr->Get(xr)[i];
            }
            yr = (*func)(xr);
        }
        // expand
        else if(yr < (yy->Get(yy)[fl] + (r2 - 1) * yy->Get(yy)[fh]) / r2) {
            for(int i = 0; i < numbers; ++i) {
                xn->Get(xn)[i] = r2 * xr->Get(xr)[i] - (r2 - 1) * xx->Get(xx)[fh * xx->GetColumns(xx) + i];
            }
            yn = (*func)(xn);
            if(yn <= yr) {
                for(int i = 0; i < numbers; ++i) {
                    xr->Get(xr)[i] = xn->Get(xn)[i];
                }
                yr = yn;
            }
        }
        for(int i = 0; i < numbers; ++i) {
            xx->Get(xx)[fh * xx->GetColumns(xx) + i] = xr->Get(xr)[i];
        }
        yy->Get(yy)[fh] = yr;
        // reduce all
        if(yy->Get(yy)[fh] >= yy->Get(yy)[fg]) {
            for(int i = 0; i < numbers + 1; ++i) {
                if(i != fl) {
                    for(int j = 0; j < numbers; ++j) {
                        xx->Get(xx)[i * xx->GetColumns(xx) + j] = 0.5 * (xx->Get(xx)[i * xx->GetColumns(xx) + j] + xx->Get(xx)[fl * xx->GetColumns(xx) + j]);
                    }
                    for(int j = 0; j < numbers; ++j) {
                        arr_x->Get(arr_x)[j] = xx->Get(xx)[i * xx->GetColumns(xx) + j];
                    }
                    yy->Get(yy)[i] = (*func)(arr_x);
                }
            }
        }
        // calculate max, min value
        fh = -1;
        fg = -1;
        fl = -1;
        for(int i = 0; i < numbers + 1; ++i) {
            if(fh < 0 || yy->Get(yy)[i] > yy->Get(yy)[fh]) {
                fh = i;
            }
            if(fl < 0 || yy->Get(yy)[i] < yy->Get(yy)[fl]) {
                fl = i;
            }
        }
        for(int i = 0; i < numbers + 1; ++i) {
            if(i != fh && (fg < 0 || yy->Get(yy)[i] > yy->Get(yy)[fg])) {
                fg = i;
            }
        }
        // convergence test
        double e = 0;
        for(int i = 0; i < numbers + 1; ++i) {
            if(i != fl) {
                yr = yy->Get(yy)[i] - yy->Get(yy)[fl];
                e += yr * yr;
            }
        }
        if(e < TOL) {
            output_y = yy->Get(yy)[fl];
            for (int i = 0; i < numbers; ++i) {
                output_x->Get(output_x)[i] = xx->Get(xx)[fl * xx->GetColumns(xx) + i];
            }
            break;
        }
    }
    self->Copy(self, output_x);
    delete_ArrayD(&output_x);
    delete_ArrayD(&arr_x);
    delete_ArrayD(&xx);
    delete_ArrayD(&yy);
    delete_ArrayD(&xg);
    delete_ArrayD(&xr);
    delete_ArrayD(&xn);
    return(output_y);
}
ArrayD *ArrayDSpline(ArrayD *self, ArrayD *arr_x) {
    int rows = self->GetRows(self);
    ArrayD *x = new_ArrayD(rows, 1, NULL, NULL);
    ArrayD *a = new_ArrayD(rows, 1, NULL, NULL);
    ArrayD *alpha = new_ArrayD(rows, 1, NULL, NULL);
    ArrayD *b = new_ArrayD(rows, 1, NULL, NULL);
    ArrayD *c = new_ArrayD(rows, 1, NULL, NULL);
    ArrayD *d = new_ArrayD(rows, 1, NULL, NULL);
    ArrayD *h = new_ArrayD(rows, 1, NULL, NULL);
    ArrayD *l = new_ArrayD(rows, 1, NULL, NULL);
    ArrayD *mu = new_ArrayD(rows, 1, NULL, NULL);
    ArrayD *z = new_ArrayD(rows, 1, NULL, NULL);
    // INPUT
    for(int row = 0; row < rows; ++row) {
        x->Get(x)[row] = self->Get(self)[row * self->GetColumns(self) + 0];
        a->Get(a)[row] = self->Get(self)[row * self->GetColumns(self) + 1];
    }
    // STEP 1
    for(int row = 0; row < rows - 1; ++row) {
        h->Get(h)[row] = x->Get(x)[row + 1] - x->Get(x)[row];
    }
    // STEP 2
    for(int row = 1; row < rows - 1; ++row) {
        alpha->Get(alpha)[row] = (3 * (a->Get(a)[row + 1] * h->Get(h)[row - 1] - a->Get(a)[row] * (x->Get(x)[row + 1] - x->Get(x)[row - 1]) + a->Get(a)[row - 1] * h->Get(h)[row])) / (h->Get(h)[row - 1] * h->Get(h)[row]);
    }
    // STEP 3
    l->Get(l)[0]  = 1;  // Steps 3, 4, 5, and part of step 6 solve
    mu->Get(mu)[0] = 0;  // a tridiagonal system using Algorithm 6.7.
    z->Get(z)[0]  = 0;
    // STEP 4
    for(int row = 1; row < rows - 1; ++row) {
        l->Get(l)[row] = 2 * (x->Get(x)[row + 1] - x->Get(x)[row - 1]) - h->Get(h)[row - 1] * mu->Get(mu)[row - 1];
        mu->Get(mu)[row] = h->Get(h)[row] / l->Get(l)[row];
        z->Get(z)[row] = (alpha->Get(alpha)[row] - h->Get(h)[row - 1] * z->Get(z)[row - 1]) / l->Get(l)[row];
    }
    // STEP 5
    l->Get(l)[rows - 1] = 1;
    z->Get(z)[rows - 1] = 0;
    c->Get(c)[rows - 1] = 0;
    // STEP 6
    for(int row = rows - 2; row >= 0; --row) {
        c->Get(c)[row] = z->Get(z)[row] - mu->Get(mu)[row] * c->Get(c)[row + 1];
        b->Get(b)[row] = (a->Get(a)[row + 1] - a->Get(a)[row]) / h->Get(h)[row] - h->Get(h)[row] * (c->Get(c)[row + 1] + 2 * c->Get(c)[row]) / 3;
        d->Get(d)[row] = (c->Get(c)[row + 1] - c->Get(c)[row]) / (3 * h->Get(h)[row]);
    }
    // OUTOPUT
    ArrayD *output = new_ArrayD(arr_x->GetRows(arr_x), arr_x->GetColumns(arr_x), NULL, NULL);
    for(int i = 0; i < arr_x->GetRows(arr_x) * arr_x->GetColumns(arr_x); ++i) {
        int number = rows - 1;
        double x_self = arr_x->Get(arr_x)[i];
        for(int j = 0; j < rows - 1; ++j) {
            if(x_self < x->Get(x)[j + 1]) {
                number = j;
                break;
            }
        }
        double x_data = x->Get(x)[number];
        output->Get(output)[i] = a->Get(a)[number] + b->Get(b)[number] * (x_self - x_data) + c->Get(c)[number] * pow(x_self - x_data, 2.0) + d->Get(d)[number] * pow(x_self - x_data, 3.0);
    }
    delete_ArrayD(&x);
    delete_ArrayD(&a);
    delete_ArrayD(&alpha);
    delete_ArrayD(&b);
    delete_ArrayD(&c);
    delete_ArrayD(&d);
    delete_ArrayD(&h);
    delete_ArrayD(&l);
    delete_ArrayD(&mu);
    delete_ArrayD(&z);
    return(output);
}
void ArrayDLoadCSV(ArrayD *self, char *path) {
    ArrayS *arr = new_ArrayS(0, 0, NULL, NULL);
    arr->LoadCSV(arr, path);
    ArrayD *arrayd = new_ArrayD(arr->GetRows(arr), arr->GetColumns(arr), NULL, arr->GetHeader(arr));
    for(int row = 0; row < arrayd->GetRows(arrayd); ++row) {
        for(int column = 0; column < arrayd->GetColumns(arrayd); ++column) {
            Strings *str = arr->Get(arr)[row * arr->GetColumns(arr) + column];
            arrayd->Get(arrayd)[row * arrayd->GetColumns(arrayd) + column] = atof(str->Get(str));
        }
    }
    self->Copy(self, arrayd);
    delete_ArrayD(&arrayd);
    delete_ArrayS(&arr);
}
void ArrayDSaveCSV(ArrayD *self, char *path) {
    int rows = self->GetRows(self);
    int columns = self->GetColumns(self);
    ArrayS *arr = new_ArrayS(rows, columns, NULL, self->GetHeader(self));
    for(int row = 0; row < rows; ++row) {
        for(int column = 0; column < columns; ++column) {
            arr->Get(arr)[row * columns + column]->Set(arr->Get(arr)[row * columns + column], (char*)"%g", self->Get(self)[row * columns + column]);
        }
    }
    arr->SaveCSV(arr, path);
    delete_ArrayS(&arr);
    return;
}
ArrayD *new_ArrayD(int rows, int columns, double *data, char *header) {
    ArrayD *output = (ArrayD*)malloc(sizeof(ArrayD));
    *output = (ArrayD) {
        0
    };
    output->Get = ArrayDGet;
    output->GetRows = ArrayDGetRows;
    output->GetColumns = ArrayDGetColumns;
    output->GetHeader = ArrayDGetHeader;
    output->SetHeader = ArrayDSetHeader;
    output->header = new_Strings(header);
    output->Print = ArrayDPrint;
    output->Duplicate = ArrayDDuplicate;
    output->Free = ArrayDFree;
    output->Copy = ArrayDCopy;
    output->Linspace = ArrayDLinspace;
    output->Select = ArrayDSelect;
    output->RandomSeed = ArrayDRandomSeed;
    output->Random = ArrayDRandom;
    output->Round = ArrayDRound;
    output->Sort = ArrayDSort;
    output->Transpose = ArrayDTranspose;
    output->Invert = ArrayDInvert;
    output->Multiply = ArrayDMultiply;
    output->Solve = ArrayDSolve;
    output->Histogram = ArrayDHistogram;
    output->Normalize = ArrayDNormalize;
    output->Simplex = ArrayDSimplex;
    output->Spline = ArrayDSpline;
    output->LoadCSV = ArrayDLoadCSV;
    output->SaveCSV = ArrayDSaveCSV;
    output->rows = rows;
    output->columns = columns;
    output->data = (double*)malloc(sizeof(double) * output->GetRows(output) * output->GetColumns(output));
    if(!output->Get(output)) {
        printf((char*)"malloc error(new_ArrayD)\n");
        return(output);
    }
    if(data) {
        memcpy(output->data, data, sizeof(double) * output->GetRows(output) * output->GetColumns(output));
    } else {
        memset(output->data, 0, sizeof(double) * output->GetRows(output) * output->GetColumns(output));
    }
    output->RandomSeed(output, 0);
    return(output);
}
void delete_ArrayD(ArrayD **arr) {
    if(*arr) {
        (*arr)->Free(*arr);
        free(*arr);
    }
    *arr = NULL;
    return;
}
