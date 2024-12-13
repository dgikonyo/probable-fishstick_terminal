#include "../headers/DrawAtCursor.h"

void draw_at_cursor(Screen &screen, char character, uint8_t color)
{
    screen.draw_at_cursor(character, color);
}