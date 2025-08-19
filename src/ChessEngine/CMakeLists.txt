project(ChessEngine)

# Recursively collect all source files from src directory
file(GLOB_RECURSE CHESS_ENGINE_SOURCES 
    "src/*.cpp"
)

# Recursively collect all header files from include directory
file(GLOB_RECURSE CHESS_ENGINE_HEADERS
    "include/*.h"
)

# Create a library for the engine core
add_library(chess_engine_lib STATIC ${CHESS_ENGINE_SOURCES} ${CHESS_ENGINE_HEADERS})

# Link chess rules library
target_link_libraries(chess_engine_lib PUBLIC chess_rules)

# Include directories
target_include_directories(chess_engine_lib PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Set C++ standard
target_compile_features(chess_engine_lib PUBLIC cxx_std_17)

# Add compiler warnings
if(MSVC)
    target_compile_options(chess_engine_lib PRIVATE /W4)
else()
    target_compile_options(chess_engine_lib PRIVATE -Wall -Wextra -pedantic)
endif()

# Create executable for UCI interface
add_executable(luna main.cpp)

# Link libraries
target_link_libraries(luna PRIVATE chess_engine_lib chess_rules)

# Include directories for executable
target_include_directories(luna PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Set C++ standard for executable
target_compile_features(luna PRIVATE cxx_std_17)

# Create alias for compatibility
add_library(chess_engine ALIAS chess_engine_lib)
