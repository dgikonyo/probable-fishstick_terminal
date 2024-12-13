#ifndef PARSE_SCREEN_SETUP_H
#define PARSE_SCREEN_SETUP_H

#include <vector>
#include <stdexcept>
#include "ColorMode.h"

void parse_screen_setup(const std::vector<uint8_t> &data, uint8_t &width, uint8_t &height, ColorMode &color_mode);

#endif // PARSE_SCREEN_SETUP_H