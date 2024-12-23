#include <unistd.h>
#include <vector>
#include <string>
#include <fcntl.h>
#include <sys/wait.h>
#include <iostream>

#include "executeExternalCommand.hpp"


void executeExternalCommand(const std::vector<std::string>& tokens, int &lastExitCode,
                            int outFd, int errFd, int inFd) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return;
    }
    if (pid == 0) { 
        if (outFd != STDOUT_FILENO) {
            dup2(outFd, STDOUT_FILENO);
        }

        if (errFd == outFd) { 
            dup2(outFd, STDERR_FILENO);
        } else if (errFd != STDERR_FILENO) {
            dup2(errFd, STDERR_FILENO);
        }

        if (outFd != STDOUT_FILENO) close(outFd);
        if (errFd != STDERR_FILENO && errFd != outFd) close(errFd);

        if (inFd != STDIN_FILENO) {
            dup2(inFd, STDIN_FILENO);
            close(inFd);
        }

        std::vector<char*> args;
        for (const auto& token : tokens) {
            if (token[0] == '$' && token.size() > 1) {
                const char* env_value = getenv(token.substr(1).c_str());
                if (env_value) {
                    args.push_back(const_cast<char*>(env_value));
                } else {
                    args.push_back(const_cast<char*>(""));
                }
            } else {
                args.push_back(const_cast<char*>(token.c_str()));
            }
        }

        args.push_back(nullptr);
        execvp(args[0], args.data());
        perror("execvp"); 
        exit(127); 
    } else { 
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            lastExitCode = WEXITSTATUS(status);
        } else {
            lastExitCode = -1; 
        }
    }
}
