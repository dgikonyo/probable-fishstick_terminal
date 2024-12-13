#include "../headers/parse_screen_setup.h";

void parse_screen_setup(const std::vector<uint8_t> &data, uint8_t &width, uint8_t &height, ColorMode &color_mode)
{
    if (data.size() != 3)
    {
        throw std::invalid_argument("Screen setup data must be exactly 3 bytes.");
    }
    width = data[0];
    height = data[1];
    uint8_t mode = data[2];

    if (width == 0 || height == 0)
    {
        throw std::invalid_argument("Width and height must be non-zero.");
    }

    if (mode > 0x02)
    {
        throw std::invalid_argument("Invalid color mode.");
    }
    color_mode = static_cast<ColorMode>(mode);
}