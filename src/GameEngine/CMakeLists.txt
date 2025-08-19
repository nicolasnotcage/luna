project(GameEngine)

# Collect all source files from subdirectories
file(GLOB_RECURSE GAME_ENGINE_SOURCES 
    "chess_game/*.cpp"
    "graph/*.cpp" 
    "image/*.cpp"
    "platform/*.cpp"
    "system/*.cpp"
    "menus/*.cpp"
    "main.cpp"
)

file(GLOB_RECURSE GAME_ENGINE_HEADERS
    "chess_game/*.hpp"
    "graph/*.hpp"
    "image/*.hpp" 
    "platform/*.hpp"
    "system/*.hpp"
    "menus/*.hpp"
)

# Create executable
add_executable(game_engine ${GAME_ENGINE_SOURCES} ${GAME_ENGINE_HEADERS})

# Include the GameEngine source directory so headers can be found
target_include_directories(game_engine PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

# Build library list for linking
set(LINK_LIBRARIES
    ${SDL_LIB}
    ${CMAKE_DL_LIBS}
    chess_rules
    chess_engine
)

# Add platform-specific libraries
if(BUILD_LINUX)
    list(APPEND LINK_LIBRARIES ${PTHREAD_LIBRARY})
endif()

# Add FMOD if found
if(FMOD_LIB)
    list(APPEND LINK_LIBRARIES ${FMOD_LIB})
endif()

# Link libraries
target_link_libraries(game_engine PRIVATE ${LINK_LIBRARIES})


# Windows-specific: Copy DLLs to output directory
if(BUILD_MS_WINDOWS)
    set(DLLS_TO_MOVE "")
    
    if(EXISTS ${SDL_PATH}/SDL3.dll)
        list(APPEND DLLS_TO_MOVE ${SDL_PATH}/SDL3.dll)
    endif()
    
    if(EXISTS ${FMOD_PATH}/fmod.dll)
        list(APPEND DLLS_TO_MOVE ${FMOD_PATH}/fmod.dll)
    endif()
    if(EXISTS ${FMOD_PATH}/fmodL.dll)
        list(APPEND DLLS_TO_MOVE ${FMOD_PATH}/fmodL.dll)
    endif()
    
    foreach(dll_file ${DLLS_TO_MOVE})
        string(REPLACE "/" "\\" DLL_FILE_PATH ${dll_file})
        add_custom_command(
            TARGET game_engine POST_BUILD
            COMMAND Copy /Y \"${DLL_FILE_PATH}\" \"$(TargetDir)\"
            COMMENT "Copying ${dll_file} to output directory"
        )
    endforeach()
endif()
