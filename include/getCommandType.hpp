#ifndef GET_COMMAND_TYPE_INCLUDE
#define GET_COMMAND_TYPE_INCLUDE

#include <string>

enum Command {
    EXIT, SCRIPT, CD, UNKNOWN, ECHO, PWD, ERRNO, EXPORT
};

Command getCommandType(const std::string& cmd);

#endif // GET_COMMAND_TYPE_INCLUDE
