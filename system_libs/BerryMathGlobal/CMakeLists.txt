cmake_minimum_required(VERSION 3.9)
project(BerryMathGlobal)

set(CMAKE_CXX_STANDARD 11)

set(BERRYMATH_DIR ../..)
set(INCLUDE_DIR ../../include)
set(SRC_DIR ../../src)

include_directories(
        ${INCLUDE_DIR}
        ${BERRYMATH_DIR}/jsoncpp-src-0.5.0/include/json/
)
AUX_SOURCE_DIRECTORY(${SRC_DIR} DIR_SRCS)

add_library(
        BerryMathGlobal SHARED library.cpp library.h
        include/BerryMath.h src/BerryMath.cpp
        include/memory.h src/memory.cpp
        include/btype.h src/btype.cpp include/color.h src/lex.cpp include/lex.h src/AST.cpp include/AST.h src/script.cpp include/script.h include/stringpp.h src/stringpp.cpp src/color.cpp src/version.cpp include/version.h
)

link_directories(/usr/local/lib)
target_link_libraries(BerryMathGlobal /usr/local/lib/libjsoncpp.dylib)