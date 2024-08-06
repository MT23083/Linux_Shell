#include <iostream>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <fstream>
#include <thread>
#include <mutex>

using namespace std;
namespace fs = std::filesystem;

std::mutex moveMutex;  // Mutex to synchronize access to shared resources

bool hasOption(std::vector<std::string> option, std::string opt) {
    return (find(option.begin(), option.end(), opt) != option.end());
}

void moveSingleFile(const fs::path& source, const fs::path& destination) {
    std::lock_guard<std::mutex> lock(moveMutex);

    try {
        fs::copy(source, destination, fs::copy_options::overwrite_existing);
        fs::remove(source);
        cout << "Moved: " << source << " to: " << destination << endl;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}

void moveDirectoryContents(const fs::path& source, const fs::path& destination, std::vector<std::string> option, size_t maxThreads) {
    try {
        fs::create_directories(destination);

        vector<thread> threads;
        const size_t hardwareConcurrency = std::thread::hardware_concurrency();

        for (const auto& entry : fs::directory_iterator(source)) {
            const auto& sourcePath = entry.path();
            const auto& destinationPath = destination / sourcePath.filename();

            if (fs::is_directory(sourcePath)) {
                threads.emplace_back(moveDirectoryContents, sourcePath, destinationPath, option, maxThreads);
            } else {
                threads.emplace_back(moveSingleFile, sourcePath, destinationPath);
            }

            // Limit the number of concurrent threads to the specified maximum or the hardware concurrency
            const size_t concurrentThreads = min(maxThreads, hardwareConcurrency);
            if (threads.size() >= concurrentThreads) {
                for (auto& thread : threads) {
                    thread.join();
                }
                threads.clear();
            }
        }

        for (auto& thread : threads) {
            thread.join();
        }

        cout << "Moved contents: " << source << " to: " << destination << endl;
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}

void moveFile(const fs::path& source, const fs::path& destination, std::vector<std::string> option, size_t maxThreads) {
    if (!fs::exists(source)) {
        cout << "Error: Source file or directory does not exist" << endl;
        return;
    }

    if (hasOption(option, "-n") && fs::exists(destination)) {
        cout << "Error: Destination file or directory already exists.\n";
        return;
    }

    try {
        if (fs::is_directory(source)) {
            moveDirectoryContents(source, destination, option, maxThreads);
        } else {
            moveSingleFile(source, destination);
        }
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
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

    const size_t maxThreads = std::thread::hardware_concurrency();
    moveFile(source, destination, options, maxThreads);

    return 0;
}
