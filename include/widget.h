/*
The MIT License (MIT)

Copyright (c) 2016 nuchida7727

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#pragma once
#ifndef WINDOWMODE
#define CUI 1
#define WINDOWMODE CUI
#endif
#include "guibasic.h"
enum {
    WIDGET_BUTTON = 0,
    WIDGET_TOGGLE = 1,
    WIDGET_EDIT = 2,
    WIDGET_LABEL = 3,
    WIDGET_LISTS = 4,
    WIDGET_PAINTER = 5,
    WIDGET_CONTROL = 6,
    WIDGET_PANEL = 7,
    WIDGET_WINDOW = 8
};
int WIDGET_ARMED = 1 << 0;
int WIDGET_PRESSED = 1 << 1;
int WIDGET_DIRTY = 1 << 2;
int WIDGET_TOGGLEON = 1 << 3;
int WIDGET_MOUSEOVER = 1 << 4;
int WIDGET_CONTROLINT = 1 << 5;
int WIDGET_CHANGED = 1 << 6;
typedef struct Widget {
    int type;
    int flags;
    SDL_Rect rect;
    SDL_Texture *texture;
    TTF_Font *font;
    struct Widget *window;
    struct Widget *child;
    struct Widget *next;
    bool (*event)(struct Widget *widget, SDL_Event *event, SDL_Rect *rect);
    void *payload;
    bool enable;
    bool (*Changed)(struct Widget *widget);
    void (*SetText)(struct Widget *widget, char *format, ...);
    void (*SetEvent)(struct Widget *widget, bool (*event)(struct Widget *widget, SDL_Event *event, SDL_Rect *rect), void *payload);
    void (*SetFlags)(struct Widget *widget, int set_flags, int clear_flags);
    union detail {
        struct button {
            Strings *caption;
        } button;
        struct toggle {
            Strings *caption;
        } toggle;
        struct edit {
            Strings *strings_ime;
            Strings *strings_direct;
            Strings *text;
            Strings *(*GetEdit)(struct Widget *edit);
        } edit;
        struct label {
            Strings *text;
        } label;
        struct lists {
            ArrayS *arrays;
            int selected;
            int scroll;
            SDL_Surface *surface;   // when texture height > 8192, Not NULL
            void (*SetList)(struct Widget *lists, ArrayS *arrays);
            int (*GetSelected)(struct Widget *lists);
        } lists;
        struct painter {
            SDL_Surface *surface;
            void (*SetPainter)(struct Widget *painter, SDL_Surface *surface);
        } painter;
        struct control {
            double value;
            double value_min;
            double value_max;
            void (*SetRange)(struct Widget *control, double value_min, double value_max, bool integers);
            void (*SetValue)(struct Widget *control, double value);
        } control;
        struct window {
            char *title;
            Guibasic* gui;
            void (*EventAll)(struct Widget *window, SDL_Event *event);
            void (*PaintAll)(struct Widget *window);
            void (*EnableAll)(struct Widget *window);
            void (*DisenableAll)(struct Widget *window);
            void (*Loop)(struct Widget *window);
        } window;
    } detail;
} Widget;
typedef struct WidgetCalls {
    void (*event)(Widget *widget, SDL_Event *event, SDL_Rect *rect);
    void (*paint)(Widget *widget, SDL_Renderer *renderer, SDL_Rect *rect);
    void (*free)(Widget *widget);
} WidgetCalls;
SDL_Color WidgetColorGraylight = {128, 128, 128, 255};
SDL_Color WidgetColorGraydeep = {64, 64, 64, 255};
SDL_Color WidgetColorBackground = {0, 0, 0, 255};
SDL_Color WidgetColorText = {255, 255, 255, 255};
void WidgetSetFlags(Widget *widget, int set_flags, int clear_flags) {
    widget->flags |= set_flags;
    widget->flags &= ~clear_flags;
    return;
}
bool WidgetHasChild(Widget *widget) {
    bool output = widget->type >= WIDGET_PANEL && widget->child;
    return(output);
}
bool WidgetIsMouse_over_rect(SDL_Event *event, SDL_Rect *rect) {
    SDL_Point point = {0, 0};
    switch(event->type) {
    case(SDL_MOUSEMOTION): {
        point = (SDL_Point) {
            event->motion.x, event->motion.y
        };
        break;
    }
    case(SDL_MOUSEBUTTONDOWN):
    case(SDL_MOUSEBUTTONUP): {
        point = (SDL_Point) {
            event->button.x, event->button.y
        };
        break;
    }
    case(SDL_MOUSEWHEEL): {
        SDL_GetMouseState(&point.x, &point.y);
        break;
    }
    default: {
        return(false);
    }
    }
    bool output = (point.x >= rect->x) && (point.x < rect->x + rect->w) && (point.y >= rect->y) && (point.y < rect->y + rect->h);
    return(output);
}
void WidgetFreeTexture(Widget *widget) {
    SDL_DestroyTexture(widget->texture);
    widget->texture = NULL;
    WidgetSetFlags(widget, WIDGET_DIRTY, 0);
    return;
}
SDL_Rect WidgetAbsoluteRect(Widget *widget, SDL_Rect *rect) {
    int x = rect->x + widget->rect.x + (widget->rect.x < 0 ? rect->w : 0);
    int y = rect->y + widget->rect.y + (widget->rect.y < 0 ? rect->h : 0);
    int v = widget->rect.w + (widget->rect.w < 0 ? rect->w - x + rect->x: 0);
    int h = widget->rect.h + (widget->rect.h < 0 ? rect->h - y + rect->y: 0);
    SDL_Rect output = {x, y, v, h};
    return(output);
}
void WidgetRenderRect(SDL_Renderer *renderer, SDL_Rect *rect, SDL_Color *color, bool is_fill) {
    SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a);
    if(is_fill) {
        SDL_RenderFillRect(renderer, rect);
    } else {
        SDL_RenderDrawRect(renderer, rect);
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    return;
}
SDL_Rect WidgetPaintText(Widget *widget, SDL_Renderer *renderer, Strings *text, SDL_Rect *rect) {
    int height_font = TTF_FontHeight(widget->font);
    int position = 0;
    if(widget->type == WIDGET_EDIT || widget->type == WIDGET_LABEL) {
        position = rect->y;
    } else {
        position = rect->y + (rect->h - height_font) / 2;  // center

    }
    if(!widget->texture) {
        if(text->Compare(text, NULL)) {
            return((SDL_Rect) {
                rect->x + 2, position, 0, height_font
            });
        }
        SDL_Surface *surface = NULL;
        if(widget->type == WIDGET_EDIT) {
            surface = TTF_RenderUTF8_Blended(widget->font, text->Get(text), WidgetColorText);
        } else {
            surface = TTF_RenderUTF8_Blended_Wrapped(widget->font, text->Get(text), WidgetColorText, widget->window->rect.w);
        }
        widget->texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_SetTextureBlendMode(widget->texture, SDL_BLENDMODE_ADD);
        SDL_FreeSurface(surface);
    }
    int text_width = 0;
    int text_height = 0;
    SDL_QueryTexture(widget->texture, NULL, NULL, &text_width, &text_height);
    SDL_Rect src_rect = {0};
    if(widget->type == WIDGET_EDIT && rect->w - 2 < text_width) {
        src_rect = (SDL_Rect) {
            text_width - rect->w + 2, 0, rect->w - 2, min_int(rect->h, text_height)
        };
    } else {
        src_rect = (SDL_Rect) {
            0, 0, min_int(rect->w - 2, text_width), min_int(rect->h, text_height)
        };
    }
    SDL_Rect dst_rect = (SDL_Rect) {
        rect->x + 2, position, src_rect.w, src_rect.h
    };
    if(widget->type == WIDGET_LABEL) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_Rect cls_rect = {dst_rect.x, dst_rect.y, widget->rect.w, widget->rect.h};
        SDL_RenderFillRect(renderer, &cls_rect);
    }
    SDL_RenderCopy(renderer, widget->texture, &src_rect, &dst_rect);
    return(dst_rect);
}
void WidgetPaintButton(Widget *widget, SDL_Renderer *renderer, SDL_Rect *rect) {
    if(widget->type != WIDGET_BUTTON && widget->type != WIDGET_TOGGLE) {
        printf((char*)"Please check type(%s)\n", __func__);
        return;
    }
    SDL_Color *color = (widget->flags & WIDGET_PRESSED) ? &WidgetColorGraydeep : &WidgetColorGraylight;
    WidgetRenderRect(renderer, rect, color, true);
    switch(widget->type) {
    case(WIDGET_BUTTON): {
        WidgetPaintText(widget, renderer, widget->detail.button.caption, rect);
        break;
    }
    case(WIDGET_TOGGLE): {
        WidgetPaintText(widget, renderer, widget->detail.toggle.caption, rect);
        SDL_Rect box = {rect->x + rect->w - 52, rect->y + (rect->h - 32) / 2, 32, 32};
        WidgetRenderRect(renderer, &box, &WidgetColorText, widget->flags & WIDGET_TOGGLEON);
        break;
    }
    }
    return;
}
void WidgetPaintEdit(Widget *edit, SDL_Renderer *renderer, SDL_Rect *rect) {
    if(edit->type != WIDGET_EDIT) {
        printf((char*)"Please check type(%s)\n", __func__);
        return;
    }
    WidgetRenderRect(renderer, rect, &WidgetColorBackground, true);
    SDL_Rect rect_temp = WidgetPaintText(edit, renderer, edit->detail.edit.text, rect);
    rect_temp = (SDL_Rect) {
        rect_temp.x + rect_temp.w, rect_temp.y, 1, rect_temp.h
    };
    if((edit->flags & WIDGET_ARMED) && rect_temp.x < rect->x + rect->w) {
        WidgetRenderRect(renderer, &rect_temp, &WidgetColorText, false);
    }
    WidgetRenderRect(renderer, rect, &WidgetColorGraylight, false);
    return;
}
void WidgetPaintControl(Widget *widget, SDL_Renderer *renderer, SDL_Rect *rect) {
    if(widget->type != WIDGET_CONTROL) {
        printf((char*)"Please check type(%s)\n", __func__);
        return;
    }
    WidgetRenderRect(renderer, rect, &WidgetColorBackground, true);
    double fill_value = (widget->detail.control.value - widget->detail.control.value_min) / (widget->detail.control.value_max - widget->detail.control.value_min);
    SDL_Rect rect_temp = {rect->x, rect->y, (int)round(rect->w * fill_value), rect->h};
    WidgetRenderRect(renderer, &rect_temp, &WidgetColorGraydeep, true);
    Strings *buf_str = new_Strings(NULL);
    char *text_value = (widget->flags & WIDGET_CONTROLINT) ? (char*)"%.f" : (char*)"%.2f";
    if(!widget->texture) {
        buf_str->Set(buf_str, text_value, widget->detail.control.value);
    }
    WidgetPaintText(widget, renderer, buf_str, rect);
    delete_Strings(&buf_str);
    WidgetRenderRect(renderer, rect, &WidgetColorGraylight, false);
    return;
}
void WidgetCreateListtexture(Widget *lists, SDL_Renderer *renderer, SDL_Rect *rect) {
    if(lists->type != WIDGET_LISTS) {
        printf((char*)"Please check type(%s)\n", __func__);
        return;
    }
    int texture_max = 8192;
    int height_font = TTF_FontHeight(lists->font);
    SDL_Surface *surface_temp = TTF_RenderUTF8_Blended(lists->font, (char*)" ", WidgetColorText);
    SDL_Surface *surface = SDL_CreateRGBSurface(0, rect->w, height_font * lists->detail.lists.arrays->GetColumns(lists->detail.lists.arrays), 32, surface_temp->format->Rmask, surface_temp->format->Gmask, surface_temp->format->Bmask, surface_temp->format->Amask);
    SDL_FillRect(surface, NULL, SDL_MapRGBA(surface_temp->format, 0, 0, 0, 255));
    SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_ADD);
    SDL_FreeSurface(surface_temp);
    for(int column = 0; column < lists->detail.lists.arrays->GetColumns(lists->detail.lists.arrays); ++column) {
        Strings *str = new_Strings(lists->detail.lists.arrays->Get(lists->detail.lists.arrays)[column]->Get(lists->detail.lists.arrays->Get(lists->detail.lists.arrays)[column]));
        if(str->Compare(str, NULL)) {
            str->Set(str, (char*)" ");
        }
        SDL_Surface *surface_str = TTF_RenderUTF8_Blended(lists->font, str->Get(str), WidgetColorText);
        delete_Strings(&str);
        SDL_Rect rect_str = {2, column * height_font, surface_str->w, surface_str->h};
        SDL_BlitSurface(surface_str, NULL, surface, &rect_str);
        SDL_FreeSurface(surface_str);
    }
    int height_max = height_font * lists->detail.lists.arrays->GetColumns(lists->detail.lists.arrays);
    if(height_max > texture_max) {
        //SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, NULL, "texture dimensions are limited to 8192x8192\n", sdl_window);
        height_max = texture_max;
        lists->detail.lists.surface = surface;
    } else {
        SDL_FreeSurface(lists->detail.lists.surface);
        lists->detail.lists.surface = NULL;
    }
    SDL_Rect rect_texture = {0, 0, surface->w, height_max};
    SDL_Surface *surface_texture = SDL_CreateRGBSurface(0, rect_texture.w, rect_texture.h, 32, surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, surface->format->Amask);
    SDL_FillRect(surface_texture, NULL, SDL_MapRGBA(surface->format, 0, 0, 0, 255));
    SDL_SetSurfaceBlendMode(surface_texture, SDL_BLENDMODE_ADD);
    SDL_BlitSurface(surface, &rect_texture, surface_texture, NULL);
    lists->texture = SDL_CreateTextureFromSurface(renderer, surface_texture);
    if(!(lists->texture)) {
        printf((char*)"CreateTextureFromSurface Error(%s)\nError: %s\n", __func__, SDL_GetError());
    }
    SDL_FreeSurface(surface_texture);
    //SDL_FreeSurface(surface);
    return;
}
void WidgetPaintList_area(Widget *lists, SDL_Renderer *renderer, SDL_Rect *rect) {
    int height_font = TTF_FontHeight(lists->font);
    SDL_Rect rect_selected = {rect->x, rect->y - lists->detail.lists.scroll + height_font * lists->detail.lists.selected, rect->w, height_font};
    SDL_Rect rect_result = {0};
    if(SDL_IntersectRect(&rect_selected, rect, &rect_result)) {  // when check scroll over
        WidgetRenderRect(renderer, &rect_result, &WidgetColorGraydeep, true);
    }
    if(!(lists->texture)) {
        WidgetCreateListtexture(lists, renderer, rect);
    }
    int texture_height = height_font * lists->detail.lists.arrays->GetColumns(lists->detail.lists.arrays);
    SDL_Rect rect_from = (SDL_Rect) {
        0, lists->detail.lists.scroll, rect->w, min_int(rect->h, texture_height)
    };
    SDL_Rect rect_to = (SDL_Rect) {
        rect->x, rect->y, rect_from.w, rect_from.h
    };
    if(lists->detail.lists.surface) {
        SDL_Surface *surface = lists->detail.lists.surface;
        SDL_Surface *surface_texture = SDL_CreateRGBSurface(0, rect_from.w, rect_from.h, 32, surface->format->Rmask, surface->format->Gmask, surface->format->Bmask, surface->format->Amask);
        SDL_FillRect(surface_texture, NULL, SDL_MapRGBA(surface->format, 0, 0, 0, 255));
        SDL_SetSurfaceBlendMode(surface_texture, SDL_BLENDMODE_ADD);
        SDL_BlitSurface(surface, &rect_from, surface_texture, NULL);
        SDL_DestroyTexture(lists->texture);
        lists->texture = SDL_CreateTextureFromSurface(renderer, surface_texture);
        SDL_FreeSurface(surface_texture);
        SDL_RenderCopyEx(renderer, lists->texture, NULL, &rect_to, 0, NULL, SDL_FLIP_NONE);  // render texture
    } else {
        SDL_RenderCopyEx(renderer, lists->texture, &rect_from, &rect_to, 0, NULL, SDL_FLIP_NONE);  // render texture
    }
    int bar_height = max_int(1, rect->h * rect->h / texture_height);
    if(bar_height < rect->h) {
        int bar_y = (lists->detail.lists.scroll * rect->h) / texture_height;
        SDL_Rect rect_bar = (SDL_Rect) {
            rect->x + rect->w - 2, rect->y + bar_y, 1, bar_height
        };
        WidgetRenderRect(renderer, &rect_bar, &WidgetColorText, true);  // scroll bar
    }
    return;
}
void WidgetPaintList(Widget *lists, SDL_Renderer *renderer, SDL_Rect *rect) {
    if(lists->type != WIDGET_LISTS) {
        printf((char*)"Please check type(%s)\n", __func__);
        return;
    }
    if(!(lists->detail.lists.arrays)) {
        printf((char*)"There is no array(%s)\n", __func__);
        return;
    }
    WidgetRenderRect(renderer, rect, &WidgetColorBackground, true);
    WidgetPaintList_area(lists, renderer, rect);
    WidgetRenderRect(renderer, rect, &WidgetColorGraylight, false);
    return;
}
void WidgetPaintPainter(Widget *widget, SDL_Renderer *renderer, SDL_Rect *rect) {
    if(widget->type != WIDGET_PAINTER) {
        printf((char*)"Please check type(%s)\n", __func__);
        return;
    }
    SDL_Surface *sdl_surface = widget->detail.painter.surface;
    if(!sdl_surface) {
        printf((char*)"Cannot make SDL_Surface:%s\n", SDL_GetError());
        return;
    }
    SDL_Rect rect_bmp = {0};
    if(rect->w == 0 && rect->h == 0) {
        rect_bmp = (SDL_Rect) {
            rect->x, rect->y, sdl_surface->w, sdl_surface->h
        };  // original
    } else {
        rect_bmp = (SDL_Rect) {
            rect->x, rect->y, rect->w, rect->h
        };
    }
    widget->texture = SDL_CreateTextureFromSurface(renderer, sdl_surface);
    SDL_RenderCopyEx(renderer, widget->texture, NULL, &rect_bmp, 0, NULL, SDL_FLIP_NONE);
    return;
}
void WidgetPaintPanel(Widget *widget, SDL_Renderer *renderer, SDL_Rect *rect) {
    if(widget->type != WIDGET_PANEL && widget->type != WIDGET_WINDOW) {
        printf((char*)"Please check type(%s)\n", __func__);
        return;
    }
    WidgetRenderRect(renderer, rect, &WidgetColorBackground, true);
    WidgetRenderRect(renderer, rect, &WidgetColorGraydeep, false);
    for(Widget *wid = widget->child; wid; wid = wid->next) { // force children to redraw
        WidgetSetFlags(wid, WIDGET_DIRTY, 0);
    }
    return;
}
void WidgetPaintLabel(Widget *widget, SDL_Renderer *renderer, SDL_Rect *rect) {
    if(widget->type != WIDGET_LABEL) {
        printf((char*)"Please check type(%s)\n", __func__);
        return;
    }
    WidgetPaintText(widget, renderer, widget->detail.label.text, rect);
    return;
}
void WidgetEventButton(Widget *widget, SDL_Event *event, SDL_Rect *rect) {
    if(widget->type != WIDGET_BUTTON && widget->type != WIDGET_TOGGLE) {
        printf((char*)"Please check type(%s)\n", __func__);
        return;
    }
    switch(event->type) {
    case(SDL_MOUSEMOTION): {
        if((widget->flags & WIDGET_ARMED) && ((widget->flags & WIDGET_MOUSEOVER) != (widget->flags & WIDGET_PRESSED))) {
            WidgetSetFlags(widget, WIDGET_DIRTY | (~widget->flags & WIDGET_PRESSED), widget->flags & WIDGET_PRESSED);
        }
        break;
    }
    case(SDL_MOUSEBUTTONDOWN): {
        WidgetSetFlags(widget, WIDGET_DIRTY | WIDGET_ARMED | WIDGET_PRESSED, 0);
        break;
    }
    case(SDL_MOUSEBUTTONUP): {
        if((widget->flags & WIDGET_MOUSEOVER) && (widget->flags & WIDGET_ARMED)) {
            WidgetSetFlags(widget, WIDGET_CHANGED, 0);
        }
        if(widget->type == WIDGET_TOGGLE && (widget->flags & WIDGET_MOUSEOVER) && (widget->flags & WIDGET_ARMED)) {
            WidgetSetFlags(widget, ~widget->flags & WIDGET_TOGGLEON, widget->flags & WIDGET_TOGGLEON);
        }
        WidgetSetFlags(widget, WIDGET_DIRTY, WIDGET_ARMED | WIDGET_PRESSED);
        break;
    }
    }
    return;
}
void WidgetEventEdit(Widget *widget, SDL_Event *event, SDL_Rect *rect) {
    if(widget->type != WIDGET_EDIT) {
        printf((char*)"Please check type(%s)\n", __func__);
        return;
    }
    bool is_render = false;
    switch(event->type) {
    case(SDL_MOUSEBUTTONDOWN): {
        if(widget->flags & WIDGET_MOUSEOVER) {
            SDL_SetTextInputRect(rect);
            SDL_StartTextInput();
            WidgetSetFlags(widget, WIDGET_DIRTY | WIDGET_ARMED, 0);
        } else if(!(widget->flags & WIDGET_MOUSEOVER)) {
            WidgetSetFlags(widget, WIDGET_DIRTY, WIDGET_ARMED);
        }
        break;
    }
    case(SDL_KEYDOWN): {
        if(event->key.keysym.sym == SDLK_BACKSPACE) {
            if(widget->detail.edit.strings_ime->Get(widget->detail.edit.strings_ime) != NULL && strlen(widget->detail.edit.strings_direct->Get(widget->detail.edit.strings_direct)) + strlen(widget->detail.edit.strings_ime->Get(widget->detail.edit.strings_ime)) > 0) {
                if(widget->detail.edit.strings_ime->Compare(widget->detail.edit.strings_ime, NULL)) {
                    widget->detail.edit.strings_direct->Backspace(widget->detail.edit.strings_direct);
                    is_render = true;
                }
            }
        } else if((event->key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL) || event->key.keysym.sym == SDLK_LEFT) {
            SDL_SetClipboardText(widget->detail.edit.strings_direct->Get(widget->detail.edit.strings_direct));
        } else if((event->key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL) || event->key.keysym.sym == SDLK_RIGHT) {
            Strings *str = widget->detail.edit.strings_direct;
            str->Set(str, (char*)"%s%s", str->Get(str), SDL_GetClipboardText());
            is_render = true;
        } else if(event->key.keysym.sym == SDLK_RETURN || event->key.keysym.sym == SDLK_RETURN2 || event->key.keysym.sym == SDLK_KP_ENTER) {
            if(widget->detail.edit.strings_ime->Get(widget->detail.edit.strings_ime) != NULL) {
                if(widget->detail.edit.strings_ime->Compare(widget->detail.edit.strings_ime, NULL)) {
                    if(widget->event) {
                        widget->SetFlags(widget, WIDGET_CHANGED, 0);
                        widget->event(widget, event, rect);
                    }
                }
            }
        } else if(event->key.keysym.sym == SDLK_TAB) {
            Strings *str = widget->detail.edit.strings_direct;
            str->Set(str, (char*)"%s\t", str->Get(str));
            is_render = true;
        }
        break;
    }
    case(SDL_TEXTINPUT): {
        if(!((event->text.text[0] == 'c' || event->text.text[0] == 'C') && (event->text.text[0] == 'v' || event->text.text[0] == 'V') && SDL_GetModState() & KMOD_CTRL)) {
            Strings *str = widget->detail.edit.strings_direct;
            str->Set(str, (char*)"%s%s", str->Get(str), event->text.text);
            is_render = true;
        }
        widget->detail.edit.strings_ime->Get(widget->detail.edit.strings_ime)[0] = 0;
        break;
    }
    case(SDL_TEXTEDITING): {
        if(!((event->text.text[0] == 'c' || event->text.text[0] == 'C') && (event->text.text[0] == 'v' || event->text.text[0] == 'V') && SDL_GetModState() & KMOD_CTRL)) {
            Strings *str = widget->detail.edit.strings_ime;
            str->Set(str, (char*)"%s", event->text.text);
            is_render = true;
        }
        break;
    }
    }
    if(is_render) {
        Strings *str_text = widget->detail.edit.text;
        Strings *str_direct = widget->detail.edit.strings_direct;
        Strings *str_ime = widget->detail.edit.strings_ime;
        str_text->Set(str_text, (char*)"%s%s", str_direct->Get(str_direct), str_ime->Get(str_ime));
        WidgetFreeTexture(widget);
    }
    return;
}
void WidgetSetValue_from_x(Widget *widget, int x, SDL_Rect *rect) {
    double tmp = (widget->detail.control.value_max - widget->detail.control.value_min) * (x - rect->x) / rect->w + widget->detail.control.value_min;
    tmp = clamp_double(widget->detail.control.value_min, tmp, widget->detail.control.value_max);
    tmp = (widget->flags & WIDGET_CONTROLINT) ? roundf(tmp) : tmp;
    tmp = tmp < 0.f ? tmp : fabsf(tmp); // get rid of -0
    if (tmp != widget->detail.control.value) {
        widget->detail.control.value = tmp;
        WidgetFreeTexture(widget);
        WidgetSetFlags(widget, WIDGET_CHANGED, 0);
    }
    return;
}
void WidgetEventControl(Widget *widget, SDL_Event *event, SDL_Rect *rect) {
    if(widget->type != WIDGET_CONTROL) {
        printf((char*)"Please check type(%s)\n", __func__);
        return;
    }
    switch(event->type) {
    case(SDL_MOUSEBUTTONDOWN): {
        WidgetSetValue_from_x(widget, event->button.x, rect);
        WidgetSetFlags(widget, WIDGET_ARMED, 0);
        break;
    }
    case(SDL_MOUSEBUTTONUP): {
        WidgetSetFlags(widget, 0, WIDGET_ARMED);
        break;
    }
    case(SDL_MOUSEMOTION): {
        if((widget->flags & WIDGET_ARMED)) {
            WidgetSetValue_from_x(widget, event->motion.x, rect);
        }
        break;
    }
    }
    return;
}
void WidgetEventList(Widget *widget, SDL_Event *event, SDL_Rect *rect) {
    if(widget->type != WIDGET_LISTS) {
        printf((char*)"Please check type(%s)\n", __func__);
        return;
    }
    switch(event->type) {
    case(SDL_MOUSEBUTTONDOWN): {
        if((widget->flags & WIDGET_MOUSEOVER)) {
            SDL_SetTextInputRect((SDL_Rect*)rect); // cast, probably an api bug
            SDL_StartTextInput();
            WidgetSetFlags(widget, WIDGET_DIRTY | WIDGET_ARMED, 0);
            int tmp = 0;
            tmp = (widget->detail.lists.scroll + event->button.y - rect->y) / TTF_FontHeight(widget->font);  // calculate which item would be selected
            if(tmp >= 0 && tmp < widget->detail.lists.arrays->GetColumns(widget->detail.lists.arrays)) {
                widget->detail.lists.selected = tmp;
                WidgetSetFlags(widget, WIDGET_DIRTY | WIDGET_CHANGED, 0);
            }
            tmp = max_int(widget->detail.lists.arrays->GetColumns(widget->detail.lists.arrays) * TTF_FontHeight(widget->font) - rect->h, 0);  // calculate max possible scroll
            //tmp = min_int(tmp, texture_max - rect->h);
            tmp = clamp_int(0, widget->detail.lists.scroll + event->button.y - rect->y - rect->h / 2, tmp); // apply scroll
            if(tmp != widget->detail.lists.scroll) {
                widget->detail.lists.scroll = tmp;
                WidgetSetFlags(widget, WIDGET_DIRTY, 0);
            }
        } else if(!(widget->flags & WIDGET_MOUSEOVER) && (widget->flags & WIDGET_ARMED)) {
            WidgetSetFlags(widget, WIDGET_DIRTY, WIDGET_ARMED);
        }
        break;
    }
    case(SDL_MOUSEWHEEL): {
        if(widget->detail.lists.surface) {
            break;
        }
        int tmp = 0;
        tmp = max_int(widget->detail.lists.arrays->GetColumns(widget->detail.lists.arrays) * TTF_FontHeight(widget->font) - rect->h, 0);  // calculate max possible scroll
        //tmp = min_int(tmp, texture_max - rect->h);
        tmp = clamp_int(0, widget->detail.lists.scroll - event->wheel.y * 3, tmp); // apply scroll
        if(tmp != widget->detail.lists.scroll) {
            widget->detail.lists.scroll = tmp;
            WidgetSetFlags(widget, WIDGET_DIRTY, 0);
        }
        break;
    }
    case(SDL_KEYDOWN): {
        if(event->key.keysym.sym == SDLK_KP_8 || event->key.keysym.sym == SDLK_2 || event->key.keysym.sym == SDLK_UP) {
            int tmp = 0;
            if(widget->detail.lists.selected >= 1) {
                widget->detail.lists.selected -= 1;
                WidgetSetFlags(widget, WIDGET_DIRTY | WIDGET_CHANGED, 0);
            }
            tmp = max_int(widget->detail.lists.arrays->GetColumns(widget->detail.lists.arrays) * TTF_FontHeight(widget->font) - rect->h, 0);  // calculate max possible scroll
            //tmp = min_int(tmp, texture_max - rect->h);
            tmp = clamp_int(0, widget->detail.lists.scroll - TTF_FontHeight(widget->font), tmp); // apply scroll
            if(tmp != widget->detail.lists.scroll) {
                widget->detail.lists.scroll = tmp;
                WidgetSetFlags(widget, WIDGET_DIRTY, 0);
            }
        } else if(event->key.keysym.sym == SDLK_KP_2 || event->key.keysym.sym ==  SDLK_8 || event->key.keysym.sym == SDLK_DOWN) {
            int tmp = 0;
            if(widget->detail.lists.selected <= widget->detail.lists.arrays->GetColumns(widget->detail.lists.arrays) - 2) {
                widget->detail.lists.selected += 1;
                WidgetSetFlags(widget, WIDGET_DIRTY | WIDGET_CHANGED, 0);
            }
            tmp = max_int(widget->detail.lists.arrays->GetColumns(widget->detail.lists.arrays) * TTF_FontHeight(widget->font) - rect->h, 0);  // calculate max possible scroll
            //tmp = min_int(tmp, texture_max - rect->h);
            tmp = clamp_int(0, widget->detail.lists.scroll + TTF_FontHeight(widget->font), tmp); // apply scroll
            if(tmp != widget->detail.lists.scroll) {
                widget->detail.lists.scroll = tmp;
                WidgetSetFlags(widget, WIDGET_DIRTY, 0);
            }
        } else if(event->key.keysym.sym == SDLK_RETURN || event->key.keysym.sym == SDLK_RETURN2 || event->key.keysym.sym == SDLK_KP_ENTER) {
            WidgetSetFlags(widget, WIDGET_CHANGED, 0);
        }
        break;
    }
    }
    return;
}
bool WidgetEventExit(Widget *widget, SDL_Event *event, SDL_Rect *rect) {
    if(!(widget->Changed(widget))) {
        return(false);
    }
    SDL_Event Eventquit = {0};
    Eventquit.type = SDL_QUIT;
    SDL_PushEvent(&Eventquit);
    return(true);
}
void WidgetEventWindow(Widget *widget, SDL_Event *event) {
    Widget *active_window = {0};
    for(active_window = widget; active_window && event->window.windowID != SDL_GetWindowID(active_window->detail.window.gui->sdl_window); active_window = active_window->next) {}
    if(!active_window) {
        return;
    }
    switch(event->window.event) {
    case(SDL_WINDOWEVENT_RESIZED): {
        active_window->rect.w = event->window.data1;
        active_window->rect.h = event->window.data2;
    }
    case(SDL_WINDOWEVENT_EXPOSED): {
        WidgetSetFlags(active_window, WIDGET_DIRTY, 0);
    }
    }
    if(active_window->event) {
        active_window->event(active_window, event, &active_window->rect);
    }
    return;
}
void WidgetRemove(Widget **first, Widget *widget) {
    for(; *first && *first != widget; first = &(*first)->next);
    *first = (*first)->next;
    return;
}
void WidgetFreeWindow(Widget *widget) {
    delete_Guibasic(&(widget->detail.window.gui));
    WidgetRemove(&widget, widget);
    return;
}
void WidgetFreeText(Widget *widget) {
    delete_Strings(&(widget->detail.button.caption));
    delete_Strings(&(widget->detail.toggle.caption));
    delete_Strings(&(widget->detail.edit.strings_direct));
    delete_Strings(&(widget->detail.edit.strings_ime));
    delete_Strings(&(widget->detail.edit.text));
    delete_Strings(&(widget->detail.label.text));
    return;
}
void WidgetFreeLists(Widget *widget) {
    delete_ArrayS(&(widget->detail.lists.arrays));
    SDL_FreeSurface(widget->detail.lists.surface);
}
void WidgetFreePainter(Widget *widget) {
    SDL_FreeSurface(widget->detail.painter.surface);
}
WidgetCalls WidgetCallTable[9] = {
    {WidgetEventButton, WidgetPaintButton, WidgetFreeText},  // button
    {WidgetEventButton, WidgetPaintButton, WidgetFreeText},  // toggle
    {WidgetEventEdit, WidgetPaintEdit, WidgetFreeText},  // edit
    {NULL, WidgetPaintLabel, WidgetFreeText},  // label
    {WidgetEventList, WidgetPaintList, WidgetFreeLists},  // lists
    {NULL, WidgetPaintPainter, WidgetFreePainter},  // painter
    {WidgetEventControl, WidgetPaintControl, NULL},  // control
    {NULL, WidgetPaintPanel, NULL},  // panel
    {NULL, WidgetPaintPanel, WidgetFreeWindow}  // window
};
bool WidgetPaint(Widget *widget, SDL_Renderer *renderer, SDL_Rect *rect) {
    bool changed = false;
    if(widget->flags & WIDGET_DIRTY) {
        SDL_Rect rect_abs = WidgetAbsoluteRect(widget, rect);
        WidgetCallTable[widget->type].paint(widget, renderer, &rect_abs);
        WidgetSetFlags(widget, 0, WIDGET_DIRTY);
        changed = true;
    }
    if(WidgetHasChild(widget)) {
        SDL_Rect rect_abs = WidgetAbsoluteRect(widget, rect);
        for(widget = widget->child; widget; widget = widget->next) {
            changed |= WidgetPaint(widget, renderer, &rect_abs);
        }
    }
    return(changed);
}
void WidgetPaintAll(Widget *window) {
    if(!window) {
        return;
    }
    WidgetPaint(window, window->detail.window.gui->sdl_renderer, &(window->rect));
    window->detail.window.gui->RenderPresent(window->detail.window.gui);
    return;
}
bool WidgetEvent(Widget *widget, SDL_Event *event, SDL_Rect *rect) {
    if(!widget || !(widget->enable)) {
        return(false);
    }
    SDL_Rect rect_abs = WidgetAbsoluteRect(widget, rect);
    int over = WidgetIsMouse_over_rect(event, &rect_abs) ? -1 : 0;
    WidgetSetFlags(widget, WIDGET_MOUSEOVER & over, WIDGET_MOUSEOVER & (-1 - over));
    if(over || (widget->flags & WIDGET_ARMED)) {
        if(WidgetCallTable[widget->type].event) {
            WidgetCallTable[widget->type].event(widget, event, &rect_abs);
        }
        if(widget->event && widget->event(widget, event, &rect_abs)) {
            return(true);
        }
    }
    if(WidgetHasChild(widget)) {
        for(widget = widget->child; widget; widget = widget->next) {
            if(WidgetEvent(widget, event, &rect_abs)) {
                return(true);
            }
        }
    }
    return(false);
}
void WidgetEventAll(Widget *window, SDL_Event *event) {
    if(!window) {
        return;
    }
    if(event->type == SDL_WINDOWEVENT) {
        WidgetEventWindow(window->window, event);
    } else {
        WidgetEvent(window, event, &(window->rect));
    }
    return;
}
void WidgetFree(Widget **widget) {
    if(!(*widget)) {
        return;
    }
    if(WidgetCallTable[(*widget)->type].free) {
        WidgetCallTable[(*widget)->type].free(*widget);
    }
    SDL_DestroyTexture((*widget)->texture);
    free(*widget);
    *widget = NULL;
    return;
}
void WidgetFreeAll(Widget **widget) {
    if(!(*widget)) {
        return;
    }
    if(WidgetHasChild(*widget)) {
        Widget *i = (*widget)->child;
        for(Widget *tmp = NULL; i; i = tmp) {
            tmp = i->next;
            WidgetFreeAll(&i);
        }
    }
    WidgetFree(widget);
    *widget = NULL;
    return;
}
void WidgetEnable(Widget *widget) {
    if(!widget) {
        return;
    }
    widget->enable = true;
    return;
}
void WidgetEnableAll(Widget *window) {
    if(!window) {
        return;
    }
    if(WidgetHasChild(window)) {
        Widget *i = window->child;
        for(Widget *tmp = NULL; i; i = tmp) {
            tmp = i->next;
            WidgetEnableAll(i);
        }
    }
    if(window == window->window) {
        return;
    }
    WidgetEnable(window);
    window->SetFlags(window, WIDGET_DIRTY, 0);
    return;
}
void WidgetDisenable(Widget *widget) {
    if(!widget) {
        return;
    }
    widget->enable = false;
    return;
}
void WidgetDisenableAll(Widget *window) {
    if(!window) {
        return;
    }
    if(WidgetHasChild(window)) {
        Widget *i = window->child;
        for(Widget *tmp = NULL; i; i = tmp) {
            tmp = i->next;
            WidgetDisenableAll(i);
        }
    }
    if(window == window->window) {
        return;
    }
    WidgetDisenable(window);
    window->SetFlags(window, WIDGET_DIRTY, 0);
    return;
}
void WidgetSetEvent(Widget *widget, bool (*event)(Widget *widget, SDL_Event *event, SDL_Rect *rect), void *payload) {
    widget->event = event;
    widget->payload = payload;
    return;
}
void WidgetSetText(Widget *widget, char *format, ...) {
    if(!format) {
        return;
    }
    va_list arg1;
    va_list arg2;
    va_start(arg1, format);
    va_copy(arg2, arg1);
    int length = vsnprintf(NULL, 0, format, arg1) + 1;
    char *buffer = (char*)malloc(sizeof(char) * length);
    if(!buffer) {
        printf((char*)"malloc error(StringsSet)\n");
    }
    vsnprintf(buffer, length, format, arg2);
    va_end(arg1);
    va_end(arg2);
    switch(widget->type) {
    case(WIDGET_BUTTON): {
        widget->detail.button.caption->Set(widget->detail.button.caption, (char*)"%s", buffer);
        WidgetFreeTexture(widget);
        break;
    }
    case(WIDGET_TOGGLE): {
        widget->detail.toggle.caption->Set(widget->detail.toggle.caption, (char*)"%s", buffer);
        WidgetFreeTexture(widget);
        break;
    }
    case(WIDGET_EDIT): {
        Strings *str_text = widget->detail.edit.text;
        Strings *str_direct = widget->detail.edit.strings_direct;
        Strings *str_ime = widget->detail.edit.strings_ime;
        str_text->Set(str_text, (char*)"%s", buffer);
        str_direct->Set(str_direct, (char*)"%s", buffer);
        str_ime->Set(str_ime, NULL);
        WidgetFreeTexture(widget);
        break;
    }
    case(WIDGET_LABEL): {
        widget->detail.label.text->Set(widget->detail.label.text, (char*)"%s", buffer);
        WidgetFreeTexture(widget);
        break;
    }
    case(WIDGET_WINDOW): {
        SDL_SetWindowTitle(widget->detail.window.gui->sdl_window, buffer);
        break;
    }
    default: {
        printf((char*)"Please check type(%s)\n", __func__);
    }
    }
    free(buffer);
    return;
}
Strings *WidgetGetEdit(Widget *edit) {
    Strings *output = edit->detail.edit.text;
    return(output);
}
void WidgetSetRange(Widget *control, double value_min, double value_max, bool integers) {
    control->detail.control.value_min = value_min;
    control->detail.control.value_max = value_max;
    if (integers)
        WidgetSetFlags(control, WIDGET_CONTROLINT, 0);
    WidgetSetFlags(control, WIDGET_DIRTY, 0);
    return;
}
void WidgetSetValue(Widget *control, double value) {
    control->detail.control.value = clamp_double(control->detail.control.value_min, value, control->detail.control.value_max);
    return;
}
double WidgetGetValue(Widget *widget) {
    double output = widget->detail.control.value;
    return(output);
}
int WidgetGetSelected(Widget *lists) {
    int output = lists->detail.lists.selected;
    return(output);
}
void WidgetSetList(Widget *lists, ArrayS *arr) {
    lists->detail.lists.arrays->Copy(lists->detail.lists.arrays, arr);
    lists->detail.lists.scroll = 0;
    if(lists->detail.lists.selected >= arr->GetColumns(arr)) {
        lists->detail.lists.selected = arr->GetColumns(arr) - 1;
    }
    WidgetFreeTexture(lists);
    return;
}
void WidgetSetPainter(Widget *painter, SDL_Surface *surface) {
    SDL_Surface *surface_new = SDL_CreateRGBSurface(0, surface->w, surface->h, 32, 0, 0, 0, 0);
    SDL_BlitSurface(surface, NULL, surface_new, NULL);
    painter->detail.painter.surface = surface_new;
    return;
}
bool WidgetChanged(Widget *widget) {
    bool changed = widget->flags & WIDGET_CHANGED;
    WidgetSetFlags(widget, 0, WIDGET_CHANGED);
    return(changed);
}
void WidgetLoop(Widget *window) {
    SDL_Event event = {0};
    while(event.type != SDL_QUIT) {
        SDL_WaitEvent(&event);
        window->detail.window.EventAll(window, &event);
        window->detail.window.PaintAll(window);
    }
    return;
}
void WidgetAdd(Widget **first, Widget *widget) {
    while(*first) {
        first = &(*first)->next;
    }
    *first = widget;
    return;
}
Widget *new_Widget(int type, int x, int y, int width, int height, Widget *parent) {
    Widget *widget = (Widget*)malloc(sizeof(Widget));
    *widget = (Widget) {
        0
    };
    widget->type = type;
    widget->enable = true;
    widget->Changed = WidgetChanged;
    widget->SetText = WidgetSetText;
    widget->SetEvent = WidgetSetEvent;
    widget->SetFlags = WidgetSetFlags;
    if(type == WIDGET_WINDOW) {
        Guibasic *gui_new = new_Guibasic(width, height);
        widget->rect = (SDL_Rect) {
            0, 0, width, height
        };
        widget->window = widget;
        widget->font = gui_new->ttf_font;
        widget->detail.window.gui = gui_new;
        widget->detail.window.EventAll = WidgetEventAll;
        widget->detail.window.PaintAll = WidgetPaintAll;
        widget->detail.window.EnableAll = WidgetEnableAll;
        widget->detail.window.DisenableAll = WidgetDisenableAll;
        widget->detail.window.Loop = WidgetLoop;
        WidgetSetFlags(widget, WIDGET_DIRTY, 0);
    } else {
        widget->rect = (SDL_Rect) {
            x, y, width, height
        };
        switch(type) {
        case(WIDGET_BUTTON): {
            widget->detail.button.caption = new_Strings(NULL);
            break;
        }
        case(WIDGET_TOGGLE): {
            widget->detail.toggle.caption = new_Strings(NULL);
            break;
        }
        case(WIDGET_LABEL): {
            widget->detail.label.text = new_Strings(NULL);
            break;
        }
        case(WIDGET_LISTS): {
            widget->detail.lists.arrays = new_ArrayS(0, 0, NULL, NULL);
            widget->detail.lists.selected = 0;
            widget->detail.lists.scroll = 0;
            widget->detail.lists.surface = NULL;
            widget->detail.lists.GetSelected = WidgetGetSelected;
            widget->detail.lists.SetList = WidgetSetList;
            break;
        }
        case(WIDGET_PAINTER): {
            widget->detail.painter.SetPainter = WidgetSetPainter;
            break;
        }
        case(WIDGET_EDIT): {
            widget->detail.edit.strings_direct = new_Strings(NULL);
            widget->detail.edit.strings_ime = new_Strings(NULL);
            widget->detail.edit.text = new_Strings(NULL);
            widget->detail.edit.GetEdit = WidgetGetEdit;
            break;
        }
        case(WIDGET_CONTROL): {
            widget->detail.control.SetRange = WidgetSetRange;
            widget->detail.control.SetValue = WidgetSetValue;
            break;
        }
        }
        widget->window = parent;
        while(widget->window != widget->window->window) {
            widget->window = widget->window->window;
        }
        widget->font = widget->window->detail.window.gui->ttf_font;
        WidgetAdd(&(parent->child), widget);
        WidgetSetFlags(widget, WIDGET_DIRTY, 0);
    }
    return(widget);
}
void delete_Widget(Widget **window) {
    if(*window) {
        if((*window)->window != *window) {
            WidgetRemove(&((*window)->window->child), *window);
        }
        WidgetSetFlags((*window)->window, WIDGET_DIRTY, 0);
        WidgetFreeAll(window);
    }
    *window = NULL;
    return;
}
