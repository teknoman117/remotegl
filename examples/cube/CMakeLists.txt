cmake_minimum_required(VERSION 3.8)

add_library(cube-core OBJECT main.c renderer.c util/esTransform.c util/timespec.c)
target_include_directories(cube-core PRIVATE ${PROJECT_SOURCE_DIR}/include)

add_executable(cube $<TARGET_OBJECTS:cube-core>)
target_link_libraries(cube remotegl-client)

add_executable(cube-fake $<TARGET_OBJECTS:cube-core>)
target_link_libraries(cube-fake remotegl-client-fake)