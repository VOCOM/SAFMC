# CMakeList.txt : CMake project for SAFMC, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("SAFMC")

# Specify at least C++11, better C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable strict handling of warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Finds MAVSDK when installed system wide.
find_package(MAVSDK REQUIRED)

# Add Header files
include_directories(inc)

# Add Sub-directories
add_subdirectory(src)
add_subdirectory(lib)

if (CMAKE_VERSION VERSION_GREATER 3.12)
    set_property(TARGET SAFMC PROPERTY CXX_STANDARD 20)
endif()

# Install config file
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.txt ${CMAKE_BINARY_DIR} COPYONLY)