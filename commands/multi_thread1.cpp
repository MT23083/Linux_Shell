#include <iostream>
#include <vector>
#include <chrono>
#include <filesystem>
#include <algorithm>
#include <fstream>
#include <thread>
#include <mutex>

using namespace std;
namespace fs = std::filesystem;

std::mutex copyMutex;  // Mutex to synchronize access to shared resources

bool hasOption(std::vector<std::string> option, std::string opt) {
    return (find(option.begin(), option.end(), opt) != option.end());
}

void copySingleFile(const fs::path& source, const fs::path& destination) {
    std::lock_guard<std::mutex> lock(copyMutex);

    try {
        fs::copy_file(source, destination);
        cout << "Copied: " << source << " to: " << destination << endl;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}

void copyDirectory(const fs::path& source, const fs::path& destination, std::vector<std::string> option, size_t maxThreads) {
    try {
        fs::create_directories(destination);

        vector<thread> threads;
        for (const auto& entry : fs::directory_iterator(source)) {
            const auto& sourcePath = entry.path();
            const auto& destinationPath = destination / sourcePath.filename();

            if (fs::is_directory(sourcePath)) {
                threads.emplace_back(copyDirectory, sourcePath, destinationPath, option, maxThreads);
            } else {
                threads.emplace_back(copySingleFile, sourcePath, destinationPath);
            }

            // Limit the number of concurrent threads to the hardware concurrency
            if (threads.size() >= maxThreads) {
                for (auto& thread : threads) {
                    thread.join();
                }
                threads.clear();
            }
        }

        for (auto& thread : threads) {
            thread.join();
        }

        cout << "Copied (recursively): " << source << " to: " << destination << endl;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}

void copyDirectory(const fs::path& source, const fs::path& destination, std::vector<std::string> option) {
    size_t maxThreads = std::thread::hardware_concurrency();
    copyDirectory(source, destination, option, maxThreads);
}

void copyFile(const fs::path& source, const fs::path& destination, std::vector<std::string> option) {
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

int main(int argc, char* argv[]) {
    if (argc < 4) {
        cerr << "Usage: " << argv[0] << " [options] [source] [directory]\n";
        return 1;
    }

    std::vector<std::string> options(argv + 1, argv + argc - 2);
    std::string source(argv[argc - 2]);
    std::string destination(argv[argc - 1]);

    copyFile(source, destination, options);

    return 0;
}
