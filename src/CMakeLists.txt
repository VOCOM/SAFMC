# Add Source to Executable
add_executable(SAFMC "SAFMC.cpp" "DroneComputer.cpp" "Utility.cpp")

# Link Library to Executable
target_link_libraries(SAFMC MAVSDK::mavsdk)

# Install Executable
install(TARGETS SAFMC DESTINATION ${CMAKE_BINARY_DIR})