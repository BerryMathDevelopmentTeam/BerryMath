cmake_minimum_required(VERSION 3.9)
project(tests)

set(CMAKE_CXX_STANDARD 17)

include_directories(
        /usr/local/BM/dev/include
        /usr/local/BM/dev/
        /usr/local/BM/dev/OSPlatformUtil/src/osplatformutil.h
        /usr/local/include
)
LINK_DIRECTORIES(/usr/local/BM/dev/lib/ /usr/local/lib/)
add_executable(tests main.cpp lexerTest.cpp astTest.cpp)
TARGET_LINK_LIBRARIES(tests BerryMath gtest)