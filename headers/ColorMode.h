#ifndef COLORMODE_H
#define COLORMODE_H

#include <cstdint>

enum class ColorMode : uint8_t
{
    MONOCHROME = 0x00,
    COLOR_16 = 0x01,
    COLOR_256 = 0x02
};

#endif // COLORMODE_H