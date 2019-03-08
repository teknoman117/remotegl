cmake_minimum_required(VERSION 3.8)

add_executable(server server.cpp)
target_link_libraries(server remotegl-server glfw)