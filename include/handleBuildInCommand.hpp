#ifndef HANDLE_BUILD_IN_COMMANDS
#define HANDLE_BUILD_IN_COMMANDS

#include <vector>
#include <string>


bool handleBuildInCommand(const std::vector<std::string>& tokens, int &lastExitCode,
                           int outFd);
#endif // HANDLE_BUILD_IN_COMMANDS
