#include <iostream>
#include <vector>
#include <filesystem>
#include <algorithm>

#include "command.h"
#include "rm_command.h"

using namespace std;
namespace fs = std::filesystem;

void rm::printHelpRm(){
    cout << "Usage: rm [options] [destination]\n"
      << "Remove Destination files.\n"
      << "Options:\n"
      << "  --help     Display this help message\n"
      << "  -d         Remove empty directories\n"
      << "  -v         Explain what is being done\n"
      << "  -R         Remove directories and their contents recursively\n"
      << "  -i         Prompt before every removal\n"
      << "  -f         Force removal without confirmation\n";
}

// Function to remove a file or directory
void rm::removeFile(const fs::path& target, std::vector<std::string> option) {
    try {
        if (fs::is_directory(target)) {
            // Remove directory
            if (hasOption(option, "-d") && fs::is_empty(target)) {
                fs::remove(target);
                if (hasOption(option, "-v"))
                    cout << "Removed directory: " << target << endl;
            }
            else if (hasOption(option, "-R")) {
                fs::remove_all(target);
                if (hasOption(option, "-v"))
                    cout << "Recursively removed directory: " << target << endl;
            } else if (hasOption(option, "-f")) {
                fs::remove(target);
                if (hasOption(option, "-v"))
                    cout << "Removed directory: " << target << endl;
            } else if (hasOption(option, "-i")) {
                cout << "Remove directory " << target << "? (y/n): ";
                char response;
                std::cin >> response;

                if (response == 'y' || response == 'Y') {
                    fs::remove_all(target);
                    if (hasOption(option, "-v")) cout << "Removed directory: " << target << std::endl;
                } else {
                    cout << "Operation canceled.\n";
                }
            } else {
                cerr << "Error: Directory is not empty. Use -r or --recursive to remove it.\n";
            }
        }
        else {
            // Remove file
            if (hasOption(option, "-f") || hasOption(option, "-i")) {
                fs::remove(target);
                if (hasOption(option, "-v")) cout << "Removed file: " << target << endl;
            } else {
                cerr << "Error: Specify -f or --force to remove files" << endl;
            }
        }
    } catch (const std::exception& e) {
        cout << "Error: " << e.what() << endl;
    }
}

void rm::execute_rm(std::vector<std::string> option){
    if(hasOption(option, "--help")){
        printHelpRm();
        return;
    }
    string destination;
    if(option.size() >= 2){
        destination = option.back();
        option.pop_back();
    }else{
        cout << "Error enter destination file" << endl;
        printHelpRm();
        return;
    }

    removeFile(destination, option);
}


