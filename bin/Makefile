Makefile :=)
# Name
TARGET := main
TARGET_DLL :=
# Macro
MODE := CONSOLE
CC := gcc.exe
CXX := g++.exe
CFLAGS_CC := -Wall -O2 -Winit-self -Wno-missing-field-initializers -std=c99 -finput-charset=utf-8 -fexec-charset=utf-8 -lm -fopenmp
CFLAGS_CXX := -Wall -O2 -Winit-self -Wno-missing-field-initializers -std=c++11 -finput-charset=utf-8 -fexec-charset=utf-8 -lm -fopenmp
RM := del
# Path
PATH_SDL2 := U:/Application/SDL2
PATH_INCLUDE := -I$(PATH_SDL2)/include
PATH_MAKE :=
PATH_LIB := $(PATH_SDL2)/lib/*.lib
# Suffixes
.SUFFIXES: .c .o
.SUFFIXES: .cpp .o
.SUFFIXES: .o .dll
# Rule
$(TARGET).exe: $(TARGET).o
ifeq ($(MODE),WINDOWS)
	$(CXX) -s -lgdi32 -luser32 -lkernel32 -lcomctl32 -o $@ $(TARGET).o $(PATH_LIB) -mwindows
else
	$(CXX) -s -o $@ $(TARGET).o $(PATH_LIB)
endif
# Rule(Sufffixes)
.o.dll: $(TARGET_DLL)
	$(CXX) -shared -o $(PATH_MAKE)$@ $<
.c.o:
	$(CC) $(CFLAGS_CC) $(PATH_INCLUDE) -c $<
.cpp.o:
	$(CXX) $(CFLAGS_CXX) $(PATH_INCLUDE) -c $<
# Clean
clean:
	$(RM) $(TARGET).exe
#	$(RM) *.dll
	$(RM) *.o
