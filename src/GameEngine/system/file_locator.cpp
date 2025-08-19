/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "system/file_locator.hpp"
#include "system/preprocessor.h"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <filesystem>  // Add for better path debugging

namespace cge
{
namespace
{
std::string executable_path;
std::string source_path;
std::string resource_path;

std::string correct_path_separators(const std::string &path)
{
#ifdef BUILD_WINDOWS
    constexpr const char incorrect_sep = '/';
    constexpr const char correct_sep = '\\';
#else
    constexpr const char incorrect_sep = '\\';
    constexpr const char correct_sep = '/';
#endif
    std::string correct_str(path);
    std::replace(correct_str.begin(), correct_str.end(), incorrect_sep, correct_sep);
    return correct_str;
}

FileInfo locate_path_for_filename_with_prefix(const std::string &prefix,
                                              const std::string &filename,
                                              uint16_t           num_directories = 5)
{
    FileInfo result;
    result.found = false;

    std::string   rel_path = filename;
    std::ifstream ifs;


    for(int i = 0; i < num_directories && !result.found; ++i)
    {
        std::string abs_path = correct_path_separators(prefix + rel_path);
        
        ifs.open(abs_path);
        if(ifs.is_open())
        {
            result.found = true;
            result.path = abs_path;
            ifs.seekg(0, std::ios::end);
            result.size = static_cast<uint32_t>(ifs.tellg());
            break;
        }

        rel_path.insert(0, "../");
    }

    ifs.close();

    return result;
}
} // namespace

void set_system_paths(const char        *exec_name,
                      const std::string &resource_dir,
                      const std::string &src_dir)
{
#ifdef BUILD_WINDOWS
    constexpr const char correct_sep = '\\';
#else
    constexpr const char correct_sep = '/';
#endif

    source_path = std::string(src_dir);
    if(source_path[0] == '\"') { source_path = source_path.substr(1, source_path.length() - 2); }

    resource_path = std::string(resource_dir);
    if(resource_path[0] == '\"')
    {
        resource_path = resource_path.substr(1, resource_path.length() - 2);
    }

    source_path = source_path + std::string(&correct_sep, 1);
    resource_path = resource_path + std::string(&correct_sep, 1);

    std::string exec_name_str(exec_name);
    for(size_t i = exec_name_str.length() - 1; i >= 0; --i)
    {
        if(exec_name_str[i] == correct_sep)
        {
            executable_path = exec_name_str.substr(0, i + 1);
            break;
        }
    }
}

FileInfo locate_path_for_filename(const std::string &filename, uint16_t num_directories)
{
    FileInfo result;
    result.found = false;
        
    // Search with prefix
    result = locate_path_for_filename_with_prefix(resource_path, filename, num_directories);

    // if not found, try looking in the local path
    if(!result.found)
    {
        result = locate_path_for_filename_with_prefix("", filename, num_directories);
    }

    // if not found, try looking at the executable path
    if(!result.found)
    {
        result = locate_path_for_filename_with_prefix(executable_path, filename, num_directories);
    }

    // if still not found, try looking at the source path
    if(!result.found)
    {
        result = locate_path_for_filename_with_prefix(source_path, filename, num_directories);
    }
    
    // Additional fallback: try ChessLab root resources
    if(!result.found)
    {
        // Go up from build/src/GameEngine/Debug to ChessLab root
        std::string chesslab_resources = executable_path + "../../../../resources/";
        result = locate_path_for_filename_with_prefix(chesslab_resources, filename, 1);
    }

    return result;
}

} // namespace cge