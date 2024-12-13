#include "../headers/RenderText.h"

void render_text(Screen &screen, uint8_t x, uint8_t y, uint8_t color, const std::string &text)
{
    screen.render_text(x, y, color, text);
}