cmake_minimum_required(VERSION 3.9)
project(BerryMath)

include_directories(
        include
        jsoncpp-src-0.5.0/include/json/
)

AUX_SOURCE_DIRECTORY(src DIR_SRCS)

set(CMAKE_CXX_STANDARD 17)

add_executable(
        BerryMath main.cpp
        include/BerryMath.h src/BerryMath.cpp
        include/memory.h src/memory.cpp
        include/btype.h src/btype.cpp include/color.h src/lex.cpp include/lex.h src/AST.cpp include/AST.h src/script.cpp include/script.h src/eval.cpp include/eval.h)

# /usr/local/lib/libjsoncpp.dylib
link_directories(/usr/local/lib)
target_link_libraries(BerryMath /usr/local/lib/libjsoncpp.dylib)

install(TARGETS BerryMath
        CONFIGURATIONS Release
        RUNTIME DESTINATION /usr/local/bin)

INSTALL(TARGETS BerryMath
        RUNTIME DESTINATION bin
        )