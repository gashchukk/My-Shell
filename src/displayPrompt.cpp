#include <iostream>
#include <unistd.h>

#include "displayPrompt.hpp"

std::string displayPrompt() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        return std::string(cwd) + " $ ";
    }
    return "$ ";
}
