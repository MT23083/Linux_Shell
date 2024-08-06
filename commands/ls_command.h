#ifndef LS_COMMAND_H
#define LS_COMMAND_H

#include <iostream>
#include <vector>
#include <cstring>
#include <iomanip>
#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <ctime>
#include "command.h"

using namespace std;

class ls: public command{
    public:
        void getList(std::vector<std::string> option);
        void listInfo(std::string name, struct stat& fileStat, std::vector<std::string> option);
        void printWithSize(off_t size);
        void printLongFormat(struct stat& fileStat, std::vector<std::string> option);
        void printHelpls();
        void listDirectory(const std::string& path, const std::vector<std::string>& option);
};

#endif