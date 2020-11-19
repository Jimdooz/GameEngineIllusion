cmake_minimum_required (VERSION 3.6)
project (Glad)


include_directories(
./include/
)

add_library(Glad
src/glad.c
./include/glad/glad.h
./include/KHR/khrplatform.h
)