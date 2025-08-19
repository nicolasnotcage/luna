#ifndef SERIALIZATION_SERIALIZER_HPP
#define SERIALIZATION_SERIALIZER_HPP

#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace cge
{

// Abstract serializer class
class Serializer
{
public:
    virtual ~Serializer() = default;

    // Primitive type serialization methods
    virtual void write(const std::string& key, const std::string& value) = 0;
    virtual void write(const std::string& key, int value) = 0;
    virtual void write(const std::string& key, float value) = 0;
    virtual void write(const std::string& key, bool value) = 0;

    // Reading methods
    virtual bool read(const std::string& key, std::string& value) = 0;
    virtual bool read(const std::string& key, int& value) = 0;
    virtual bool read(const std::string& key, float& value) = 0;
    virtual bool read(const std::string& key, bool& value) = 0;

    // File operations
    virtual bool open(const std::string& filepath, bool write_mode) = 0;
    virtual void close() = 0;
    virtual bool save() = 0;
};

// Text-based Serialization
class TextSerializer : public Serializer
{
public:
    // Write/read methods
    void write(const std::string& key, const std::string& value) override;
    void write(const std::string& key, int value) override;
    void write(const std::string& key, float value) override;
    void write(const std::string& key, bool value) override;

    bool read(const std::string& key, std::string& value) override;
    bool read(const std::string& key, int& value) override;
    bool read(const std::string& key, float& value) override;
    bool read(const std::string& key, bool& value) override;

    // File operations
    bool open(const std::string& filepath, bool write_mode) override;
    void close() override;
    bool save() override;

private:
    std::unordered_map<std::string, std::string> data_;
    std::string filepath_;
    bool is_open_ = false;
    bool is_write_mode_ = false;
};

// Binary-based Serialization
class BinarySerializer : public Serializer
{
public:
    BinarySerializer() : is_little_endian_(system_is_little_endian()) {}

    // Read/write
    void write(const std::string& key, const std::string& value) override;
    void write(const std::string& key, int value) override;
    void write(const std::string& key, float value) override;
    void write(const std::string& key, bool value) override;

    bool read(const std::string& key, std::string& value) override;
    bool read(const std::string& key, int& value) override;
    bool read(const std::string& key, float& value) override;
    bool read(const std::string& key, bool& value) override;

    // File operations
    bool open(const std::string& filepath, bool write_mode) override;
    void close() override;
    bool save() override;

private:
    std::vector<char> data_buffer_;                                         // Binary data buffer
    std::unordered_map<std::string, std::pair<size_t, size_t>> data_map_;   // Key: {offset, size}

    std::string filepath_;
    bool is_open_{false};
    bool is_write_mode_{false};
    bool is_little_endian_;

    // Helper methods for binary operations
    bool system_is_little_endian()
    {
        constexpr uint8_t one_byte = 0x1;
        uint32_t one_int = 0x1;
        return reinterpret_cast<uint8_t*>(&one_int)[0] == one_byte;
    }

    // Template function to reverse the order of bytes in a buffer
    template <size_t SIZE>
    void reverse_order(char buffer[SIZE])
    {
        for (size_t i = 0; i < SIZE / 2; ++i)
        {
            std::swap(buffer[i], buffer[SIZE - i - 1]);
        }
    }

    // Template function to reverse the byte order of a value
    template <typename T>
    T reverse_byte_order(const T& in)
    {
        if (sizeof(T) == 1) return in;

        char buffer[sizeof(T)];
        std::memcpy(buffer, &in, sizeof(T));
        reverse_order<sizeof(T)>(buffer);

        T out;
        std::memcpy(&out, buffer, sizeof(T));
        return out;
    }

    // Template methods for reading/writing binary data
    template <typename T>
    void write_data(const T& data, size_t& offset) 
    {
        if (is_little_endian_ != system_is_little_endian()) 
        {
            T t_data = reverse_byte_order<T>(data);
            const char* bytes = reinterpret_cast<const char*>(&t_data);
            data_buffer_.insert(data_buffer_.end(), bytes, bytes + sizeof(T));
        }
        else 
        {
            const char* bytes = reinterpret_cast<const char*>(&data);
            data_buffer_.insert(data_buffer_.end(), bytes, bytes + sizeof(T));
        }
        offset += sizeof(T);
    }

    template <typename T>
    bool read_data(T& data, size_t offset, size_t size) 
    {
        if (size != sizeof(T)) 
        {
            std::cout << "Size mismatch: expected " << sizeof(T) << ", got " << size << std::endl;
            return false;
        }

        if (is_little_endian_ != system_is_little_endian()) 
        {
            T t_data;
            std::memcpy(&t_data, &data_buffer_[offset], sizeof(T));
            data = reverse_byte_order<T>(t_data);
        }
        else 
        {
            std::memcpy(&data, &data_buffer_[offset], sizeof(T));
        }
        return true;
    }
};

} // namespace cge

#endif // SERIALIZATION_SERIALIZER_HPP