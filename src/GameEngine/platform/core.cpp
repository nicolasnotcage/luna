/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "platform/core.hpp"
#include "platform/sdl.h"

#include <cstring>
#include <iostream>

namespace cge
{

// Initializes the SDL library; specifies that video should be initialized
void init_sdl()
{
    if(!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cout << "Error initializing SDL: " << SDL_GetError() << std::endl;
        exit(1);
    }
}

// 
void create_sdl_components(SDLInfo           &sdl_info,
                           int                window_width,
                           int                window_height,
                           const std::string &window_name)
{
    // Creates a new group of SDL properties
    SDL_PropertiesID props = SDL_CreateProperties();
    if(props == 0)
    {
        std::cout << "Error creating SDL Window Properties: " << SDL_GetError() << '\n';
        exit(1);
    }

    // Sets SDL properties within the above-created collection of properties
    SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, window_name.c_str());
    SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, true);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, window_width);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, window_height);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_X_NUMBER, 200);
    SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_Y_NUMBER, 100);

    // Set the Window pointer for the custom SDLInfo struct with the properties
    sdl_info.window = SDL_CreateWindowWithProperties(props);
    if(sdl_info.window == nullptr)
    {
        std::cout << "Error initializing SDL Window" << SDL_GetError() << std::endl;
        exit(1);
    }

    // Set the Renderer pointer in the SDLInfo struct with the new window pointer
    sdl_info.renderer = SDL_CreateRenderer(sdl_info.window, NULL);
    if(sdl_info.renderer == nullptr)
    {
        std::cout << "Error initializing SDL Renderer" << SDL_GetError() << std::endl;
        exit(1);
    }
}

void destroy_sdl_components(SDLInfo &sdl_info)
{
    SDL_DestroyRenderer(sdl_info.renderer);
    SDL_DestroyWindow(sdl_info.window);
    SDL_Quit();
}

} // namespace cge
