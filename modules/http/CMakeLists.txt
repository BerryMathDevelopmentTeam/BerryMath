cmake_minimum_required(VERSION 3.9)
project(http)

set(CMAKE_CXX_STANDARD 17)

include_directories(
        /usr/local/BM/dev/include
        /usr/local/BM/dev/
        /usr/local/BM/dev/OSPlatformUtil/src/osplatformutil.h
        HTTPRequest/include/
)
LINK_DIRECTORIES(/usr/local/BM/dev/lib/)
add_library(http SHARED library.cpp library.h)
TARGET_LINK_LIBRARIES(http BerryMath)