#include <string>

#include "getCommandType.hpp"

Command getCommandType(const std::string& cmd) {
    if (cmd == "mexit") return EXIT;
    if (cmd == ".") return SCRIPT;
    if (cmd == "mcd") return CD;
    if (cmd == "mecho") return ECHO;
    if (cmd == "mpwd") return PWD;
    if (cmd == "merrno") return ERRNO;
    if (cmd == "mexport") return EXPORT;
    return UNKNOWN;
}
