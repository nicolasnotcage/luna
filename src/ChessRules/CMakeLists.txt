project(ChessRules)

# Recursively collect all source files from src directory
file(GLOB_RECURSE CHESS_RULES_SOURCES 
    "src/*.cpp"
)

# Recursively collect all header files from include directory
file(GLOB_RECURSE CHESS_RULES_HEADERS
    "include/*.h"
)

# Create static library
add_library(chess_rules STATIC ${CHESS_RULES_SOURCES} ${CHESS_RULES_HEADERS})

# Set up include directories
target_include_directories(chess_rules PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Also include src directory for any internal headers
target_include_directories(chess_rules PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Set C++ standard
target_compile_features(chess_rules PUBLIC cxx_std_17)

# Add compiler warnings for better code quality
if(MSVC)
    target_compile_options(chess_rules PRIVATE /W4)
else()
    target_compile_options(chess_rules PRIVATE -Wall -Wextra -pedantic)
endif()

# Define preprocessor macro for library identification
target_compile_definitions(chess_rules PRIVATE CHESS_RULES_LIBRARY)