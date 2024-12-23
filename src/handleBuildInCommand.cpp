
#include <iostream>
#include <string>
#include <vector>
#include <stdio.h> 
#include <unistd.h>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>

#include "handleBuildInCommand.hpp"
#include "getCommandType.hpp"
#include "parseCommand.hpp"

bool handleBuildInCommand(const std::vector<std::string>& tokens, int &lastExitCode,
                           int outFd) {
    Command commandType = getCommandType(tokens[0]);
    
    switch (commandType) {
        case EXIT: {
            if (tokens.size() > 1 && (tokens[1] == "-h" || tokens[1] == "--help")) {
                std::cout << "Usage: mexit\n"
                          "Exits the shell program.\n";
                return true;
            }
            exit(0);
            return true;
        }
        case SCRIPT: {
            if (tokens.size() > 1 && (tokens[1] == "-h" || tokens[1] == "--help")) {
                std::cout << "Usage: . <script_file>\n"
                          "Executes the commands in the specified script file.\n";
                return true;
            }
            if (tokens.size() < 2) {
                std::cerr << "Usage: . <script_file>" << std::endl;
                lastExitCode = 1;
                return true;
            }
            const std::string& scriptFile = tokens[1];
            std::ifstream file(scriptFile);
            if (!file) {
                std::cerr << "mscript: file not found: " << scriptFile << std::endl;
                lastExitCode = 1;
                return true;
            }

            std::string line;
            while (std::getline(file, line)) {
                parseCommand(line, lastExitCode);
            }
            lastExitCode = 0;
            return true;
        }
        case CD: {
            if (tokens.size() > 1 && (tokens[1] == "-h" || tokens[1] == "--help")) {
                std::cout << "Usage: mcd <directory>\n"
                          "Changes the current working directory to the specified directory.\n";
                return true;
            }
            if (tokens.size() < 2) {
                std::cerr << "mcd: expected argument" << std::endl;
                lastExitCode = 1;
            } else {
                const char* path = tokens[1].c_str();
                if (chdir(path) != 0) { 
                    perror("mcd");
                    lastExitCode = 1;
                } else {
                    lastExitCode = 0;
                }
            }
            return true;
        }
        case ECHO: {
            if (tokens.size() > 1 && (tokens[1] == "-h" || tokens[1] == "--help")) {
                std::cout << "Usage: mecho <string>\n"
                          "Prints the specified string to the console.\n";
                return true;
            }
            if (tokens.size() < 2) {
                return true;
            }

            for (size_t i = 1; i < tokens.size(); ++i) {
                if (tokens[i][0] == '$' && tokens[i].size() > 1) {
                    const char* env_value = getenv(tokens[i].substr(1).c_str());
                    if (env_value) {
                        std::cout << env_value;
                    } else {
                        std::cerr<<"ERROR: No such enviroment variable";
                        lastExitCode = 1;
                        return true;
                    }
                }
                else if (outFd != STDOUT_FILENO) {
                    dprintf(outFd, "%s ", tokens[i].c_str());
                } else {
                    std::cout << tokens[i] << " ";
                }
            }
            std::cout << std::endl;
            lastExitCode = 0;
            return true;
        }
        case PWD: {
            if (tokens.size() > 1 && (tokens[1] == "-h" || tokens[1] == "--help")) {
                std::cout << "Usage: mpwd\n"
                          "Prints the current working directory.\n";
                return true;
            }
            char cwd[1024]; 
            if (getcwd(cwd, sizeof(cwd)) != nullptr) {
                if (outFd != STDOUT_FILENO) {
                    dprintf(outFd, "%s\n", cwd); 
                } else {
                    std::cout << cwd << std::endl;
                }
                lastExitCode = 0;
            } else {
                perror("mpwd");
                lastExitCode = 1;
            }
            return true;
        }
        case ERRNO: {   
            if (tokens.size() > 1) {
                if (tokens[1] == "-h" || tokens[1] == "--help") {
                    std::cout << "Usage: merrno [-h|--help]\n"
                              "Prints the exit code of the last executed command.\n"
                              "Returns 0 if the last command executed successfully or no commands have been executed yet.\n";
                    return true;
                } else {
                    std::cerr << "merrno: invalid option\n";
                    lastExitCode = 1;  
                    return true;
                }
            }
            if (outFd != STDOUT_FILENO) {
                dprintf(outFd, "%d\n", lastExitCode); 
            } else {
                std::cout << lastExitCode << std::endl;
            }
            lastExitCode = 0; 
            return true;
        }
        case EXPORT: {
    if (tokens.size() > 1 && (tokens[1] == "-h" || tokens[1] == "--help")) {
        std::cout << "Usage: mexport var_name=VAL\n"
                  << "Sets the environment variable 'var_name' to 'VAL'.\n"
                  << "Supports command substitution (e.g., mexport VAR=$(ls)).\n";
        return true;
    }
    if (tokens.size() < 2) {
        std::cerr << "mexport: expected argument\n";
        lastExitCode = 1;
        return true;
    }

    std::string varAssignment = tokens[1];
    size_t equalPos = varAssignment.find('=');

    if (equalPos == std::string::npos) {
        std::cerr << "mexport: invalid format. Use var_name=VAL.\n";
        lastExitCode = 1;
        return true;
    }

    std::string varName = varAssignment.substr(0, equalPos);
    std::string varValue = varAssignment.substr(equalPos + 1);

    if (varValue.size() >= 3 && varValue[0] == '$' && varValue[1] == '(' && varValue.back() == ')') {
        std::string command = varValue.substr(2, varValue.size() - 3);

        FILE* pipe = popen(command.c_str(), "r");
        if (!pipe) {
            perror("mexport");
            lastExitCode = 1;
            return true;
        }

        char buffer[128];
        std::string result;
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
        pclose(pipe);

        if (!result.empty() && result.back() == '\n') {
            result.pop_back();
        }

        varValue = result;
    }

    if (varName.empty() || !isalnum(varName[0])) {
        std::cerr << "mexport: invalid variable name '" << varName << "'.\n";
        lastExitCode = 1;
        return true;
    }

    if (setenv(varName.c_str(), varValue.c_str(), 1) != 0) {
        perror("mexport");
        lastExitCode = 1;
    } else {
        lastExitCode = 0;
    }

    return true;
}

        case UNKNOWN: {
            return false;
        }
    }
    return false;
}
