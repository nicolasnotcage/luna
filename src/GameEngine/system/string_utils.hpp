#ifndef SERIALIZATION_STRING_UTILS_HPP
#define SERIALIZATION_STRING_UTILS_HPP

#include <cctype>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>

// Contains string utility functions specified in lecture notes
namespace cge
{
namespace utility
{

// Helper methods from lecture notes
inline std::vector<std::string> lines_from_file(const std::string& path)
{
    std::vector<std::string> lines;
    std::ifstream file(path);
    if (file.is_open()) 
    {
        std::string line;
        while (std::getline(file, line)) 
        {
            lines.push_back(line);
        }
        file.close();
    }
    return lines;
}

inline std::vector<std::string> split(const std::string& str, const std::string& delimiter)
{
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = str.find(delimiter);
    while (end != std::string::npos) 
    {
        tokens.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    tokens.push_back(str.substr(start));
    return tokens;
}

inline std::string trim(const std::string& s)
{
    const char* whitespace = " \t\n\r\f\v";
    size_t start = s.find_first_not_of(whitespace);
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(whitespace);
    return s.substr(start, end - start + 1);
}

inline std::string to_upper(const std::string& s)
{
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::toupper(c); });
    return result;
}

inline std::string to_lower(const std::string& s)
{
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return result;
}

} // namespace cge::utility
} // namespace cge

#endif // SERIALIZATION_STRING_UTILS_HPP
