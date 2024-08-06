#include <iostream>
#include <vector>
#include <cstring>
#include <iomanip>
#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>
#include <fstream>
#include "command.h"
#include "ls_command.h"
#include "cp_command.h"
#include "cd_command.h"
#include "mv_command.h"
#include "rm_command.h"

int main(){
    command com;
    ls list_file;
    cp copyFile;
    cd change_path;
    rm removeFile;
    mv move_file;

    vector<string> tokens;

    string input;

    while(1){
        cout << "<< ~$ ";
        getline(cin, input);

        if(input == "exit")
            break;

        tokens = com.run_command(input);

        if(tokens.empty())
            continue;

        //get the command and remove it from the list
        string input_com = tokens.at(0);
        tokens.erase(tokens.begin());

        if(input_com == "ls"){
            list_file.getList(tokens);
        }

        else if(input_com == "cd"){
            change_path.changeDir(tokens);
        }

        else if(input_com == "cp")
            copyFile.execute_cp(tokens);

        else if(input_com == "mv")
            move_file.execute_mv(tokens);

        else if(input_com == "rm")
            removeFile.execute_rm(tokens);

        else
            cout << "Invalid command, Please enter ls, rm, cp, cd, mv commands only. Use --help for options" << endl;


        cout << endl;

    }

    return 0;
}