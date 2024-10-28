cmake_minimum_required(VERSION 3.14)
project(IMGUI)

file(GLOB SRC "src/*.cpp" "include/*.h")
file(GLOB IMPL "backends/*_glfw.*" "backends/*_opengl3.*")
include_directories("../glfw/include")

add_library(imgui ${SRC} ${IMPL})
target_include_directories(imgui PUBLIC include/ backends/)