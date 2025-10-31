cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(SFML COMPONENTS System Window Graphics Audio REQUIRED)
find_package(ImGui-SFML CONFIG REQUIRED)

add_executable(main 
    src/main.cpp
)

target_compile_features(main PRIVATE cxx_std_17)

target_link_libraries(main
    #PRIVATE SFML::Network
    PRIVATE SFML::System
    PRIVATE SFML::Window
    PRIVATE SFML::Graphics
    PRIVATE SFML::Audio
    PRIVATE ImGui-SFML::ImGui-SFML
)


# --- Copy catlaughing.jpg to the build directory ---
#add_custom_command(TARGET main POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy
#    ${CMAKE_SOURCE_DIR}/res/catlaughing.jpg
#    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/catlaughing.jpg
#)