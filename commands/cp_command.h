#ifndef CP_COMMAND_H
#define CP_COMMAND_H

#include <iostream>
#include <vector>
#include <chrono>
#include <filesystem>
#include<algorithm>
#include <fstream>
#include "command.h"

using namespace std;
namespace fs = std::filesystem;

class cp: public command{
    public:
        void copyFile(const fs::path& source, const fs::path& destination, std::vector<std::string> option);
        void execute_cp(std::vector<std::string> option);
        void printHelpCp();
        void copyDirectory(const fs::path& source, const fs::path& destination, std::vector<std::string> option);
};

#endif
