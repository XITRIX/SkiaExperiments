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
set(PROJECT_ICON ${PROJECT_RESOURCES}/demo_icon.jpg)


set(USE_LIBROMFS ON)

if (USE_LIBROMFS)
    add_libromfs(${PROJECT_NAME} ${PROJECT_RESOURCES})
endif ()

check_libromfs_generator()

if (PLATFORM_MAC)
    add_definitions(-DSK_GANESH -DSK_GL -DSK_DEBUG -DSK_TRIVIAL_ABI=[[clang::trivial_abi]])
elseif (PLATFORM_IOS)
    add_definitions(-DENABLE_ARC=OFF -DSK_GANESH -DSK_GL -DSK_DEBUG -DSK_TRIVIAL_ABI=[[clang::trivial_abi]])
elseif (PLATFORM_SWITCH)
    add_definitions(-DSK_GANESH -DSK_GL -DSK_DEBUG -DSK_TRIVIAL_ABI=[[clang::trivial_abi]])
endif ()

add_executable(SkiaTest
        main.cpp
        SDLApplication.cpp
        SDLApplication.hpp
        "${CMAKE_SOURCE_DIR}/Thirdparty/glad/glad.c"
#        TestApplication.cpp
#        TestApplication.hpp
#        "${CMAKE_SOURCE_DIR}/Thirdparty/skia/tools/sk_app/CommandSet.cpp"
#        "${CMAKE_SOURCE_DIR}/Thirdparty/skia/tools/sk_app/Window.cpp"
)

if (PLATFORM_MAC)
#    target_sources(SkiaTest PRIVATE
#            "${CMAKE_SOURCE_DIR}/Thirdparty/skia/tools/sk_app/mac/main_mac.mm"
#            "${CMAKE_SOURCE_DIR}/Thirdparty/skia/tools/sk_app/mac/Window_mac.mm"
#    )

    target_compile_definitions(SkiaTest PUBLIC PLATFORM_MAC)
    target_link_directories(SkiaTest PUBLIC "${CMAKE_SOURCE_DIR}/Thirdparty/skia/out/Angle-AppleSilicon")
    target_link_libraries(SkiaTest EGL)
    target_link_libraries(SkiaTest GLESv2)

    target_link_libraries(SkiaTest "${CMAKE_SOURCE_DIR}/Thirdparty/skia/out/Angle-AppleSilicon/libskia.a")
    target_include_directories(SkiaTest PRIVATE "${CMAKE_SOURCE_DIR}/Thirdparty/skia/out/Angle-AppleSilicon/gen")
elseif (PLATFORM_IOS)
    target_compile_definitions(SkiaTest PUBLIC PLATFORM_IOS)
    ios_bundle(
            "Platforms/ios/tvos/Splash.storyboard"
            "Platforms/ios/iphoneos/Splash.storyboard"
            "Platforms/ios/Images.xcassets"
            "Platforms/ios/iOSBundleInfo.plist.in"
            "Skia_Test"
            "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_ALTER}")

#    target_sources(SkiaTest PRIVATE
#            "${CMAKE_SOURCE_DIR}/Thirdparty/skia/tools/sk_app/ios/main_ios.mm"
#            "${CMAKE_SOURCE_DIR}/Thirdparty/skia/tools/sk_app/ios/Window_ios.mm"
#    )

    target_link_libraries(SkiaTest "${CMAKE_SOURCE_DIR}/Thirdparty/skia/out/ios64/libskia.a")
    target_include_directories(SkiaTest PRIVATE "${CMAKE_SOURCE_DIR}/Thirdparty/skia/out/ios64/gen")
elseif (PLATFORM_SWITCH)
    target_compile_definitions(SkiaTest PUBLIC PLATFORM_SWITCH)
    add_custom_target(${PROJECT_NAME}.nro DEPENDS ${PROJECT_NAME}
            COMMAND ${NX_NACPTOOL_EXE} --create "${PROJECT_NAME}" "${PROJECT_AUTHOR}" "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_ALTER}" ${PROJECT_NAME}.nacp --titleid=${PROJECT_TITLEID}
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_RESOURCES} ${CMAKE_BINARY_DIR}/resources
            #            COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/resources/font
            COMMAND ${NX_ELF2NRO_EXE} ${PROJECT_NAME}.elf ${PROJECT_NAME}.nro --icon=${PROJECT_ICON} --nacp=${PROJECT_NAME}.nacp
    )

    target_link_libraries(${PROJECT_NAME} nx png webp expat freetype turbojpeg z bz2 glad egl glapi drm_nouveau)
    target_link_libraries(SkiaTest "${CMAKE_SOURCE_DIR}/Thirdparty/skia/out/Horizon/libskia.a")
    target_include_directories(SkiaTest PRIVATE "${CMAKE_SOURCE_DIR}/Thirdparty/skia/out/Horizon/gen")
endif ()

target_include_directories(SkiaTest PRIVATE "${CMAKE_SOURCE_DIR}/Thirdparty/skia")
target_include_directories(SkiaTest PRIVATE "${CMAKE_SOURCE_DIR}/Thirdparty")

if (PLATFORM_MAC OR PLATFORM_IOS)
    target_link_libraries(SkiaTest "-framework CoreServices")
    target_link_libraries(SkiaTest "-framework CoreFoundation")
    target_link_libraries(SkiaTest "-framework CoreGraphics")
    target_link_libraries(SkiaTest "-framework CoreText")
    target_link_libraries(SkiaTest "-framework QuartzCore")
    target_link_libraries(SkiaTest "-framework Metal")
endif ()

if (PLATFORM_MAC)
    target_link_libraries(SkiaTest "-framework OpenGL")
    target_link_libraries(SkiaTest "-framework AppKit")
elseif (PLATFORM_IOS)
    target_link_libraries(SkiaTest "-framework OpenGLES")
    target_link_libraries(SkiaTest "-framework UIKit")
endif ()

set(SDL_SHARED OFF)
set(SDL_STATIC ON)

if (PLATFORM_MAC OR PLATFORM_IOS)
    add_subdirectory(${CMAKE_SOURCE_DIR}/Thirdparty/SDL EXCLUDE_FROM_ALL)
    #find_library(SDL2 NAMES SDL2-static SDL2main)
    target_include_directories(SkiaTest PRIVATE
        "${CMAKE_SOURCE_DIR}/Thirdparty/SDL/include"
        "${CMAKE_SOURCE_DIR}/Thirdparty/SDL/include/SDL2"
    )
    target_link_libraries(SkiaTest SDL2-static SDL2main)
else ()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    message(STATUS "SDL2 Path: \"${SDL2_INCLUDE_DIRS}\"")
    target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_INCLUDE_DIRS})
endif ()


# libromfs
if(USE_LIBROMFS)
    add_subdirectory(Thirdparty/libromfs EXCLUDE_FROM_ALL)
    #    list(APPEND BRLS_PLATFORM_LIBS ${LIBROMFS_LIBRARY})
    target_link_libraries(${PROJECT_NAME} ${LIBROMFS_LIBRARY})
endif()