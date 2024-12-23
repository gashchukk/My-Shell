#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <cstring>
#include <vector>
#include <fstream>

#include "executeScript.hpp"
#include "parseCommand.hpp"
#include "displayPrompt.hpp"

int lastExitCode = 0;  
void zombie_handler(int sig) {
    int wstat;
    while (true) {
        pid_t pid = wait3(&wstat, WNOHANG, NULL);
        if (pid == 0 || pid == -1) {
            return; 
        }
        if (WIFEXITED(wstat)) {
            std::cout << "Process " << pid << " exited with status " << WEXITSTATUS(wstat) << std::endl;
        } else if (WIFSIGNALED(wstat)) {
            std::cout << "Process " << pid << " was killed by signal " << WTERMSIG(wstat) << std::endl;
        } else if (WIFSTOPPED(wstat)) {
            std::cout << "Process " << pid << " stopped by signal " << WSTOPSIG(wstat) << std::endl;
        }
    }
}
int main(int argc, char* argv[]) {
    signal(SIGCHLD, zombie_handler);
    if (argc == 2) {
        executeScript(argv[1], lastExitCode);
        return 0;
    }
    const char *path_env = std::getenv("PATH");
    std::string new_path = std::string(path_env ? path_env : "") + ":" + "./";
    setenv("PATH", new_path.c_str(), 1);
    while (true) {
        std::string prompt = displayPrompt();
        char* input = readline(prompt.c_str());

        std::string line(input);
        free(input);

        if (!line.empty()) {
            add_history(line.c_str());
            parseCommand(line, lastExitCode);
        }
    }

    return 0;
}
