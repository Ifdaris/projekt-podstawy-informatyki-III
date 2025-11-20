cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(SFML COMPONENTS System Window Graphics Audio REQUIRED)
find_package(ImGui-SFML CONFIG REQUIRED)
find_package(Threads REQUIRED)

# --- EXECUTABLE 1: Multi-Threaded (main.cpp) ---
add_executable(MultiThreadedApp src/main.cpp)

target_compile_features(MultiThreadedApp PRIVATE cxx_std_17)

target_link_libraries(MultiThreadedApp
    PRIVATE SFML::System
    PRIVATE SFML::Window
    PRIVATE SFML::Graphics
    PRIVATE SFML::Audio
    PRIVATE ImGui-SFML::ImGui-SFML
    PRIVATE Threads::Threads
)

# --- EXECUTABLE 2: Single-Threaded (main2.cpp) ---
add_executable(SingleThreadedApp src/main2.cpp)

target_compile_features(SingleThreadedApp PRIVATE cxx_std_17)

target_link_libraries(SingleThreadedApp
    PRIVATE SFML::System
    PRIVATE SFML::Window
    PRIVATE SFML::Graphics
    PRIVATE SFML::Audio
    PRIVATE ImGui-SFML::ImGui-SFML
    PRIVATE Threads::Threads
)

# --- RESOURCE COPYING ---
# You only need to attach this to one of them if they output to the same folder
add_custom_command(TARGET MultiThreadedApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/res"
    "$<TARGET_FILE_DIR:MultiThreadedApp>/res"
)