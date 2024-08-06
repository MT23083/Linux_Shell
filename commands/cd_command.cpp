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
#include "cd_command.h"
using namespace std;

void cd::printHelpCd() {
    std::cout << "Usage: cd [options] [directory]\n"
              << "Change the current working directory.\n"
              << "Options:\n"
              << "  --help     Display this help message\n"
              << "  ..         Return to previous directory\n"
              << "  ~          Change to Home Directory\n"
              << "  /          Change to root directory\n"
              << "  <path>     Change to designated path";
}

void cd::changeDir(std::vector<std::string> option){
    if(hasOption(option, "..")){
        if(chdir("..") != 0){
            perror("chdir");
            return;
        }
    }else if(hasOption(option, "~")){
        // Change to the user's home directory
        const char *home_dir = getenv("HOME");
        if(home_dir == nullptr){
            cout << "Please set HOME env variable" << endl;
            return;
        }

        if(chdir(home_dir) != 0){
            perror("chdir");
            return;
        }
    }else if(hasOption(option, "/")){
        // Change to the root directory
        if(chdir("/") != 0){
            perror("chdir");
            return;
        }
    }else if(hasOption(option, "--help")){
        printHelpCd();
        return;
    }else{
        // Change to the specified directory
        const char* change_path = option.at(0).c_str();
        if(chdir(change_path) != 0){
            perror("chdir");
            return;
        }
    }


    // Print the current working directory after the change
    char cwd[256];
    if(getcwd(cwd, sizeof(cwd)) != nullptr){
        cout << "Current directory: " << cwd << endl;
    }else{
        perror("getcwd");
        return;
    }
}
