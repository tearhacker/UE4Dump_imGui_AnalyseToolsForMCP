cmake_minimum_required(VERSION 3.16)

# -----------------------------
# Options
# -----------------------------
option(BUILD_SHARED "Build shared linrary" OFF)
option(KT_DISABLE_KEYSTONE "Disable keystone support" ON)

project(UEDump3r
        VERSION 1.0
        LANGUAGES CXX)

# -----------------------------
# C++ standard
# -----------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(DEPS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../deps)

# -----------------------------
# KittyMemoryEx library
# -----------------------------
set(KITTYMEMORY_PATH "${DEPS_PATH}/KittyMemoryEx/KittyMemoryEx")
file(GLOB KITTYMEMORY_SRC "${KITTYMEMORY_PATH}/*.cpp")
set(KEYSTONE_LIB ${KITTYMEMORY_PATH}/Deps/Keystone/libs-android/${CMAKE_ANDROID_ARCH_ABI}/libkeystone.a)

# -----------------------------
# Project sources
# -----------------------------
file(GLOB UE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/UE/*.cpp)
set(PROJ_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Utils/BufferFmt.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Dumper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/UPackageGenerator.cpp
    ${UE_SRC}
    ${DEPS_PATH}/fmt/format.cc
    ${KITTYMEMORY_SRC}
)

if(BUILD_SHARED)
    add_library(UEDump3r SHARED
        ${PROJ_SRC}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/library.cpp)
else()
    add_executable(UEDump3r
        ${PROJ_SRC}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Utils/ProgressUtils.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/executable.cpp)
endif()

# -----------------------------
# Includes
# -----------------------------
target_include_directories(UEDump3r
    PRIVATE
        ${DEPS_PATH} ${KITTYMEMORY_PATH}
)

# -----------------------------
# Compile definitions
# -----------------------------
target_compile_definitions(UEDump3r
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
        $<$<NOT:$<BOOL:${BUILD_SHARED}>>:kEXECUTABLE>
        $<$<BOOL:${KT_DISABLE_KEYSTONE}>:kNO_KEYSTONE>
)

# -----------------------------
# Compile options
# -----------------------------
target_compile_options(UEDump3r
    PRIVATE
        $<$<CONFIG:Debug>:-O0 -g>
        $<$<CONFIG:Release>:-O2>
        -Wall
        -Wextra
        -Werror
        -fexceptions
        -Wno-unused-command-line-argument
)

# -----------------------------
# Platform-specific libraries
# -----------------------------
if(ANDROID)
    target_link_libraries(UEDump3r
    PRIVATE
        log
        $<$<NOT:$<BOOL:${KT_DISABLE_KEYSTONE}>>:${KEYSTONE_LIB}>
    )
endif()