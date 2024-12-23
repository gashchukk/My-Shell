cmake_minimum_required(VERSION 3.15)

set(PROJECT_NAME myshell2)
project(${PROJECT_NAME} C CXX)

set(CMAKE_CXX_STANDARD 20)

set(WARNINGS_AS_ERRORS OFF)

set(ENABLE_PVS_STUDIO OFF)

set(ENABLE_UBSan OFF)
set(ENABLE_ASAN OFF)
set(ENABLE_TSan OFF)
set(ENABLE_MSAN OFF)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

include(cmake/CompilerWarnings.cmake)


add_executable(${PROJECT_NAME} main.cpp include/displayPrompt.hpp include/executeExternalCommand.hpp include/executeScript.hpp include/getCommandType.hpp include/handleBuildInCommand.hpp include/parseCommand.hpp src/displayPrompt.cpp src/executeExternalCommand.cpp src/executeScript.cpp src/getCommandType.cpp src/handleBuildInCommand.cpp src/parseCommand.cpp)

find_package(Boost 1.71.0 COMPONENTS program_options system REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::program_options Boost::system)

target_include_directories(${PROJECT_NAME} PRIVATE src)
target_include_directories(${PROJECT_NAME} PRIVATE include)

include_directories(/opt/homebrew/opt/readline/include)
link_directories(/opt/homebrew/opt/readline/lib)
target_link_libraries(${PROJECT_NAME} PRIVATE readline)

install(TARGETS ${PROJECT_NAME} DESTINATION ${MYSHELL_INSTALL_DIR})

INSTALL(PROGRAMS
    $<TARGET_FILE:${PROJECT_NAME}>
    DESTINATION ./)

set(ALL_TARGETS ${PROJECT_NAME})

include(cmake/main-config.cmake)