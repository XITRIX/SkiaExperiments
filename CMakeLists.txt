cmake_minimum_required(VERSION 3.26)
project(SkiaTest)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

#include_directories(Thirdparty/skia/include)
#link_directories(Thirdparty/skia/out/AppleSilicon)

#add_library(Skia STATIC IMPORTED)
#set_target_properties(Skia PROPERTIES
#        IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/Thirdparty/skia/out/AppleSilicon/libskia.a"
#        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/Thirdparty/skia/include"
#)

add_executable(SkiaTest
        main.cpp
        SDLApplication.cpp
        SDLApplication.hpp
#        TestApplication.cpp
#        TestApplication.hpp
#        "${CMAKE_SOURCE_DIR}/Thirdparty/skia/tools/sk_app/CommandSet.cpp"
#        "${CMAKE_SOURCE_DIR}/Thirdparty/skia/tools/sk_app/Window.cpp"
#        "${CMAKE_SOURCE_DIR}/Thirdparty/skia/tools/sk_app/mac/main_mac.mm"
#        "${CMAKE_SOURCE_DIR}/Thirdparty/skia/tools/sk_app/mac/Window_mac.mm"

)

add_definitions(-DSK_GANESH -DSK_METAL -DSK_DEBUG -DSK_TRIVIAL_ABI=[[clang::trivial_abi]])
#target_link_libraries(SkiaTest Skia)

target_link_libraries(SkiaTest "${CMAKE_SOURCE_DIR}/Thirdparty/skia/out/AppleSilicon/libskia.a")
target_link_libraries(SkiaTest "${CMAKE_SOURCE_DIR}/Thirdparty/skia/out/AppleSilicon/libwindow.a")
target_include_directories(SkiaTest PRIVATE "${CMAKE_SOURCE_DIR}/Thirdparty/skia/out/AppleSilicon/gen")
target_include_directories(SkiaTest PRIVATE "${CMAKE_SOURCE_DIR}/Thirdparty/skia")

target_link_libraries(SkiaTest "-framework CoreServices")
target_link_libraries(SkiaTest "-framework CoreFoundation")
target_link_libraries(SkiaTest "-framework CoreGraphics")
target_link_libraries(SkiaTest "-framework CoreText")
target_link_libraries(SkiaTest "-framework QuartzCore")
target_link_libraries(SkiaTest "-framework Metal")
target_link_libraries(SkiaTest "-framework OpenGL")
target_link_libraries(SkiaTest "-framework AppKit")

find_package(SDL2 REQUIRED)
target_include_directories(SkiaTest PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(SkiaTest ${SDL2_LIBRARIES})
