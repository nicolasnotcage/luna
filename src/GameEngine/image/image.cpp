/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "image/image.hpp"
#include "system/file_locator.hpp"  // Add this include

// https://github.com/nothings/stb
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION
#endif

#include <iostream>

namespace cge::image
{

// Define texture cache
std::unordered_map<std::string, SDLTextureInfo> TextureCache::texture_cache_;

void replace_all(std::string &in, const std::string &old_str, const std::string &new_str)
{
    size_t start_pos = 0;
    while((start_pos = in.find(old_str, start_pos)) != std::string::npos)
    {
        in.replace(start_pos, old_str.length(), new_str);
        start_pos += new_str.length(); // Handles case where 'to' is a substring of 'from'
    }
}

void load_image_data(ImageData &im_data, const std::string &fname)
{
    // First, use the file locator to find the actual path
    auto file_info = locate_path_for_filename(fname);
        
    stbi_set_flip_vertically_on_load(false);
    im_data.data =
        stbi_load(file_info.path.c_str(), &im_data.w, &im_data.h, &im_data.channels, STBI_rgb_alpha);

    // We are explicityly setting the loaded image to RGBA (using STBI_rgb_alpha)
    im_data.channels = 4;

    im_data.bytes_per_row = im_data.channels * im_data.w;

    if(im_data.data == nullptr)
    {
        return;
    }
}

void free_image_data(ImageData &im_data)
{
    if(im_data.data == nullptr) return;

    // Free the image data
    stbi_image_free(im_data.data);
    im_data.data = nullptr;
}

SDLTextureInfo create_texture(const SDLInfo &sdl_info, const std::string &filepath)
{
    // First check the cache; if it doesn't exist, create a new texture and add it to the cache. 
    if (auto cache_lookup = TextureCache::texture_cache_.find(filepath); cache_lookup != TextureCache::texture_cache_.end())
    {
        return cache_lookup->second;
    }
    else
    {
        SDLTextureInfo result;
        result.texture = nullptr;
        result.width = 0;
        result.height = 0;

        // No need to correct filepath here anymore since file_locator handles it
        ImageData im_data;
        load_image_data(im_data, filepath);
        
        // Check if image data was loaded successfully
        if (im_data.data == nullptr)
        {
            return result;
        }

        // Changed to different pixel format, which solved the problem of swapped color channels.
        result.texture = SDL_CreateTexture(sdl_info.renderer,
                                           SDL_PIXELFORMAT_ABGR8888,
                                           SDL_TEXTUREACCESS_STATIC,
                                           im_data.w,
                                           im_data.h);

        if (result.texture == nullptr)
        {
            std::cout << "ERROR: Failed to create SDL texture for " << filepath << '\n';
            std::cout << "SDL Error: " << SDL_GetError() << '\n';
            free_image_data(im_data);
            return result;
        }

        result.width = im_data.w;
        result.height = im_data.h;
        if(!SDL_UpdateTexture(result.texture, NULL, im_data.data, im_data.bytes_per_row))
        {
            std::cout << "ERROR Loading image using STB\n";
            std::cout << "SDL Error: " << SDL_GetError() << '\n';
        }

        free_image_data(im_data);

        // Push new texture to texture cache
        TextureCache::texture_cache_.insert({filepath, result});

        return result;
    }
}

void destroy_texture(const SDLTextureInfo &texture_info)
{
    if (texture_info.texture != nullptr)
    {
        SDL_DestroyTexture(texture_info.texture);
    }
}

} // namespace cge::image