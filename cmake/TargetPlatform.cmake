# Include BEFORE project().
#
# Windows host: native build with whatever compiler CMake finds.
# Linux host:   native Linux build by default, or a Windows (MinGW-w64) cross build with
#               -DTARGET_WINDOWS=ON  (optionally -DMINGW_TRIPLET=x86_64-w64-mingw32ucrt).

if(CMAKE_HOST_WIN32)
    message(STATUS "Windows host: building native Windows binary")
    return()
endif()

option(TARGET_WINDOWS "Cross-compile a Windows binary with MinGW-w64 (Linux host only)" OFF)
set(MINGW_TRIPLET "x86_64-w64-mingw32" CACHE STRING "MinGW-w64 toolchain prefix, e.g. x86_64-w64-mingw32 or x86_64-w64-mingw32ucrt")

if(NOT TARGET_WINDOWS)
    message(STATUS "Building native Linux binary")
    return()
endif()

find_program(MINGW_CXX NAMES ${MINGW_TRIPLET}-g++ ${MINGW_TRIPLET}-g++-posix ${MINGW_TRIPLET}-g++-win32)
find_program(MINGW_CC NAMES ${MINGW_TRIPLET}-gcc ${MINGW_TRIPLET}-gcc-posix ${MINGW_TRIPLET}-gcc-win32)
find_program(MINGW_RC NAMES ${MINGW_TRIPLET}-windres)

if(NOT MINGW_CXX OR NOT MINGW_CC)
    message(FATAL_ERROR "MinGW-w64 compiler '${MINGW_TRIPLET}-g++' not found. Install it or set -DMINGW_TRIPLET=<prefix>.")
endif()

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
set(CMAKE_C_COMPILER ${MINGW_CC})
set(CMAKE_CXX_COMPILER ${MINGW_CXX})
if(MINGW_RC)
    set(CMAKE_RC_COMPILER ${MINGW_RC})
endif()

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Make the .exe standalone (no libstdc++/libgcc DLLs needed next to it).
set(CMAKE_EXE_LINKER_FLAGS_INIT "-static -static-libgcc -static-libstdc++")

message(STATUS "Cross-compiling Windows binary using ${MINGW_CXX}")
