#ifndef CD_COMMAND_H
#define CD_COMMAND_H

#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <iomanip>
#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>
#include "command.h"

using namespace std;

class cd: public command{
    public:
        void changeDir(std::vector<std::string> option);
        void printHelpCd();
};

#endif