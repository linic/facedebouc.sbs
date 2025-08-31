// port_dialog.cpp
#include "port_dialog.h"
#include <string>

PortDialog::PortDialog(int* port, const char* title) : Fl_Window(300, 100, title) {
    port_value = port;
    port_input = new Fl_Int_Input(100, 10, 100, 25, "Port:");
    port_input->value(std::to_string(*port_value).c_str());

    apply_button = new Fl_Button(70, 50, 70, 25, "Apply");
    apply_button->callback(apply_cb, this);

    cancel_button = new Fl_Button(160, 50, 70, 25, "Cancel");
    cancel_button->callback(cancel_cb, this);

    end();
}

void PortDialog::apply_cb(Fl_Widget* w, void* v) {
    PortDialog* dialog = (PortDialog*)v;
    *(dialog->port_value) = std::stoi(dialog->port_input->value());
    dialog->hide();
}

void PortDialog::cancel_cb(Fl_Widget* w, void* v) {
    ((PortDialog*)v)->hide();
}

