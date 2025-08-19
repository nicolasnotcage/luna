/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef FILELOCATOR_HPP
#define FILELOCATOR_HPP

#include <cstdint>
#include <string>

namespace cge
{

struct FileInfo
{
    bool        found = false;
    std::string path;
    uint64_t    size = 0;
};

void set_system_paths(const char        *exec_name,
                      const std::string &resource_dir,
                      const std::string &src_dir);

FileInfo locate_path_for_filename(const std::string &filename, uint16_t num_directories = 5);

} // namespace cge
#endif
