#ifndef MV_COMMAND_H
#define MV_COMMAND_H

#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>
#include <filesystem>
#include "command.h"

using namespace std;
namespace fs = std::filesystem;

class mv: public command{
    public:
        void moveFile(const fs::path& source, const fs::path& destination, std::vector<std::string> option);
        void printHelpMv();
        void execute_mv(std::vector<std::string> option);
        void moveDirectory(const fs::path& source, const fs::path& destination, std::vector<std::string> option);
};

#endif
