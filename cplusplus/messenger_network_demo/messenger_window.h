// messenger_window.h
#ifndef MESSENGER_WINDOW_H
#define MESSENGER_WINDOW_H

#include <thread>
#include <atomic>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Multiline_Output.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Menu_Bar.H>
#include "state_machine.h"

struct OutputMessage {
    std::vector<char> text;
    char prefix;
};

class MessengerWindow : public Fl_Window {
private:
    Fl_Menu_Bar* menu;
    Fl_Multiline_Output* output;
    Fl_Input* input;
    Fl_Button* send_button;
    StateMachine* state_machine;
    std::thread output_thread;
    std::atomic<bool> running;

    static void send_cb(Fl_Widget* widget, void* data);
    static void set_receiving_port_cb(Fl_Widget* w, void* v);
    static void set_sending_port_cb(Fl_Widget* w, void* v);
    static void set_destination_ip_cb(Fl_Widget* w, void* v);
    static void update_output_cb(void* data);
    void output_loop();

public:
    MessengerWindow(int w, int h, const char* title, StateMachine* sm);
    ~MessengerWindow();

    void run();
};

#endif // MESSENGER_WINDOW_H

