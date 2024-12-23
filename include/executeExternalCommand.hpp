#ifndef EXECUTE_EXTERNAL_COMMAND_INCLUDE
#define EXECUTE_EXTERNAL_COMMAND_INCLUDE

#include <vector>
#include <string>

void executeExternalCommand(const std::vector<std::string>& tokens, int &lastExitCode,
                            int outFd, int errFd, int inFd);

#endif // EXECUTE_EXTERNAL_COMMAND_INCLUDE
