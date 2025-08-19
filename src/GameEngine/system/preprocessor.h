/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#define STRINGIFY(x) #x
#define C_STRING(x) STRINGIFY(x)
#define STD_STRING(x) std::string(STRINGIFY(x))

#endif // PREPROCESSOR_H
