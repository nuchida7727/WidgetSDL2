WidgetSDL2
====

# Overview
WidgetSDL2 - C99/SDL2によるクロスプラットホームなシンプルGUI

# Description
C99によるクロスプラットホームに対応したシンプルGUI

# Domo
サンプル(gui_test)  
```c:main.c
#include "SDL2/SDL.h"
#include "../../include/guiglobal.h"
bool Run(void) {
    printf("Hello World!, 日本語テスト\n");
    return(true);
}
```
サンプル(widget_test)  
![サンプル](./doc/sample.jpg)

# Requirement
OS  
Windows or Linux or Android  
GCC 5.1 or Visual Stduio 2015 or C4droid  
SDL2  

テスト環境
Windows7(32bit)  
Android(C4droid)  
Raspberry Pi3  

# Usage
only include header

# Install
only include header

# License
MIT

# Author
nuchida7727@adagio.ocn.ne.jp

#Reference
ke2k-tools  
https://gitorious.org/ke2k-toolkit/ke2k-toolkit  

