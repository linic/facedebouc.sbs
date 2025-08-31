// configuration.cpp
#include "configuration.h"
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <iostream>

Configuration::Configuration() : sending_port(8081), receiving_port(8080), destination_ip("0.0.0.0") {}

void Configuration::read() {
    try {
        YAML::Node config = YAML::LoadFile(config_file);
        
        if (config["sending_port"]) {
            sending_port = config["sending_port"].as<int>();
        }
        if (config["receiving_port"]) {
            receiving_port = config["receiving_port"].as<int>();
        }
        if (config["destination_ip"]) {
            destination_ip = config["destination_ip"].as<std::string>();
        }
    } catch (const YAML::Exception& e) {
        std::cerr << "Error reading configuration file: " << e.what() << std::endl;
    }
}

void Configuration::save() {
    try {
        YAML::Node config;
        config["sending_port"] = sending_port;
        config["receiving_port"] = receiving_port;
        config["destination_ip"] = destination_ip;

        std::ofstream fout(config_file);
        fout << config;
    } catch (const std::exception& e) {
        std::cerr << "Error saving configuration file: " << e.what() << std::endl;
    }
}

