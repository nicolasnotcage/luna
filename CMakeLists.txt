cmake_minimum_required(VERSION 3.16)
project(ChessLab VERSION 1.0.0)

# Platform detection
if(WIN32)
    set(BUILD_MS_WINDOWS 1)
    add_definitions(-DBUILD_WINDOWS)
    set(WINDOW_ARCH "x64" CACHE STRING "Windows Architecture")
    set_property(CACHE WINDOW_ARCH PROPERTY STRINGS "x64;x86;arm64")
elseif(APPLE)
    set(BUILD_MACOS 1)
    add_definitions(-DBUILD_MACOS)
elseif(UNIX AND NOT APPLE)
    set(BUILD_LINUX 1)
    add_definitions(-DBUILD_LINUX)
endif()

# C/C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Directory definitions
if(BUILD_MS_WINDOWS)
    set(SRC_DIR_WIN \"$(ProjectDir)..\")
    set(RESOURCE_DIR_WIN \"$(ProjectDir)..\\resources\")
    add_definitions(-DSRC_DIR=${SRC_DIR_WIN})
    add_definitions(-DRESOURCE_DIR=${RESOURCE_DIR_WIN})
else()
    add_definitions(-DSRC_DIR=${CMAKE_SOURCE_DIR})
    add_definitions(-DRESOURCE_DIR=${CMAKE_SOURCE_DIR}/resources)
endif()

# Global include directories
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/include)

# Library detection
if(BUILD_MS_WINDOWS)
    # SDL3 Library
    unset(SDL_LIB CACHE)
    set(SDL_PATH ${CMAKE_SOURCE_DIR}/lib/win/SDL3/${WINDOW_ARCH})
    find_library(SDL_LIB NAMES SDL3 PATHS ${SDL_PATH} NO_DEFAULT_PATH)
    
    # FMOD Library
    unset(FMOD_LIB CACHE)
    set(FMOD_PATH ${CMAKE_SOURCE_DIR}/lib/win/fmod/${WINDOW_ARCH})
    find_library(FMOD_LIB 
        NAMES 
            fmod_vc 
            fmod64_vc 
            fmod 
            fmodex_vc
            fmodex64_vc
        PATHS ${FMOD_PATH}
        NO_DEFAULT_PATH
    )
    
    if(NOT FMOD_LIB)
        set(FMOD_PATH ${CMAKE_SOURCE_DIR}/lib/win/fmod)
        find_library(FMOD_LIB 
            NAMES 
                fmod_vc 
                fmod64_vc 
                fmod 
                fmodex_vc
                fmodex64_vc
            PATHS ${FMOD_PATH}
            NO_DEFAULT_PATH
        )
    endif()
    
elseif(BUILD_MACOS)
    find_library(SDL_LIB NAMES SDL3 PATHS ${CMAKE_SOURCE_DIR}/lib/macos/SDL3)
    find_library(FMOD_LIB NAMES fmod PATHS ${CMAKE_SOURCE_DIR}/lib/macos/fmod)
    
elseif(BUILD_LINUX)
    find_library(SDL_LIB NAMES SDL3 PATHS ${CMAKE_SOURCE_DIR}/lib/linux/SDL3)
    find_library(FMOD_LIB NAMES fmod PATHS ${CMAKE_SOURCE_DIR}/lib/linux/fmod)
    find_library(PTHREAD_LIBRARY NAMES pthread)
endif()

# Verify required libraries
if(NOT SDL_LIB)
    message(FATAL_ERROR "SDL3 library not found! Check your library paths.")
endif()

if(NOT FMOD_LIB)
    message(WARNING "FMOD library not found! Audio features may not work properly.")
endif()

# Add subdirectories for each project component
add_subdirectory(src/ChessRules)
add_subdirectory(src/ChessEngine) 
add_subdirectory(src/GameEngine)