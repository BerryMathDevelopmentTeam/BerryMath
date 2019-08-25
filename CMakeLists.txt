cmake_minimum_required(VERSION 3.9)
project(BerryMath)

option(SHARED "Set compile option: shared" OFF)

include_directories(
        include
)

AUX_SOURCE_DIRECTORY(src DIR_SRCS)

set(CMAKE_CXX_STANDARD 17)

if (SHARED)
    message("shared")
    add_library(BerryMath SHARED
            src/BerryMath.cpp include/BerryMath.h
            src/value.cpp include/value.h
            src/lex.cpp include/lex.h src/ast.cpp include/ast.h src/dylib.cpp include/dylib.h src/interpreter.cpp include/interpreter.h src/compiler.cpp include/compiler.h
            src/vm.cpp include/vm.h include/types.h src/register.cpp include/register.h OSPlatformUtil/src/osplatformutil.h)
else ()
    message("unshared")
    add_executable(
            BerryMath main.cpp
            src/BerryMath.cpp include/BerryMath.h
            src/value.cpp include/value.h
            src/lex.cpp include/lex.h src/ast.cpp include/ast.h src/dylib.cpp include/dylib.h src/interpreter.cpp include/interpreter.h src/compiler.cpp include/compiler.h src/vm.cpp include/vm.h include/types.h src/register.cpp include/register.h OSPlatformUtil/src/osplatformutil.h src/sym.cpp include/sym.h)

    install(TARGETS BerryMath
            CONFIGURATIONS Release
            RUNTIME DESTINATION /usr/local/bin)

    INSTALL(TARGETS BerryMath
            RUNTIME DESTINATION bin
            )
endif ()