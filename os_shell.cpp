#include <iostream>
#include <istream>
#include <fstream>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

bool runCommand(std::string theCommand)
{
    int argc = 0;
    char *argv[100];
    char commandString[1024];

    strcpy(commandString, theCommand.c_str());

    char * cp;
    cp = strtok(commandString, " \t\n");
    while (cp) {
        argv[argc++] = cp;
        cp = strtok(NULL, " \t\n");
    }
    argv[argc] = NULL;


    pid_t pid = fork();
    if (pid == 0) {
        // Child process - exec the command
        if(execvp(argv[0], argv) == -1) {
            exit(1);
        }
    }

    return true;
}


int main(int argc, char ** argv)
{

    istream *inStream;
    ifstream inFile;
    bool interactive;

    if (argc == 1) {
        // Using interactive mode.
        interactive = true;
        inStream = &cin;
    } else {
        interactive = false;
        inFile.open(argv[1]);
        if (! inFile.good()) {
            cerr << "Failed to open file " << argv[1] << endl;
            exit(10);
        }

        inStream = &inFile;
    }

    std::string inLine;

    if (interactive) {
        cout << "Make your next wish> " ;
    }
    while( getline(*inStream, inLine)) {

        int num_children = 0;
        bool quit = false;
        while(inLine.length()) {
            string theCommand;

            size_t pos = inLine.find(";");
            if (pos == string::npos) {
                theCommand = inLine;
                inLine.erase();
            } else {
                theCommand = inLine.substr(0, pos);
                inLine.erase(0, pos+1);
            }


            // Trim leading and trailing spaces
            theCommand.erase(0, theCommand.find_first_not_of(" \t\n"));
            theCommand.erase(theCommand.find_last_not_of(" \t\n") + 1);

            if (theCommand == "quit") {
                // Quit after finishing any other commands in this cycle
                quit = true;
            } else if (theCommand.length()) {
                runCommand(theCommand);
                num_children++;
            }
        }

        while(num_children) {
            if (wait(NULL) > 0) {
                --num_children;
            }
        }

        if (quit) {
            return(0);
        }

        if (interactive) {
            cout << "Make your next wish> " ;
        }
    }

}

