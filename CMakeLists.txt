include(${CMAKE_CURRENT_SOURCE_DIR}/Thirdparty/cmake/toolchain.cmake)
cmake_minimum_required(VERSION 3.24)

project(SkiaTest)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(VERSION_MAJOR "1")
set(VERSION_MINOR "0")
set(VERSION_ALTER "0")
set(VERSION_BUILD "0")
set(PACKAGE_NAME "ru.xitrix.skiatest")
set(PROJECT_AUTHOR "XITRIX")
set(PROJECT_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/res)

add_executable(SkiaTest
#        main.cpp
#        SDLApplication.cpp
#        SDLApplication.hpp
        TestApplication.cpp
        TestApplication.hpp
        "${CMAKE_SOURCE_DIR}/Thirdparty/skia/tools/sk_app/CommandSet.cpp"
        "${CMAKE_SOURCE_DIR}/Thirdparty/skia/tools/sk_app/Window.cpp"
)

add_definitions(-DSK_GANESH -DSK_GL -DSK_DEBUG -DSK_TRIVIAL_ABI=[[clang::trivial_abi]])

if (PLATFORM_MAC)
    target_sources(SkiaTest PRIVATE
            "${CMAKE_SOURCE_DIR}/Thirdparty/skia/tools/sk_app/mac/main_mac.mm"
            "${CMAKE_SOURCE_DIR}/Thirdparty/skia/tools/sk_app/mac/Window_mac.mm"
    )

    target_link_libraries(SkiaTest "${CMAKE_SOURCE_DIR}/Thirdparty/skia/out/AppleSilicon/libskia.a")
    target_link_libraries(SkiaTest "${CMAKE_SOURCE_DIR}/Thirdparty/skia/out/AppleSilicon/libwindow.a")
    target_include_directories(SkiaTest PRIVATE "${CMAKE_SOURCE_DIR}/Thirdparty/skia/out/AppleSilicon/gen")
elseif (PLATFORM_IOS)
    ios_bundle(
            "Platforms/ios/tvos/Splash.storyboard"
            "Platforms/ios/iphoneos/Splash.storyboard"
            "Platforms/ios/Images.xcassets"
            "Platforms/ios/iOSBundleInfo.plist.in"
            "Skia_Test"
            "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_ALTER}")

    target_sources(SkiaTest PRIVATE
            "${CMAKE_SOURCE_DIR}/Thirdparty/skia/tools/sk_app/ios/main_ios.mm"
            "${CMAKE_SOURCE_DIR}/Thirdparty/skia/tools/sk_app/ios/Window_ios.mm"
    )

    target_link_libraries(SkiaTest "${CMAKE_SOURCE_DIR}/Thirdparty/skia/out/ios64/libskia.a")
    target_link_libraries(SkiaTest "${CMAKE_SOURCE_DIR}/Thirdparty/skia/out/ios64/libwindow.a")
    target_include_directories(SkiaTest PRIVATE "${CMAKE_SOURCE_DIR}/Thirdparty/skia/out/ios64/gen")

endif ()

target_include_directories(SkiaTest PRIVATE "${CMAKE_SOURCE_DIR}/Thirdparty/skia")

target_link_libraries(SkiaTest "-framework CoreServices")
target_link_libraries(SkiaTest "-framework CoreFoundation")
target_link_libraries(SkiaTest "-framework CoreGraphics")
target_link_libraries(SkiaTest "-framework CoreText")
target_link_libraries(SkiaTest "-framework QuartzCore")
target_link_libraries(SkiaTest "-framework Metal")

if (PLATFORM_MAC)
    target_link_libraries(SkiaTest "-framework OpenGL")
    target_link_libraries(SkiaTest "-framework AppKit")
elseif (PLATFORM_IOS)
    target_link_libraries(SkiaTest "-framework OpenGLES")
    target_link_libraries(SkiaTest "-framework UIKit")
endif ()

#find_package(SDL2 REQUIRED)
#target_include_directories(SkiaTest PRIVATE ${SDL2_INCLUDE_DIRS})
#target_link_libraries(SkiaTest ${SDL2_LIBRARIES})
