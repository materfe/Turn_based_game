cmake_minimum_required(VERSION 3.2)
project(PowerOf4)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(externals/SFML)
add_subdirectory(game_api)
set(IMGUI_SFML_FIND_SFML OFF)
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/externals/imgui/" CACHE STRING "")
add_subdirectory(externals/imgui-sfml)

add_executable(server main/server.cc)
target_include_directories(server PRIVATE externals/SFML/include include)
target_link_libraries(server PRIVATE sfml-network)
target_link_libraries(server PRIVATE api)

add_executable(client main/client.cc)
target_include_directories(client PRIVATE externals/SFML/include include externals/imgui-sfml externals/imgui)
target_link_libraries(client PRIVATE sfml-network sfml-graphics ImGui-SFML)
target_link_libraries(client PRIVATE api)