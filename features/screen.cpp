#include "../headers/Screen.h";

Screen::Screen(uint8_t w, uint8_t h, ColorMode mode)
    : width(w), height(h), color_mode(mode),
      buffer(h, std::vector<char>(w, ' ')),
      colors(h, std::vector<uint8_t>(w, 0)),
      cursor_x(0), cursor_y(0) {}

uint8_t Screen::get_width() const { return width; }
uint8_t Screen::get_height() const { return height; }

void Screen::draw_character(uint8_t x, uint8_t y, char character, uint8_t color)
{
    if (x >= width || y >= height)
    {
        throw std::out_of_range("Coordinates are out of bounds.");
    }
    buffer[y][x] = character;
    colors[y][x] = color;
}

void Screen::draw_line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, char character, uint8_t color)
{
    if (x1 >= width || y1 >= height || x2 >= width || y2 >= height)
    {
        throw std::out_of_range("Coordinates are out of bounds.");
    }

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (true)
    {
        draw_character(x1, y1, character, color);
        if (x1 == x2 && y1 == y2)
            break;
        int e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y1 += sy;
        }
    }
}

void Screen::render_text(uint8_t x, uint8_t y, uint8_t color, const std::string &text)
{
    if (x >= width || y >= height)
    {
        throw std::out_of_range("Starting coordinates are out of bounds.");
    }

    for (size_t i = 0; i < text.size(); ++i)
    {
        uint8_t cur_x = x + i;
        if (cur_x >= width)
            break;
        draw_character(cur_x, y, text[i], color);
    }
}

void Screen::move_cursor(uint8_t x, uint8_t y)
{
    if (x >= width || y >= height)
    {
        throw std::out_of_range("Cursor coordinates are out of bounds.");
    }
    cursor_x = x;
    cursor_y = y;
}

void Screen::draw_at_cursor(char character, uint8_t color)
{
    draw_character(cursor_x, cursor_y, character, color);
}

void Screen::clear_screen()
{
    for (auto &row : buffer)
    {
        std::fill(row.begin(), row.end(), ' ');
    }
    for (auto &row : colors)
    {
        std::fill(row.begin(), row.end(), 0);
    }
    cursor_x = 0;
    cursor_y = 0;
}

void Screen::display() const
{
    std::cout << "Screen (" << (int)width << "x" << (int)height
              << ", Color Mode: " << (int)color_mode << ")\n";
    for (const auto &row : buffer)
    {
        for (char c : row)
        {
            std::cout << c;
        }
        std::cout << '\n';
    }
}
