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
#include "ls_command.h"

using namespace std;

void ls::printHelpls() {
    std::cout << "Usage: ls [options] [directory]\n"
              << "List contents in specified directory.\n"
              << "Options:\n"
              << "  --help     Display this help message\n"
              << "  -a         List all entries, including hidden files\n"
              << "  -R         List subdirectories recursively\n"
              << "  -Q         Enclose entry names in double quotes\n"
              << "  -lh        List in long format with human-readable sizes\n"
              << "  -l         Long Format Display\n";
}

void ls::getList(std::vector<std::string> option) {
    if(hasOption(option, "--help")){
        printHelpls();
        return;
    }


    string path = ".";
    if (!option.empty()) {
        //get path of the file and remove it from vector
        path = option.back();
        option.pop_back();
    }

    listDirectory(path, option);


//    DIR* dir = opendir(path.c_str());
//    if (!dir) {
//        cout << "Directory not found." << endl;
//        return;
//    }
//
//    struct dirent* row;
//    while((readdir(dir)) != nullptr){
//        row = readdir(dir);
//        if(!hasOption(option, "-a") && row->d_name[0] == '.'){
//            continue; //no hidden files if -a option not there
//        }
//
//        string path_name = path + "/" + row->d_name;
//        struct stat fileStat;
//
//        if(stat(path_name.c_str(), &fileStat) == -1){
//            cout << "Failed to get file information for '" << row->d_name << endl;
//            continue;
//        }
//
//        listInfo(row->d_name, fileStat, option);
//
//
//        if (hasOption(option, "-R") && S_ISDIR(fileStat.st_mode)) {
//            cout << "\n" << path_name << ":\n";
//            listInfo(path_name, fileStat, option);
//        }
//    }
}

void ls::listDirectory(const std::string& path, const std::vector<std::string>& option) {
    DIR* dir = opendir(path.c_str());
    if (!dir) {
        cout << "Directory not found." << endl;
        return;
    }

    struct dirent* row;
    while ((row = readdir(dir)) != nullptr) {
        if (!hasOption(option, "-a") && row->d_name[0] == '.') {
            continue; // No hidden files if -a option not there
        }

        string path_name = path + "/" + row->d_name;
        struct stat fileStat;

        if (stat(path_name.c_str(), &fileStat) == -1) {
            cout << "Failed to get file information for '" << row->d_name << endl;
            continue;
        }

        listInfo(row->d_name, fileStat, option);

        if (hasOption(option, "-R") && S_ISDIR(fileStat.st_mode) && string(row->d_name) != "." && string(row->d_name) != "..") {
            cout << "\n" << path_name << ":\n";
            listDirectory(path_name, option);
        }
    }

    closedir(dir);
}


void ls::listInfo(std::string name, struct stat& fileStat, std::vector<std::string> option){
    if(hasOption(option, "-Q")){
        cout << "\"" << name << "\" ";
    } else{
        cout << name << " ";
    }

    if(hasOption(option, "-lh")){
        printWithSize(fileStat.st_size);
    }

    if(hasOption(option, "-l")){
        printLongFormat(fileStat, option);
    }

    cout << endl;
}

void ls::printLongFormat(struct stat& fileStat, std::vector<std::string> option) {
    cout << ((S_ISDIR(fileStat.st_mode)) ? "d" : "-");
    cout << ((fileStat.st_mode & S_IRUSR) ? "r" : "-");
    cout << ((fileStat.st_mode & S_IWUSR) ? "w" : "-");
    cout << ((fileStat.st_mode & S_IXUSR) ? "x" : "-");
    cout << ((fileStat.st_mode & S_IRGRP) ? "r" : "-");
    cout << ((fileStat.st_mode & S_IWGRP) ? "w" : "-");
    cout << ((fileStat.st_mode & S_IXGRP) ? "x" : "-");
    cout << ((fileStat.st_mode & S_IROTH) ? "r" : "-");
    cout << ((fileStat.st_mode & S_IWOTH) ? "w" : "-");
    cout << ((fileStat.st_mode & S_IXOTH) ? "x" : "-");

    // Number of hard links
    cout << " " << fileStat.st_nlink;

    // Owner
    struct passwd* owner = getpwuid(fileStat.st_uid);
    cout << " " << ((owner) ? owner->pw_name : to_string(fileStat.st_uid));

    // Group
    struct group* grp = getgrgid(fileStat.st_gid);
    cout << " " << ((grp) ? grp->gr_name : to_string(fileStat.st_gid));

    // Size
    cout << setw(6) << fileStat.st_size;

    // Last modification time
    char date[13];
    strftime(date, 13, "%b %d %H:%M", localtime(&(fileStat.st_mtime)));
    cout << " " << date;
}

void ls::printWithSize(off_t size){
    const char* sizes[] = {"B", "KB", "MB", "GB"};
    int index = 0;

    while(size >= 1024 && index < 3){
        size = size/1024;
        index++;
    }

    cout << setw(3) << size << " " << sizes[index];
}