//#include "SDL2/SDL.h"
#include "../../include/widget.h"
bool EventButton(Widget *widget, SDL_Event *event, SDL_Rect *rect) {
    if(!(widget->Changed(widget))) {
        return(false);
    }
    widget->SetText(widget, (char*)"Button_done");
    return(true);
}
bool EventPrint(Widget *widget, SDL_Event *event, SDL_Rect *rect) {
    if(!(widget->Changed(widget))) {
        return(false);
    }
    Guibasic *guibasic = widget->window->detail.window.gui;
    widget->window->detail.window.DisenableAll(widget->window);
    guibasic->Printf(guibasic, "%s", "日本語テスト\n");
    guibasic->CharGetting(guibasic);
    widget->window->detail.window.EnableAll(widget->window);
    widget->window->SetFlags(widget->window, WIDGET_DIRTY, 0);
    widget->SetText(widget, (char*)"Button_done");
    return(true);
}
bool Run(void) {
    Widget *window = new_Widget(WIDGET_WINDOW, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1080, 700, NULL);  // Window
    window->SetText(window, (char*)"ウィンドウタイトル　テスト");
    Widget *label = new_Widget(WIDGET_LABEL, 700, 10, 350, 200, window);  // Label
    label->SetText(label, (char*)"Label\ntest\n123\n日本語テストあいうえお");
    Widget *button = new_Widget(WIDGET_BUTTON, 10, 10, 300, 50, window);  // Button
    button->SetText(button, (char*)"Button");
    button->SetEvent(button, EventButton, NULL);
    Widget *toggle = new_Widget(WIDGET_TOGGLE, 10, 70, 300, 50, window);  // Toggle
    toggle->SetText(toggle, (char*)"Toggle");
    Widget *control = new_Widget(WIDGET_CONTROL, 10, 130, 300, 50, window);  // Control
    control->detail.control.SetRange(control, -6, 24, false);
    control->detail.control.SetValue(control, 12);
    Widget *edit = new_Widget(WIDGET_EDIT, 10, 190, 300, 50, window);  // Edit
    edit->SetText(edit, (char*)"Edit");
    Widget *painter = new_Widget(WIDGET_PAINTER, 700, 250, 100, 200, window);  // Painter
    SDL_Surface *surface = IMG_Load((char*)"../../res/lena.bmp");
    painter->detail.painter.SetPainter(painter, surface);
    SDL_FreeSurface(surface);
    char *list_char[] = {(char*)"List item 0", (char*)"List item 1", (char*)"List item 2", (char*)"List item 3", (char*)"List item 4", (char*)"List item 5", (char*)"List item 6", (char*)"List item 7", (char*)"List item 8"};
    ArrayS *list_data = new_ArrayS(1, sizeof(list_char) / sizeof(list_char[0]), list_char, NULL);
    Widget *list = new_Widget(WIDGET_LISTS, 320, 10, 300, 230, window);  // List
    list->detail.lists.SetList(list, list_data);
    delete_ArrayS(&list_data);
    Widget *panel = new_Widget(WIDGET_PANEL, 10, 250, 600, 120, window);  // Panel
    Widget *file_label = new_Widget(WIDGET_LABEL, 0, 0, 550, 60, panel);
    file_label->SetText(file_label, (char*)"Path");
    Widget *button_open = new_Widget(WIDGET_BUTTON, -200, 5, -10, 50, panel);
    button_open->SetText(button_open, (char*)"Print");
    button_open->SetEvent(button_open, EventPrint, file_label);
    Widget *test_version = new_Widget(WIDGET_LABEL, -300, -50, 300, 50, window);  // When negative
    test_version->SetText(test_version, (char*)"Test Version");
    Widget *button_exit = new_Widget(WIDGET_BUTTON, 0, -300, 300, 50, window);  // Exit
    button_exit->SetText(button_exit, (char*)"Exit");
    button_exit->SetEvent(button_exit, WidgetEventExit, NULL);
    window->detail.window.Loop(window);
    delete_Widget(&window);
    return(true);
}
