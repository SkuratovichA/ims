cmake_minimum_required(VERSION 3.10)
project(ims)

# set the c++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(THIRD_PARTY_DIR "${CMAKE_SOURCE_DIR}/third_party")
set(SIMLIB_BIN_DIR "${THIRD_PARTY_DIR}/simlib/bin")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# include directories for the header files
include_directories(src "${SIMLIB_BIN_DIR}/include")

# add all source files from the src directory
file(GLOB SOURCES "src/*.cpp")

# define the executable with the collected source files
add_executable(${PROJECT_NAME} ${SOURCES})

# link the static library
target_link_libraries(${PROJECT_NAME} "${SIMLIB_BIN_DIR}/lib/libsimlib.a")

