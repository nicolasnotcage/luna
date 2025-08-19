/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef IMAGE_IMAGE_HPP
#define IMAGE_IMAGE_HPP

#include "platform/sdl.h"
#include "platform/types.hpp"

#include <optional>
#include <string>
#include <unordered_map>

namespace cge::image
{

struct ImageData
{
    int            w = 0;
    int            h = 0;
    int            channels = 0;
    int            bytes_per_row;
    unsigned char *data = nullptr;
};

struct SDLTextureInfo
{
    SDL_Texture *texture;
    int          width;
    int          height;
};

// Contains a static texture cache to prevent unnecessary duplicate loading
struct TextureCache
{
    static std::unordered_map<std::string, SDLTextureInfo> texture_cache_;
};

void load_image_data(ImageData &im_data, const std::string &fname);

void free_image_data(ImageData &im_data);

SDLTextureInfo create_texture(const SDLInfo &sdl_info, const std::string &filepath);
void           destroy_texture(const SDLTextureInfo &texture_info);

} // namespace cge::image

#endif // IMAGE_IMAGE_HPP
