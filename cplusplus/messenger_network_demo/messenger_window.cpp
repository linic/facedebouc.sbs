// messenger_window.cpp
#include "messenger_window.h"
#include "port_dialog.h"
#include "configuration.h"
#include <FL/fl_ask.H>
#include <FL/Fl.H>
#include <iostream>

extern Configuration config;

MessengerWindow::MessengerWindow(int w, int h, const char* title, StateMachine* sm)
    : Fl_Window(w, h, title), state_machine(sm), running(true) {
    
    menu = new Fl_Menu_Bar(0, 0, w, 25);
    menu->add("Settings/Set Receiving Port", 0, set_receiving_port_cb, this);
    menu->add("Settings/Set Sending Port", 0, set_sending_port_cb, this);
    menu->add("Settings/Set Destination IP", 0, set_destination_ip_cb, this);

    output = new Fl_Multiline_Output(10, 35, w - 20, h - 105);
    output->textfont(FL_COURIER);
    
    input = new Fl_Input(10, h - 60, w - 90, 30, "");

    send_button = new Fl_Button(w - 70, h - 60, 60, 30, "Send");
    send_button->callback(send_cb, this);
    
    end();

    output_thread = std::thread(&MessengerWindow::output_loop, this);
}

MessengerWindow::~MessengerWindow() {
    running = false;
    if (output_thread.joinable()) {
        output_thread.join();
    }
}

void MessengerWindow::run() {
    show();
    Fl::run();
}

void MessengerWindow::send_cb(Fl_Widget* widget, void* data) {
    MessengerWindow* window = static_cast<MessengerWindow*>(data);
    const char* text = window->input->value();
    
    std::vector<char> text_message(text, text + strlen(text));
    OutputMessage* msg = new OutputMessage{text_message, '<'};
    if(Fl::awake(update_output_cb, msg)) {
		std::cerr << "registration failed!" << std::endl;
	}

    if (window->state_machine) {
        window->state_machine->pushInputMessage(text_message);
    }
    
    window->input->value("");
}

void MessengerWindow::set_receiving_port_cb(Fl_Widget* w, void* v) {
    PortDialog* dialog = new PortDialog(&config.receiving_port, "Set Receiving Port");
    dialog->set_modal();
    dialog->show();
    while (dialog->shown()) Fl::wait();
    delete dialog;
    config.save();
}

void MessengerWindow::set_sending_port_cb(Fl_Widget* w, void* v) {
    PortDialog* dialog = new PortDialog(&config.sending_port, "Set Sending Port");
    dialog->set_modal();
    dialog->show();
    while (dialog->shown()) Fl::wait();
    delete dialog;
    config.save();
}

void MessengerWindow::set_destination_ip_cb(Fl_Widget* w, void* v) {
    const char* ip = fl_input("Enter destination IP:", config.destination_ip.c_str());
    if (ip) {
        config.destination_ip = ip;
        config.save();
    }
}

void MessengerWindow::output_loop() {
    while (running) {
        std::cout << "waiting for new message" << std::endl;
        std::vector<char> message = state_machine->popOutputMessage();
        std::cout << "got new message: " << message.data() << std::endl;
        if (!message.empty()) {
            OutputMessage* msg = new OutputMessage{message, '>'};
            if(Fl::awake(update_output_cb, msg)) {
				std::cerr << "registration failed!" << std::endl;
			}
        }
    }
}

void MessengerWindow::update_output_cb(void* data) {
    OutputMessage* msg = static_cast<OutputMessage*>(data);
    MessengerWindow* window = static_cast<MessengerWindow*>(Fl::first_window());
    if (window && msg) {
        char prefix[2];
        prefix[0] = msg->prefix;
        prefix[1] = '\0';
        window->output->insert(prefix);
        window->output->insert(" ");
        window->output->insert(std::string(msg->text.begin(), msg->text.end()).c_str());
        window->output->insert("\n");
        delete msg;
    }
}

