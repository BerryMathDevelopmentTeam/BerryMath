cmake_minimum_required(VERSION 3.9)
project(BerryMath)

include_directories(
        include
)

AUX_SOURCE_DIRECTORY(src DIR_SRCS)

set(CMAKE_CXX_STANDARD 17)

add_executable(
        BerryMath main.cpp
        src/BerryMath.cpp include/BerryMath.h
        src/value.cpp include/value.h
        src/lex.cpp include/lex.h)

install(TARGETS BerryMath
        CONFIGURATIONS Release
        RUNTIME DESTINATION /usr/local/bin)

INSTALL(TARGETS BerryMath
        RUNTIME DESTINATION bin
        )