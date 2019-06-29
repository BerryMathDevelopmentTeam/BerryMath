cmake_minimum_required(VERSION 3.9)
project(BerryMath)

set(
        SHARED 0
)

include_directories(
        include
)

AUX_SOURCE_DIRECTORY(src DIR_SRCS)

set(CMAKE_CXX_STANDARD 17)

if (${SHARED})
    add_library(BerryMath SHARED
            include/BerryMath.h
            include/value.h
            include/lex.h
            include/ast.h
            include/dylib.h
            include/interpreter.h
            src/BerryMath.cpp
            src/value.cpp
            src/lex.cpp
            src/ast.cpp
            src/dylib.cpp
            src/interpreter.cpp
            )
else ()

    add_executable(
            BerryMath main.cpp
            src/BerryMath.cpp include/BerryMath.h
            src/value.cpp include/value.h
            src/lex.cpp include/lex.h src/ast.cpp include/ast.h src/dylib.cpp include/dylib.h src/interpreter.cpp include/interpreter.h)

    install(TARGETS BerryMath
            CONFIGURATIONS Release
            RUNTIME DESTINATION /usr/local/bin)

    INSTALL(TARGETS BerryMath
            RUNTIME DESTINATION bin
            )
endif ()