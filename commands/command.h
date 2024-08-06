#ifndef COMMAND_H
#define COMMAND_H

#include <iostream>
#include<vector>
#include<algorithm>

using namespace std;

class command{
    public:
        std::vector<std::string> run_command(std::string command);
        std::vector<std::string> getCommandOptions(std::string command);
        bool hasOption(std::vector<std::string> option, std::string opt);
};

#endif