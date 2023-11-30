cmake_minimum_required(VERSION 3.10)
project(ims)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories for the header files
include_directories(src third_party/simlib/bin/include)

# Add all source files from the src directory
file(GLOB SOURCES "src/*.cpp")

# Define the executable with the collected source files
add_executable(${PROJECT_NAME} ${SOURCES})

# Link the static library
target_link_libraries(${PROJECT_NAME} "${CMAKE_SOURCE_DIR}/third-parties/simlib/bin/lib/libsimlib.a")
