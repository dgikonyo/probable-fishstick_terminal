#include "../headers/DrawCharacter.h"

void draw_character(Screen &screen, uint8_t x, uint8_t y, char character, uint8_t color)
{
    screen.draw_character(x, y, character, color);
}