#pragma once
int remaining_malloc = 0;
#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "stddef.h"
#include "crtdbg.h"  // for Visual C++
#ifdef _WIN32
#include "process.h"  // for Visual C++
#endif // _WIN32
#include "time.h"
bool Run(void);
#if (WINDOWMODE == CUI)
int main(int argc, char *argv[]) {
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_DELAY_FREE_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF);
    //clock_t time_start = clock();
    time_t time_start = 0;
    time(&time_start);
    printf((char*)"--- Start ---\n");
    int remaining_malloc_before = remaining_malloc;
    Run();
    int remaining_malloc_after = remaining_malloc;
    //clock_t time_end = clock();
    time_t time_end = 0;
    time(&time_end);
    printf((char*)"--- End ---\n");
    //printf((char*)"TotalSeconds = %g sec\n", (double)(time_end - time_start) / CLOCKS_PER_SEC);
    printf((char*)"TotalSeconds = %g sec\n", difftime(time_end, time_start));
    if(remaining_malloc_before != remaining_malloc_after) {
        printf((char*)"\n=== MEMORY LEAK ===\nremaining_malloc = %d\n", remaining_malloc_after - remaining_malloc_before);
    }
    return(EXIT_SUCCESS);
}
// #endif  // #if (WINDOWMODE == CUI)
#elif (WINDOWMODE == GUI)
void GuiglobalPrintf(char *format, ...);
#define printf GuiglobalPrintf
int GuiglobalCharGetting(void);
#define getchar() GuiglobalCharGetting()
void GuiglobalCls(void);
#define system(str); GuiglobalCls();
void new_Guiglobal(int width, int height);
void delete_Guiglobal(void);
int main(int argc, char *argv[]) {
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_DELAY_FREE_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF);
    //clock_t time_start = clock();
    time_t time_start = 0;
    time(&time_start);
    new_Guiglobal(1080, 700);
    printf((char*)"--- Start ---\n");
    int remaining_malloc_before = remaining_malloc;
    Run();
    int remaining_malloc_after = remaining_malloc;
    //clock_t time_end = clock();
    time_t time_end = 0;
    time(&time_end);
    printf((char*)"--- End ---\n");
    //printf((char*)"TotalSeconds = %g sec\n", (double)(time_end - time_start) / CLOCKS_PER_SEC);
    printf((char*)"TotalSeconds = %g sec\n", difftime(time_end, time_start));
    if(remaining_malloc_before != remaining_malloc_after) {
        printf((char*)"\n=== MEMORY LEAK ===\nremaining_malloc = %d\n", remaining_malloc_after - remaining_malloc_before);
    }
    getchar();
    delete_Guiglobal();
    return(EXIT_SUCCESS);
}
#endif // #if (WINDOWMODE == GUI)
void *malloc_leak(size_t sizes) {
    void *output = malloc(sizes);
    if(output) {
        remaining_malloc += 1;
    }
    return(output);
}
void free_leak(void *data) {
    if(data) {
        remaining_malloc -= 1;
        free(data);
    }
    return;
}
#define malloc malloc_leak
#define free free_leak
