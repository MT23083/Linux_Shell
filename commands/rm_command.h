#ifndef RM_COMMAND_H
#define RM_COMMAND_H

#include <iostream>
#include <vector>
#include <filesystem>
#include <algorithm>

#include "command.h"

using namespace std;
namespace fs = std::filesystem;

class rm:public command{
    public:
        void removeFile(const fs::path& target, std::vector<std::string> option);
        void execute_rm(std::vector<std::string> option);
        void printHelpRm();
};


#endif