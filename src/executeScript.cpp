#include <fstream>
#include <string>

#include "executeScript.hpp"
#include "parseCommand.hpp"

void executeScript(const std::string& filename, int &lastExitCode) {
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        parseCommand(line, lastExitCode);
    }
}
