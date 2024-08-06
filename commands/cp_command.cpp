#include <iostream>
#include <vector>
#include <chrono>
#include <filesystem>
#include<algorithm>
#include <fstream>

#include "command.h"
#include "cp_command.h"

using namespace std;
namespace fs = std::filesystem;

void cp::printHelpCp() {
    std::cout << "Usage: cp [options] [source] [directory]\n"
              << "Copy contents from source to destination.\n"
              << "Options:\n"
              << "  --help     Display this help message\n"
              << "  -n         Do not overwrite existing files\n"
              << "  -u         Copy only when the SOURCE file is newer than the destination file\n"
              << "  -i         Prompt before overwriting existing files\n"
              << "  -a         Copy recursively, preserving all attributes\n"
              << "  -R         Same as -a, for compatibility with some versions of cp\n";
}

void cp::copyDirectory(const fs::path& source, const fs::path& destination, std::vector<std::string> option) {
    try {  // Ensure destination directory exists
        fs::create_directories(destination);
        for (const auto& entry : fs::directory_iterator(source)) {
            const auto& sourcePath = entry.path();
            const auto& destinationPath = destination / sourcePath.filename();

            if (fs::is_directory(sourcePath)) {
                // Recursively copy subdirectories
                copyDirectory(sourcePath, destinationPath, option);
            } else {
                const auto& destinationFile = destination / sourcePath.filename();
                // Copy a single file
                fs::copy_file(sourcePath, destinationFile);
            }
        }
        cout << "Copied (recursively): " << source << " to: " << destination << endl;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}

void cp::copyFile(const fs::path& source, const fs::path& destination, std::vector<std::string> option) {
    if (!fs::exists(source)) {
        cout << "Error: Source file or directory does not exist" << endl;
        return;
    }

    if (hasOption(option, "-n") && fs::exists(destination)) {
        cout << "Error: Destination file or directory already exists.\n";
        return;
    }

    if (hasOption(option, "-u") && fs::exists(destination)) {
        auto sourceTime = fs::last_write_time(source);
        auto destTime = fs::last_write_time(destination);

        if (sourceTime <= destTime) {
            cout << "Source file is not newer than the destination file. Operation canceled.\n";
            return;
        }
    }

    try {
        if (hasOption(option, "-i") && fs::exists(destination)) {
            cout << "File or directory already exists at the destination. Overwrite? (y/n): ";
            char response;
            std::cin >> response;

            if (response != 'y' && response != 'Y') {
                std::cout << "Operation canceled.\n";
                return;
            }else{
                fs::path path_s = fs::current_path() / source;
                fs::path path_d = fs::current_path() / destination / source;
                std::ofstream ofs(path_d, std::ios::trunc); // Open for writing, truncate existing content
                std::ifstream ifs(path_s, std::ios::binary); // Open for reading in binary mode

                if (ifs.is_open() && ofs.is_open()) {
                    ofs << ifs.rdbuf(); // Copy contents from source to destination
                    cout << "Overwritten: " << source << " to: " << destination << endl;

                } else {
                    cout << "Error: Unable to open files for reading/writing" << endl;
                }
                return;
            }
        }

        if (hasOption(option, "-a")) {
            // Copy with preservation of attributes
            fs::copy(source, destination);
        } else {
            if (hasOption(option, "-R")) {
                // Copy directories recursively
                copyDirectory(source, destination, option);
            } else {
                // Copy a single file
                fs::copy_file(source, destination);
            }
        }

        cout << "Copied: " << source << " to: " << destination << endl;
    } catch (const std::exception& e) {
        cout << "Error: " << e.what() << std::endl;
    }
}

void cp::execute_cp(std::vector<std::string> option){
    string source;
    string destination;
    if(option.size() >= 3){
        destination = option.back();
        option.pop_back();
        source = option.back();
        option.pop_back();
    }else if(hasOption(option, "--help")){
        printHelpCp();
        return;
    }
    else{
        cout << "Error enter source file and destination" << endl;
        printHelpCp();
        return;
    }

    copyFile(source, destination, option);
}
