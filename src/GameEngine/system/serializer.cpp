/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "system/serializer.hpp"
#include "system/string_utils.hpp"

#include <fstream>
#include <string>
#include <cstring>
#include <iostream>

namespace cge
{
//--------------------------
//     Text Serializer
//--------------------------
bool TextSerializer::open(const std::string& filepath, bool write_mode)
{
    filepath_ = filepath;
    is_write_mode_ = write_mode;

    // Clear existing data
    data_.clear();

    // If not in write mode, load the file
    if (!write_mode)
    {
        auto lines = cge::utility::lines_from_file(filepath);
        for (const auto& line : lines)
        {
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') continue;

            // Parse key-value pairs (format: Key = Value)
            auto parts = cge::utility::split(line, "=");
            if (parts.size() >= 2) {
                std::string key = cge::utility::trim(parts[0]);
                std::string value = cge::utility::trim(parts[1]);
                data_[key] = value;
            }
        }
    }

    is_open_ = true;
    return true;
}

void TextSerializer::close()
{
    is_open_ = false;
}

bool TextSerializer::save()
{
    if (!is_write_mode_) return false;

    std::ofstream file(filepath_);
    if (!file.is_open()) return false;

    // Write header
    file << "# Configuration File" << std::endl;
    file << "# Generated on: " << __DATE__ << " " << __TIME__ << std::endl;
    file << std::endl;

    // Write data
    for (const auto& [key, value] : data_)
    {
        file << key << " = " << value << std::endl;
    }

    file.close();
    return true;
}

void TextSerializer::write(const std::string& key, const std::string& value)
{
    data_[key] = value;
}

void TextSerializer::write(const std::string& key, int value)
{
    data_[key] = std::to_string(value);
}

void TextSerializer::write(const std::string& key, float value)
{
    data_[key] = std::to_string(value);
}

void TextSerializer::write(const std::string& key, bool value)
{
    data_[key] = value ? "true" : "false";
}

bool TextSerializer::read(const std::string& key, std::string& value)
{
    auto it = data_.find(key);
    if (it != data_.end())
    {
        value = it->second;
        return true;
    }
    return false;
}

bool TextSerializer::read(const std::string& key, int& value)
{
    auto it = data_.find(key);
    if (it != data_.end())
    {
        try
        {
            value = std::stoi(it->second);
            return true;
        }
        catch (...)
        {
            return false;
        }
    }
    return false;
}

bool TextSerializer::read(const std::string& key, float& value)
{
    auto it = data_.find(key);
    if (it != data_.end())
    {
        try
        {
            value = std::stof(it->second);
            return true;
        }
        catch (...)
        {
            return false;
        }
    }
    return false;
}

bool TextSerializer::read(const std::string& key, bool& value) {
    auto it = data_.find(key);
    if (it != data_.end()) {
        std::string lower = cge::utility::to_lower(it->second);
        if (lower == "true" || lower == "1")
        {
            value = true;
            return true;
        }
        else if (lower == "false" || lower == "0")
        {
            value = false;
            return true;
        }
    }
    return false;
}

//--------------------------
//     Binary Serializer
//--------------------------
bool BinarySerializer::open(const std::string& filepath, bool write_mode)
{
    filepath_ = filepath;
    is_write_mode_ = write_mode;

    // Clear existing data
    data_buffer_.clear();
    data_map_.clear();

    if (!write_mode)
    {
        // Load file contents into buffer
        std::ifstream file(filepath_, std::ios::binary);
        if (!file.is_open()) return false;

        // Read number of entries
        int num_entries;
        file.read(reinterpret_cast<char*>(&num_entries), sizeof(int));

        // Read each key-value pair
        for (int i = 0; i < num_entries; i++) 
        {
            // Read key length
            int key_length;
            file.read(reinterpret_cast<char*>(&key_length), sizeof(int));

            // Preallocate string size and read key
            std::string key;
            key.resize(key_length);
            file.read(key.data(), key_length);

            // Read data size
            size_t data_size;
            file.read(reinterpret_cast<char*>(&data_size), sizeof(size_t));

            // Store the current position in the buffer as the offset
            size_t offset = data_buffer_.size();

            // Resize buffer to accommodate the new data
            data_buffer_.resize(offset + data_size);

            // Read data into buffer
            file.read(&data_buffer_[offset], data_size);

            // Store in map
            data_map_[key] = { offset, data_size };
        }

        file.close();
    }

    is_open_ = true;
    return true;
}

void BinarySerializer::close()
{
    is_open_ = false;
}

bool BinarySerializer::save()
{
    if (!is_write_mode_) return false;

    std::ofstream file(filepath_, std::ios::binary);
    if (!file.is_open()) return false;

    // Write number of entries
    int num_entries = data_map_.size();
    file.write(reinterpret_cast<const char*>(&num_entries), sizeof(int));

    // Write each key-value pair sequentially
    for (const auto& [key, value_pair] : data_map_) 
    {
        // Write key length
        int key_length = key.size();
        file.write(reinterpret_cast<const char*>(&key_length), sizeof(int));

        // Write key content
        file.write(key.data(), key_length);

        // Write data size
        size_t data_size = value_pair.second;
        file.write(reinterpret_cast<const char*>(&data_size), sizeof(size_t));

        // Write data content
        file.write(&data_buffer_[value_pair.first], data_size);
    }

    file.close();
    return true;
}

void BinarySerializer::write(const std::string& key, const std::string& value)
{
    // Get offset
    size_t start_offset = data_buffer_.size();

    // Get and write size of string
    size_t current_offset = start_offset;
    uint32_t length = value.size();
    write_data<uint32_t>(length, current_offset);

    // Write string data
    data_buffer_.insert(data_buffer_.end(), value.begin(), value.end());

    // Store total size
    size_t total_size = sizeof(uint32_t) + value.size();

    // Store data using the starting offset
    data_map_[key] = { start_offset, total_size };
}

void BinarySerializer::write(const std::string& key, int value)
{
    size_t offset = data_buffer_.size();
    write_data<int>(value, offset);
    data_map_[key] = { offset - sizeof(int), sizeof(int) };
}

void BinarySerializer::write(const std::string& key, float value)
{
    size_t offset = data_buffer_.size();
    write_data<float>(value, offset);
    data_map_[key] = { offset - sizeof(float), sizeof(float) };
}

void BinarySerializer::write(const std::string& key, bool value)
{
    size_t offset = data_buffer_.size();
    char byte = value ? 1 : 0;
    data_buffer_.push_back(byte);
    data_map_[key] = { offset, sizeof(char) };
}

bool BinarySerializer::read(const std::string& key, std::string& value)
{
    auto it = data_map_.find(key);
    if (it == data_map_.end()) return false;

    size_t offset = it->second.first;

    // Read string length first
    uint32_t length;
    if (!read_data<uint32_t>(length, offset, sizeof(uint32_t))) return false;
   
    // Verify that the rest of the data is available
    if (offset + sizeof(uint32_t) + length > data_buffer_.size()) return false;

    // Read string data
    value = std::string(data_buffer_.data() + offset + sizeof(uint32_t), length);

    return true;
}

bool BinarySerializer::read(const std::string& key, int& value)
{
    auto it = data_map_.find(key);
    if (it == data_map_.end()) return false;

    size_t offset = it->second.first;
    size_t size = it->second.second;

    return read_data<int>(value, offset, size);
}

bool BinarySerializer::read(const std::string& key, float& value)
{
    auto it = data_map_.find(key);
    if (it == data_map_.end()) return false;

    size_t offset = it->second.first;
    size_t size = it->second.second;

    return read_data<float>(value, offset, size);
}

bool BinarySerializer::read(const std::string& key, bool& value)
{
    auto it = data_map_.find(key);
    if (it == data_map_.end()) return false;

    size_t offset = it->second.first;
    size_t size = it->second.second;

    if (size != sizeof(char)) return false;

    // Boolean values are stored as a single byte
    char byte;
    std::memcpy(&byte, &data_buffer_[offset], size);
    value = byte != 0;

    return true;
}

} // namespace cge
