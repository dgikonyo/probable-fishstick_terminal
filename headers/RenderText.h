#ifndef RENDER_TEXT_H
#define RENDER_TEXT_H

#include "Screen.h"
#include <string>

void render_text(Screen &screen, uint8_t x, uint8_t y, uint8_t color, const std::string &text);

#endif // RENDER_TEXT_H