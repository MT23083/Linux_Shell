#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <filesystem>
#include <cstring>
#include <iomanip>
#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <ctime>

using namespace std;
namespace fs = std::filesystem;

// Global mutex for synchronization
std::mutex mtx;

void printHelpls() {
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

void listDirectory(const fs::path& path, const std::vector<std::string>& option, int depth);

void listInfo(std::string name, const fs::path& path, const std::vector<std::string>& option);

void printWithSize(off_t size);

bool hasOption(std::vector<std::string> option, std::string opt) {
    return (find(option.begin(), option.end(), opt) != option.end());
}

void processEntry(const fs::directory_entry& entry, const std::vector<std::string>& option, int depth) {
    std::lock_guard<std::mutex> lock(mtx);
    listInfo(entry.path().filename().string(), entry.path(), option);

    if (depth > 0 && entry.is_directory()) {
        std::thread(listDirectory, entry.path(), option, depth - 1).detach();
    }
}

void listDirectory(const fs::path& path, const std::vector<std::string>& option, int depth) {
    for (const auto& entry : fs::directory_iterator(path)) {
        if (!hasOption(option, "-a") && entry.path().filename().string()[0] == '.') {
            continue; // No hidden files if -a option not there
        }

        processEntry(entry, option, depth);
    }
}

void listInfo(std::string name, const fs::path& path, const std::vector<std::string>& option) {
    if (hasOption(option, "-Q")) {
        cout << "\"" << name << "\" ";
    } else {
        cout << name << " ";
    }

    if (hasOption(option, "-lh")) {
        printWithSize(fs::file_size(path));
    }

    cout << endl;
}

void printWithSize(off_t size) {
    // Same as before...
    const char* sizes[] = {"B", "KB", "MB", "GB"};
        int index = 0;

        while(size >= 1024 && index < 3){
            size = size/1024;
            index++;
        }

        cout << setw(3) << size << " " << sizes[index];
}


int main(int argc, char* argv[]) {
    std::vector<std::string> options(argv + 1, argv + argc);
    fs::path path = ".";
    int depth = std::thread::hardware_concurrency(); // Use hardware_concurrency for the initial depth

    if (!options.empty() && options.back() != "--help") {
        path = options.back();
        options.pop_back();
    }

    // Recursively listing all the files based on the path and options provided along with it
    listDirectory(path, options, depth);

    // Sleep to allow detached threads to finish
    std::this_thread::sleep_for(std::chrono::seconds(2));

    return 0;
}
