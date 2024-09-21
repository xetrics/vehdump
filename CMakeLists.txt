cmake_minimum_required(VERSION 3.5.0)
project(VEHDump VERSION 0.1.0 LANGUAGES C CXX)

add_executable(VEHDump main.cpp ntdll.h process.h math.h)

