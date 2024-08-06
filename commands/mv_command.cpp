#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>
#include <filesystem>

#include "command.h"
#include "mv_command.h"

using namespace std;
namespace fs = std::filesystem;

void mv::printHelpMv() {
    cout << "Usage: mv [options] [source] [destination]\n"
      << "Move files from source to destination.\n"
      << "Options:\n"
      << "  --help     Display this help message\n"
      << "  -i         Prompt before overwriting\n"
      << "  -u         Move only when the SOURCE file is newer than the destination file\n"
      << "  -R         Recursively move entire directories\n"
      << "  -n         Do not overwrite existing files\n";
}

void mv::moveFile(const fs::path& source, const fs::path& destination, std::vector<std::string> option) {
    if (!hasOption(option, "-i") && !fs::exists(source)) {
        cout << "Error: Source file or directory does not exist." << endl;
        return;
    }

    if (fs::exists(destination)) {
        if (hasOption(option, "-n")) {
            cout << "Destination file already exists. Skipped.\n";
            return;
        }

        if (hasOption(option, "-u")) {
            auto sourceTime = fs::last_write_time(source);
            auto destTime = fs::last_write_time(destination);

            if (sourceTime <= destTime) {
                std::cout << "Source file is not newer than the destination file. Operation canceled.\n";
                return;
            }
        } else if (hasOption(option, "-i")) {
            cout << "File or directory already exists at the destination. Overwrite? (y/n): ";
            char response;
            cin >> response;

            if (response != 'y' && response != 'Y') {
                cout << "Operation canceled.\n";
                return;
            }
        } else {
            cout << "Error: Destination file or directory already exists.\n";
            return;
        }
    }

    try {
        if (fs::exists(destination)) {
            fs::remove_all(destination);
        }
        fs::rename(source, destination);
        std::cout << "Moved: " << source << " to: " << destination << std::endl;
    } catch (const exception& e) {
        cout << "Error: " << e.what() << std::endl;
    }
}

void mv::moveDirectory(const fs::path& source, const fs::path& destination, std::vector<std::string> option) {
    try {
        fs::create_directories(destination);  // Ensure destination directory exists
        for (const auto& entry : fs::directory_iterator(source)) {
            const auto& sourcePath = entry.path();
            const auto& destinationPath = destination / sourcePath.filename();

            if (fs::is_directory(sourcePath)) {
                moveDirectory(sourcePath, destinationPath, option);
            } else {
                fs::rename(sourcePath, destinationPath);
                cout << "Moved: " << sourcePath << " to: " << destinationPath << endl;
            }
        }
        cout << "Moved (recursively): " << source << " to: " << destination << endl;

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}

void mv::execute_mv(std::vector<std::string> option){
    string backupSuffix;
    string source;
    string destination;

    if(hasOption(option, "--help")){
        printHelpMv();
        return;
    }

    if(option.size() >= 3){
        destination = option.back();
        option.pop_back();
        source = option.back();
        option.pop_back();
    }else{
        cout << "Error enter source file and destination OR use --help option" << endl;
        return;
    }

    if (hasOption(option, "-R")) {
        if (fs::is_directory(source)) {
            moveDirectory(source, destination, option);
        } else {
            cout << "Error: Use -R option to move directories recursively" << endl;
        }
    } else {
        if (fs::is_directory(source)) {
            cout << "Error: Use -R option to move directories recursively" << endl;
            return;
        }

        if (!fs::is_directory(destination)) {
            cout << "Error: Destination must be a directory.\n";
            return;
        }

        moveFile(source, destination + "/" + source, option);
    }
}
