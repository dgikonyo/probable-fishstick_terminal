#include "../headers/DrawLine.h"

void draw_line(Screen &screen, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, char character, uint8_t color)
{
    screen.draw_line(x1, y1, x2, y2, character, color);
}