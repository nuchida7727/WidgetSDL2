#pragma once
#include "arrays.h"
#include "arrayd.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mixer.h"
#undef main
typedef struct Guibasic {
    int window_width;
    int window_height;
    Strings *str_all;
    SDL_Window *sdl_window;
    SDL_Renderer *sdl_renderer;
    SDL_Texture *sdl_texture;
    TTF_Font *ttf_font;
    TTF_Font *ttf_font_small;
    TTF_Font *ttf_font_big;
    void (*Free)(struct Guibasic *self);
    void (*RenderPresent)(struct Guibasic *self);
    void (*Cls)(struct Guibasic *self);
    int (*CharGetting)(struct Guibasic *self);
    Strings *(*StringsGetting)(struct Guibasic* self, char *char_init);
    void (*Printf)(struct Guibasic *self, char *format, ...);
} Guibasic;
bool is_androidactive = 0; // for android
Guibasic *new_Guibasic(int width, int height);
void delete_Guibasic(Guibasic **gui);
void GuibasicMsgbox(char *format, ...) {
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
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, (char*)"", buffer, NULL);
    free(buffer);
    return;
}
int AndroidEventFilter(void *userdata, SDL_Event *event) {
    if(event->type == SDL_APP_WILLENTERBACKGROUND) {
        //printf((char*)"SDL_APP_WILLENTERBACKGROUND\n");
        SDL_StopTextInput();
        is_androidactive = false;
        return(1);
    } else if(event->type == SDL_APP_WILLENTERFOREGROUND) {
        //printf((char*)"SDL_APP_WILLENTERFOREGROUND\n");
        SDL_StartTextInput();
        is_androidactive = true;
        return(1);
    }
    return(0);
}
int GuibasicGetline(char *str) {
    int output = 0;
    for(int number = 0; str[number] != '\0'; ++number) {
        if(str[number] == '\n') {
            output += 1;
        }
    }
    return(output);
}
void GuibasicFree(Guibasic *self) {
    if(self->ttf_font) {
        TTF_CloseFont(self->ttf_font);
    }
    if(self->ttf_font_small) {
        TTF_CloseFont(self->ttf_font_small);
    }
    if(self->ttf_font_big) {
        TTF_CloseFont(self->ttf_font_big);
    }
    TTF_Quit();
    IMG_Quit();
    SDL_DestroyWindow(self->sdl_window);
    SDL_DestroyRenderer(self->sdl_renderer);
    SDL_DestroyTexture(self->sdl_texture);
    SDL_Quit();
    delete_Strings(&(self->str_all));
    *self = (Guibasic) {
        0
    };
    return;
}
void GuibasicRenderPresent(Guibasic *self) {
    SDL_SetRenderDrawColor(self->sdl_renderer, 0, 0, 0, 255);
    SDL_Rect sdl_rect = (SDL_Rect) {
        0, 0, self->window_width, self->window_height
    };
    SDL_SetRenderTarget(self->sdl_renderer, NULL);
    SDL_RenderCopy(self->sdl_renderer, self->sdl_texture, NULL, &sdl_rect);
    SDL_RenderPresent(self->sdl_renderer);
    SDL_SetRenderTarget(self->sdl_renderer, self->sdl_texture);
    return;
}
void GuibasicCls(Guibasic *self) {
    self->str_all->Set(self->str_all, NULL);
    return;
}
void GuibasicSetMessage(Guibasic *self) {
    SDL_Surface *sdl_surface = NULL;
    if(self->str_all->Compare(self->str_all, NULL)) {
        sdl_surface = TTF_RenderUTF8_Blended_Wrapped(self->ttf_font, (char*)"(null)", (SDL_Color) {
            0xff, 0xff, 0xff
        }, self->window_width);
    } else {
        sdl_surface = TTF_RenderUTF8_Blended_Wrapped(self->ttf_font, self->str_all->Get(self->str_all), (SDL_Color) {
            0xff, 0xff, 0xff
        }, self->window_width);
    }
    SDL_Rect sdl_rect = (SDL_Rect) {
        0, 0, sdl_surface->w, sdl_surface->h
    };
    SDL_Texture *sdl_texture_text = SDL_CreateTextureFromSurface(self->sdl_renderer, sdl_surface);
    SDL_SetTextureBlendMode(sdl_texture_text, SDL_BLENDMODE_ADD);
    SDL_RenderClear(self->sdl_renderer);
    int text_width = 0;
    int text_height = 0;
    SDL_QueryTexture(sdl_texture_text, NULL, NULL, &text_width, &text_height);
    SDL_Rect src_rect = {0, 0, min_int(sdl_rect.w - 2, text_width), min_int(sdl_rect.h, text_height)};
    SDL_RenderCopyEx(self->sdl_renderer, sdl_texture_text, &src_rect, &src_rect, 0, NULL, SDL_FLIP_NONE);
    SDL_DestroyTexture(sdl_texture_text);
    sdl_texture_text = NULL;
    SDL_FreeSurface(sdl_surface);
    self->RenderPresent(self);
    return;
}
void GuibasicPrintf(Guibasic *self, char *format, ...) {
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
    self->str_all->Set(self->str_all, (char*)"%s%s", self->str_all->Get(self->str_all), buffer);
    free(buffer);
    if(GuibasicGetline(self->str_all->Get(self->str_all)) <= 14) {
        GuibasicSetMessage(self);
    } else {
        self->str_all->Set(self->str_all, (char*)"%s%s", self->str_all->Get(self->str_all), (char*)"\n--- Hit any key ---\n");
        GuibasicSetMessage(self);
        self->Cls(self);
        self->CharGetting(self);
    }
    return;
}
int GuibasicCharGetting(Guibasic *self) {
    int output = -1;
    bool is_run = true;
    SDL_Event event = {0};
    SDL_StartTextInput();
    while(is_run) {
        SDL_Delay(100);
        if(!is_androidactive) {
            continue;
        }
        while(SDL_PollEvent(&event) != 0) {
            switch(event.type) {
            case(SDL_QUIT): {
                //delete_Guibasic(&self);
                //exit(0);
                is_run = false;
            }
            case(SDL_WINDOWEVENT): {
                self->RenderPresent(self);
                break;
            }
            case(SDL_MOUSEBUTTONUP): {
                //SDL_StartTextInput();
                //self->RenderPresent(self);
                is_run = false;
                break;
            }
            case(SDL_KEYDOWN): {
                switch(event.key.keysym.sym) {
                case(SDLK_KP_0):
                case(SDLK_0): {
                    output = 0;
                    is_run = false;
                    break;
                }
                case(SDLK_KP_1):
                case(SDLK_1): {
                    output = 1;
                    is_run = false;
                    break;
                }
                case(SDLK_KP_2):
                case(SDLK_2): {
                    output = 2;
                    is_run = false;
                    break;
                }
                case(SDLK_KP_3):
                case(SDLK_3): {
                    output = 3;
                    is_run = false;
                    break;
                }
                case(SDLK_KP_4):
                case(SDLK_4): {
                    output = 4;
                    is_run = false;
                    break;
                }
                case(SDLK_KP_5):
                case(SDLK_5): {
                    output = 5;
                    is_run = false;
                    break;
                }
                case(SDLK_KP_6):
                case(SDLK_6): {
                    output = 6;
                    is_run = false;
                    break;
                }
                case(SDLK_KP_7):
                case(SDLK_7): {
                    output = 7;
                    is_run = false;
                    break;
                }
                case(SDLK_KP_8):
                case(SDLK_8): {
                    output = 8;
                    is_run = false;
                    break;
                }
                case(SDLK_KP_9):
                case(SDLK_9): {
                    output = 9;
                    is_run = false;
                    break;
                }
                default: {
                    is_run = false;
                    break;
                }
                }
            }
            }
        }
    }
    SDL_StopTextInput();
    return(output);
}
Strings *GuibasicStringsGetting(Guibasic *self, char *char_init) {
    self->Cls(self);
    Strings *str = new_Strings(self->str_all->Get(self->str_all));
    Strings *output = new_Strings(NULL);
    Strings *output_ime = new_Strings(NULL);
    output->Set(output, (char*)"%s", char_init);
    bool is_run = true;
    SDL_Event event = {0};
    SDL_StartTextInput();
    SDL_SetTextInputRect(NULL);
    bool is_first = true;
    while(is_run) {
        SDL_Delay(100);
        //self->RenderPresent(self);
        if(is_androidactive) {
            bool is_render = false;
            while(SDL_PollEvent(&event) != 0) {
                switch(event.type) {
                case(SDL_QUIT): {
                    //delete_Guibasic(&self);
                    //exit(0);
                    is_run = false;
                }
                case(SDL_WINDOWEVENT): {
                    self->RenderPresent(self);
                    break;
                }
                case(SDL_MOUSEBUTTONUP): {
                    //SDL_StartTextInput();
                    //self->RenderPresent(self);
                    is_run = false;
                    break;
                }
                case(SDL_KEYDOWN): {
                    if(event.key.keysym.sym == SDLK_RETURN ||event.key.keysym.sym == SDLK_RETURN2 || event.key.keysym.sym == SDLK_KP_ENTER) {
                        is_render = true;
                        if(output_ime->Compare(output_ime, NULL)) {
                            is_run = false;
                        }
                    } else if(event.key.keysym.sym == SDLK_BACKSPACE && output->GetSizes(output) > 0) {
                        if(output_ime->Compare(output_ime, NULL)) {
                            output->Backspace(output);
                            is_render = true;
                        }
                    } else if((event.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL) || event.key.keysym.sym == SDLK_LEFT) {
                        SDL_SetClipboardText(output->Get(output));
                    } else if((event.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL) || event.key.keysym.sym == SDLK_RIGHT) {
                        output->Set(output, (char*)"%s%s", output->Get(output), SDL_GetClipboardText());
                        is_render = true;
                    }
                    break;
                }
                case(SDL_TEXTINPUT): {
                    if(!((event.text.text[0] == 'c' || event.text.text[0] == 'C') && (event.text.text[0] == 'v' || event.text.text[0] == 'V') && SDL_GetModState() & KMOD_CTRL)) {
                        output->Set(output, (char*)"%s%s", output->Get(output), event.edit.text);
                        is_render = true;
                        output_ime->Set(output_ime, NULL);
                    }
                    break;
                }
                case(SDL_TEXTEDITING): {
                    if(!((event.text.text[0] == 'c' || event.text.text[0] == 'C') && (event.text.text[0] == 'v' || event.text.text[0] == 'V') && SDL_GetModState() & KMOD_CTRL)) {
                        output_ime->Set(output_ime, (char*)"%s", event.edit.text);
                        is_render = true;
                    }
                    break;
                }
                }
            }
            if(is_render || is_first) {
                GuibasicCls(self);
                GuibasicPrintf(self, (char*)"%sinput text = %s%s\n", str->Get(str), output->Get(output), output_ime->Get(output_ime));
                is_first = false;
            }
        }
    }
    SDL_StopTextInput();
    delete_Strings(&str);
    delete_Strings(&output_ime);
    return(output);
}
Guibasic *new_Guibasic(int width, int height) {
    Guibasic *output = (Guibasic*)malloc(sizeof(Guibasic));
    *output = (Guibasic) {
        0
    };
    output->window_width = width;
    output->window_height = height;
    is_androidactive = true;
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        GuibasicMsgbox((char*)"SDL could not initialize! SDL Error: %s", SDL_GetError());
        exit(1);
    }
    //sdl_window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_SHOWN);
    output->sdl_window = SDL_CreateWindow(NULL, 10, 20, output->window_width, output->window_height, SDL_WINDOW_SHOWN);
    if(!output->sdl_window) {
        GuibasicMsgbox((char*)"Window could not be created! SDL Error: %s", SDL_GetError());
        exit(1);
    }
    TTF_Init();
    char *path_windows = (char*)"U:/OneDrive/30_c/lib/fonts/ipaexg.ttf";
    char *path_android = (char*)"/sdcard/_backup/font/ipaexg.ttf";
    char *path_current = (char*)"./ipaexg.ttf";
    output->ttf_font = TTF_OpenFont(path_windows, 36);
    if(!output->ttf_font) {
        output->ttf_font = TTF_OpenFont(path_android, 36);
        if(!output->ttf_font) {
            output->ttf_font = TTF_OpenFont(path_current, 36);
            if(!output->ttf_font) {
                GuibasicMsgbox((char*)"TTF could not be opened! SDL Error: %s", SDL_GetError());
                exit(1);
            }
        }
    }
    output->ttf_font_small = TTF_OpenFont(path_windows, 24);
    if(!output->ttf_font_small) {
        output->ttf_font_small = TTF_OpenFont(path_android, 24);
        if(!output->ttf_font_small) {
            output->ttf_font_small = TTF_OpenFont(path_current, 24);
            if(!output->ttf_font_small) {
                GuibasicMsgbox((char*)"TTF could not be opened! SDL Error: %s", SDL_GetError());
                exit(1);
            }
        }
    }
    output->ttf_font_big = TTF_OpenFont(path_windows, 65);
    if(!output->ttf_font_big) {
        output->ttf_font_big = TTF_OpenFont(path_android, 65);
        if(!output->ttf_font_big) {
        output->ttf_font_big = TTF_OpenFont(path_current, 65);
            if(!output->ttf_font_big) {
                GuibasicMsgbox((char*)"TTF could not be opened! SDL Error: %s", SDL_GetError());
                exit(1);
            }
        }
    }
    TTF_SetFontHinting(output->ttf_font, TTF_HINTING_NONE);
    TTF_SetFontHinting(output->ttf_font_small, TTF_HINTING_NONE);
    TTF_SetFontHinting(output->ttf_font_big, TTF_HINTING_NONE);
    output->sdl_renderer = SDL_CreateRenderer(output->sdl_window, -1, SDL_RENDERER_ACCELERATED);
    //sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_SOFTWARE | SDL_RENDERER_TARGETTEXTURE);
    if(!output->sdl_renderer) {
        printf((char*)"Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        exit(1);
    }
    output->sdl_texture = SDL_CreateTexture(output->sdl_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, output->window_width, output->window_height);
    if(!output->sdl_texture) {
        printf((char*)"Texture could not be created! SDL Error: %s\n", SDL_GetError());
        exit(1);
    }
    SDL_SetRenderTarget(output->sdl_renderer, output->sdl_texture);
    SDL_AddEventWatch(&AndroidEventFilter, NULL);
    output->str_all = new_Strings(NULL);
    output->Free = GuibasicFree;
    output->RenderPresent = GuibasicRenderPresent;
    output->Cls = GuibasicCls;
    output->CharGetting = GuibasicCharGetting;
    output->StringsGetting = GuibasicStringsGetting;
    output->Printf = GuibasicPrintf;
    return(output);
}
void delete_Guibasic(Guibasic **gui) {
    if(*gui) {
        (*gui)->Free(*gui);
        free(*gui);
    }
    *gui = NULL;
}
