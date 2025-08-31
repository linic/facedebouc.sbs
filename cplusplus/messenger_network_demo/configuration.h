// configuration.h
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <string>

class Configuration {
public:
    Configuration();
    void read();
    void save();

    int sending_port;
    int receiving_port;
    std::string destination_ip;

private:
    const std::string config_file = "configuration.yaml";
};

#endif // CONFIGURATION_H
