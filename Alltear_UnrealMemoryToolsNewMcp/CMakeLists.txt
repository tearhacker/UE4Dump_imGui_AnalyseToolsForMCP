cmake_minimum_required(VERSION 3.10)
set(NDK_PATH E:/ndk/android-ndk-r29)
set(CMAKE_TOOLCHAIN_FILE ${NDK_PATH}/build/cmake/android.toolchain.cmake)
set(CMAKE_SYSTEM_NAME Android)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_SYSTEM_VERSION 30)
set(ANDROID_PLATFORM 30)
set(ANDROID_ABI arm64-v8a)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/outputs/${ANDROID_ABI})
project(UnrealMemoryTools)

set(KITTYMEMORY_PATH KittyMemoryEx)
file(GLOB KITTYMEMORY_SRC ${KITTYMEMORY_PATH}/*.cpp)

set(DEPS_PATH deps)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -O0 -g -std=c++20 -fexceptions -DDEBUG -DkNO_KEYSTONE -w")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2 -s -std=c++20 -fexceptions -DNDEBUG -DkNO_KEYSTONE -w ")

file(GLOB UE_SRC src/UE/*.cpp)
file(GLOB UE_PROFILE_SRC src/UE/UEGameProfiles/*.cpp)
file(GLOB AUTOFIX_SRC src/AutoFix/*.cpp)
file(GLOB GUI_SRC
        src/GUI/Android_draw/*.cpp
        src/GUI/Android_Graphics/*.cpp
        src/GUI/Android_my_imgui/*.cpp
        src/GUI/Android_touch/*.cpp
        src/GUI/ImGui/*.cpp
        src/GUI/ImGui/backends/*.cpp
        src/GUI/ImGui/misc/freetype/*.cpp
        src/GUI/My_Utils/*.cpp)

include_directories(
        ${DEPS_PATH}
        ${KITTYMEMORY_PATH}
        src/GUI
        src/GUI/Android_draw
        src/GUI/Android_Graphics
        src/GUI/Android_my_imgui
        src/GUI/Android_touch
        src/GUI/ImGui
        src/GUI/ImGui/backends
        src/GUI/ImGui/misc/freetype
        src/GUI/ImGui/misc/git_freetype
        src/GUI/My_Utils
        src/GUI/native_surface)
link_libraries(-llog -landroid -lEGL -lGLESv3 -lvulkan -lz -ldl)

add_executable(UnrealMemoryTools ${UE_SRC} ${UE_PROFILE_SRC} ${AUTOFIX_SRC} ${GUI_SRC} src/executable.cpp src/Dumper.cpp src/UPackageGenerator.cpp src/SDKExplorer.cpp ${KITTYMEMORY_SRC} ${DEPS_PATH}/fmt/format.cc
        src/AutoFix/NativeFunctionResolver.cpp
        src/AutoFix/PropertyOffsetFinder.cpp
        src/AutoFix/StructLayout.cpp
        src/Utils/BufferFmt.cpp
        src/Utils/KittyCmdln.cpp
        src/Utils/ProgressUtils.cpp)

target_compile_definitions(UnrealMemoryTools PRIVATE
        kEXECUTABLE
        VK_USE_PLATFORM_ANDROID_KHR
        IMGUI_IMPL_VULKAN_NO_PROTOTYPES
        IMGUI_ENABLE_FREETYPE
        IMGUI_DISABLE_DEBUG_TOOLS)

target_link_libraries(UnrealMemoryTools PRIVATE
        ${CMAKE_SOURCE_DIR}/src/GUI/ImGui/misc/git_freetype/${ANDROID_ABI}/libfreetype.a)
