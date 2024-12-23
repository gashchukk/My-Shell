#include <string>
#include <vector>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <sys/wait.h>
#include <signal.h>
#include <array>

#include "parseCommand.hpp"
#include "handleBuildInCommand.hpp"
#include "executeExternalCommand.hpp"


std::vector<std::string> parsePipeline(const std::string& command_part) {
    std::vector<std::string> commands;
    std::istringstream stream(command_part);
    std::string command;
    while (std::getline(stream, command, '|')) {
        commands.push_back(command);
    }
    return commands;
}

void parseCommand(const std::string& line, int &lastExitCode) {

    std::vector<std::string> tokens;
    std::string outputFile, errorFile, inputFile;
    bool redirectStdout = false, redirectStderr = false, redirectBoth = false, redirectStdin = false, isBackground = false;
    int outFd = STDOUT_FILENO, errFd = STDERR_FILENO, inFd = STDIN_FILENO;


    size_t comment_pos = line.find('#');
    std::string command_part = line.substr(0, comment_pos);
    
    
    if (command_part.find('|') != std::string::npos) {
    std::vector<std::string> commands = parsePipeline(command_part);

    int numCommands = commands.size();
    std::vector<std::array<int, 2>> pipes(numCommands - 1);

    for (int i = 0; i < numCommands - 1; ++i) {
        if (pipe(pipes[i].data()) == -1) {
            std::cerr << "Error: Failed to create pipe.\n";
            return;
        }
    }

    for (int i = 0; i < numCommands; ++i) {
        std::istringstream stream(commands[i]);
        tokens.clear();
        std::string token;

        bool redirectStdout = false, redirectStderr = false, redirectStdin = false;
        std::string outputFile, errorFile, inputFile;

        while (stream >> token) {
            if (token == ">") {
                redirectStdout = true;
                if (!(stream >> token)) {
                    std::cerr << "Error: Missing file for stdout redirection.\n";
                    return;
                }
                outputFile = token;
            } else if (token == "2>") {
                redirectStderr = true;
                if (!(stream >> token)) {
                    std::cerr << "Error: Missing file for stderr redirection.\n";
                    return;
                }
                errorFile = token;
            } else if (token == "<") {
                redirectStdin = true;
                if (!(stream >> token)) {
                    std::cerr << "Error: Missing file for stdin redirection.\n";
                    return;
                }
                inputFile = token;
            } else {
                tokens.push_back(token);
            }
        }

        pid_t pid = fork();
        if (pid == 0) { 
            if (redirectStdin) {
                int inFd = open(inputFile.c_str(), O_RDONLY);
                if (inFd == -1) {
                    std::cerr << "Error: Failed to open input file for redirection.\n";
                    exit(1);
                }
                dup2(inFd, STDIN_FILENO);
                close(inFd);
            }

            if (redirectStdout) {
                int outFd = open(outputFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (outFd == -1) {
                    std::cerr << "Error: Failed to open output file for redirection.\n";
                    exit(1);
                }
                dup2(outFd, STDOUT_FILENO);
                close(outFd);
            }

            if (redirectStderr) {
                int errFd = open(errorFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (errFd == -1) {
                    std::cerr << "Error: Failed to open error file for redirection.\n";
                    exit(1);
                }
                dup2(errFd, STDERR_FILENO);
                close(errFd);
            }

            if (i > 0) { 
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }
            if (i < numCommands - 1) { 
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            for (int j = 0; j < numCommands - 1; ++j) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            if (!tokens.empty()) {
                if (!handleBuildInCommand(tokens, lastExitCode, outFd)) {
                    executeExternalCommand(tokens, lastExitCode, outFd, errFd, inFd);
                }
            }
            exit(0);
        } else if (pid < 0) {
            std::cerr << "Error: Failed to fork process for pipeline.\n";
            return;
        }
    }

    for (int i = 0; i < numCommands - 1; ++i) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (int i = 0; i < numCommands; ++i) {
        wait(nullptr);
    }

    return;
}

    std::istringstream stream(command_part);
    std::string token;

    while (stream >> token) {
        if (token == ">") {
            redirectStdout = true;
            if (!(stream >> token)) {
                std::cerr << "Error: Missing file for stdout redirection.\n";
                return;
            }
            outputFile = token;
        } else if (token == "2>") {
            redirectStderr = true;
            if (!(stream >> token)) {
                std::cerr << "Error: Missing file for stderr redirection.\n";
                return;
            }
            errorFile = token;
        } else if (token == "2>&1" || token == "&>" || token == ">&") {
            redirectStdout = true;
            redirectStderr = true;
            if (!(stream >> token)) {
                std::cerr << "Error: Missing file for stdout and stderr redirection.\n";
                return;
            }
            outputFile = token;
        } else if (token == "<") {
            redirectStdin = true;
            if (!(stream >> token)) {
                std::cerr << "Error: Missing file for stdin redirection.\n";
                return;
            }
            inputFile = token;
        } else if (token == "&") {
            isBackground = true;
        } else {
            tokens.push_back(token);
        }
    }

    if (redirectStdout && redirectStderr) {
        outFd = open(outputFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (outFd == -1) {
            std::cerr << "Error: Could not open file for stdout redirection: " << outputFile << std::endl;
            return;
        }
        errFd = outFd; 
    } else {
        if (redirectStdout) {
            outFd = open(outputFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (outFd == -1) {
                std::cerr << "Error: Could not open file for stdout redirection: " << outputFile << std::endl;
                return;
            }
        }

        if (redirectStderr) {
            errFd = open(errorFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (errFd == -1) {
                std::cerr << "Error: Could not open file for stderr redirection: " << errorFile << std::endl;
                return;
            }
        }

        if (redirectStdin) {
            inFd = open(inputFile.c_str(), O_RDONLY);
            if (inFd == -1) {
                std::cerr << "Error: Could not open file for stdin redirection: " << inputFile << std::endl;
                return;
            }
        }
    }
    if (isBackground) {
                pid_t pid = fork();
                if (pid == 0) {
                    if (outFd != STDOUT_FILENO) {
                        dup2(outFd, STDOUT_FILENO);
                    }
                    if (errFd != STDERR_FILENO) {
                        dup2(errFd, STDERR_FILENO);
                    }
                    if (inFd != STDIN_FILENO) {
                        dup2(inFd, STDIN_FILENO);
                    }
                    if (!tokens.empty()) {
                        if (!handleBuildInCommand(tokens, lastExitCode, outFd)) {
                                executeExternalCommand(tokens, lastExitCode, outFd, errFd, inFd);
                        }
                    }
                    
                    exit(0); 
                } else if (pid < 0) {
                    std::cerr << "Error: Failed to fork background process." << std::endl;
                } else {
                    std::cout << "Background process PID: " << pid << std::endl;
                }
    } else {
       if (!tokens.empty()) {
            if (!handleBuildInCommand(tokens, lastExitCode, outFd)) {
                executeExternalCommand(tokens, lastExitCode, outFd, errFd, inFd);
            }
        }
    }
    

    if (redirectStdout) close(outFd);
    if (redirectStderr) close(errFd);
    if (redirectStdin) close(inFd);
}