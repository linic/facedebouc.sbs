// port_dialog.h
#ifndef PORT_DIALOG_H
#define PORT_DIALOG_H

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Int_Input.H>
#include <FL/Fl_Button.H>

class PortDialog : public Fl_Window {
private:
    Fl_Int_Input* port_input;
    Fl_Button* apply_button;
    Fl_Button* cancel_button;
    int* port_value;

    static void apply_cb(Fl_Widget* w, void* v);
    static void cancel_cb(Fl_Widget* w, void* v);

public:
    PortDialog(int* port, const char* title);
};

#endif // PORT_DIALOG_H

