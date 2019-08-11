cmake_minimum_required(VERSION 3.9)
project(os)

set(CMAKE_CXX_STANDARD 17)

include_directories(
        /usr/local/BM/dev/include
        /usr/local/BM/dev/
        /usr/local/BM/dev/OSPlatformUtil/src/osplatformutil.h
)
LINK_DIRECTORIES(/usr/local/BM/dev/lib/)
add_library(os SHARED library.cpp library.h)
TARGET_LINK_LIBRARIES(os BerryMath)