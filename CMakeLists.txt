cmake_minimum_required(VERSION 3.10)
project(SokobanGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2.5 COMPONENTS graphics window system audio REQUIRED)

file(GLOB SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_executable(sokoban ${SOURCES})
target_include_directories(sokoban PRIVATE include)
target_link_libraries(sokoban PRIVATE sfml-graphics sfml-window sfml-system sfml-audio)

target_compile_definitions(sokoban PRIVATE
    SOKOBAN_ASSET_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets"
    SOKOBAN_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/data"
    SOKOBAN_PROJECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)
