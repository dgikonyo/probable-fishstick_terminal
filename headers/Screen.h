#ifndef SCREEN_H
#define SCREEN_H

#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>
#include <cmath>
#include "ColorMode.h"

class Screen
{
private:
    uint8_t width;
    uint8_t height;
    ColorMode color_mode;
    std::vector<std::vector<char>> buffer;
    std::vector<std::vector<uint8_t>> colors;
    uint8_t cursor_x;
    uint8_t cursor_y;

public:
    Screen(uint8_t w, uint8_t h, ColorMode mode);
    uint8_t get_width() const;
    uint8_t get_height() const;
    void draw_character(uint8_t x, uint8_t y, char character, uint8_t color);
    void draw_line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, char character, uint8_t color);
    void render_text(uint8_t x, uint8_t y, uint8_t color, const std::string &text);
    void move_cursor(uint8_t x, uint8_t y);
    void draw_at_cursor(char character, uint8_t color);
    void clear_screen();
    void display() const;
};

#endif // SCREEN_H