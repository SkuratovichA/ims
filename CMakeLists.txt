cmake_minimum_required(VERSION 3.26)
project(ims)

set(CMAKE_CXX_STANDARD 14)

# Add the header include directory
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add the library directory
link_directories(${CMAKE_SOURCE_DIR}/lib)

# Add your executable
add_executable(ims src/main.cpp)

# For static linking, use the static library directly:
target_link_libraries(ims ${CMAKE_SOURCE_DIR}/lib/libsimlib.a)

# For dynamic linking (shared libraries): TODO: make this possible
#target_link_libraries(ims simlib)

# Set RPATH so that the executable finds shared libraries at runtime
set(CMAKE_INSTALL_RPATH "${CMAKE_SOURCE_DIR}/lib")
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

# Ensure the linker can find shared libraries during build
set(CMAKE_SKIP_BUILD_RPATH FALSE)

# Ensure RPATH is used when installing
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
