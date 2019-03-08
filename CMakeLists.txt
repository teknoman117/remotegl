cmake_minimum_required(VERSION 3.8)
project(remotegl VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_EXAMPLES "build example projects" ON)

# client lib
add_library(remotegl-client
    src/remotegl/remotegl-client.cpp
)

add_library(remotegl::remotegl-client ALIAS remotegl-client)

target_include_directories(remotegl-client
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

# fake lib
add_library(remotegl-client-fake src/remotegl/remotegl-client-fake.cpp)
add_library(remotegl::remotegl-client-fake ALIAS remotegl-client-fake)

target_include_directories(remotegl-client-fake
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

target_link_libraries(remotegl-client-fake PRIVATE GL GLEW glfw)

# server lib
add_library(remotegl-server
    src/remotegl/remotegl-server.cpp
)

add_library(remotegl::remotegl-server ALIAS remotegl-server)

target_include_directories(remotegl-server
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

target_link_libraries(remotegl-server PRIVATE GL GLEW)

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()