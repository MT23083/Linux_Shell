#include <iostream>
#include<vector>
#include<algorithm>
#include "command.h"

using namespace std;

std::vector<std::string> command::run_command(std::string command){
    vector<string> token = getCommandOptions(command);

    //cout << "Tokens: " << token.at(0) << " " << token.at(1) << " " << token.at(2);

    if (token.empty()) {
        cout << "No command Found" << endl;
        return token;
    }

    //string str = token.at(0);
    //token.erase(token.begin());
    return token;
}

std::vector<std::string> command::getCommandOptions(std::string command){
    vector<string> tokens;
    size_t start = 0, end = 0;

    while ((end = command.find(' ', start)) != string::npos) {
        tokens.push_back(command.substr(start, end - start));
        start = end + 1;
    }

    tokens.push_back(command.substr(start));
    return tokens;
}

bool command::hasOption(std::vector<std::string> option, std::string opt) {
    return (find(option.begin(), option.end(), opt) != option.end());
}
