#include <iostream>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <cstdint>

// Define Color Modes
enum ColorMode
{
    MONOCHROME = 0x00,
    COLOR_16 = 0x01,
    COLOR_256 = 0x02
};

// Screen Class
class Screen
{
public:
    uint8_t width;
    uint8_t height;
    ColorMode color_mode;
    std::vector<std::vector<char>> buffer;
    std::vector<std::vector<uint8_t>> colors;
    uint8_t cursor_x;
    uint8_t cursor_y;

    Screen(uint8_t w, uint8_t h, ColorMode mode)
        : width(w), height(h), color_mode(mode),
          buffer(h, std::vector<char>(w, ' ')),
          colors(h, std::vector<uint8_t>(w, 0)),
          cursor_x(0), cursor_y(0) {}

    uint8_t get_width() const
    {
        return width;
    }
    uint8_t get_height() const
    {
        return height;
    }

    void draw_character(uint8_t x, uint8_t y, char character, uint8_t color)
    {
        if (x >= width || y >= height)
        {
            throw std::out_of_range("Coordinates are out of bounds.");
        }
        buffer[y][x] = character;
        colors[y][x] = color;
    }

    void draw_line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, char character, uint8_t color)
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

    void render_text(uint8_t x, uint8_t y, uint8_t color, const std::string &text)
    {
        if (x >= width || y >= height)
        {
            throw std::out_of_range("Starting coordinates are out of bounds.");
        }

        for (size_t i = 0; i < text.size(); ++i)
        {
            uint8_t cur_x = x + i;
            if (cur_x >= width)
                break; // Stop rendering if out of bounds
            draw_character(cur_x, y, text[i], color);
        }
    }

    void move_cursor(uint8_t x, uint8_t y)
    {
        if (x >= width || y >= height)
        {
            throw std::out_of_range("Cursor coordinates are out of bounds.");
        }
        cursor_x = x;
        cursor_y = y;
    }

    void draw_at_cursor(char character, uint8_t color)
    {
        draw_character(cursor_x, cursor_y, character, color);
    }

    void clear_screen()
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

    void display() const
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
};

// Parse screen setup parameters
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

// Handle screen setup command
Screen handle_screen_setup(const std::vector<uint8_t> &data)
{
    uint8_t width, height;
    ColorMode color_mode;
    parse_screen_setup(data, width, height, color_mode);
    return Screen(width, height, color_mode);
}

// Process commands
template <typename ScreenPtr>
ScreenPtr process_command(uint8_t command_byte, uint8_t length_byte, const std::vector<uint8_t> &data, ScreenPtr current_screen)
{
    if (command_byte == 0x1)
    {
        current_screen = new Screen(handle_screen_setup(data));
        return current_screen;
    }
    else if (command_byte == 0x2)
    {
        if (!current_screen)
        {
            throw std::runtime_error("Screen not set up. Cannot process draw command.");
        }
        if (data.size() != 4)
        {
            throw std::invalid_argument("Draw character command requires exactly 4 data bytes.");
        }
        uint8_t x = data[0];
        uint8_t y = data[1];
        uint8_t color = data[2];
        char character = static_cast<char>(data[3]);

        current_screen->draw_character(x, y, character, color);
        return current_screen;
    }
    else if (command_byte == 0x3)
    {
        if (!current_screen)
        {
            throw std::runtime_error("Screen not set up. Cannot process draw line command.");
        }
        if (data.size() != 6)
        {
            throw std::invalid_argument("Draw line command requires exactly 6 data bytes.");
        }
        uint8_t x1 = data[0];
        uint8_t y1 = data[1];
        uint8_t x2 = data[2];
        uint8_t y2 = data[3];
        uint8_t color = data[4];
        char character = static_cast<char>(data[5]);

        current_screen->draw_line(x1, y1, x2, y2, character, color);
        return current_screen;
    }
    else if (command_byte == 0x4)
    {
        if (!current_screen)
        {
            throw std::runtime_error("Screen not set up. Cannot process render text command.");
        }
        if (data.size() < 3)
        {
            throw std::invalid_argument("Render text command requires at least 3 data bytes.");
        }
        uint8_t x = data[0];
        uint8_t y = data[1];
        uint8_t color = data[2];
        std::string text(data.begin() + 3, data.end());

        current_screen->render_text(x, y, color, text);
        return current_screen;
    }
    else if (command_byte == 0x5)
    {
        if (!current_screen)
        {
            throw std::runtime_error("Screen not set up. Cannot process cursor movement command.");
        }
        if (data.size() != 2)
        {
            throw std::invalid_argument("Cursor movement command requires exactly 2 data bytes.");
        }
        uint8_t x = data[0];
        uint8_t y = data[1];

        current_screen->move_cursor(x, y);
        return current_screen;
    }
    else if (command_byte == 0x6)
    {
        if (!current_screen)
        {
            throw std::runtime_error("Screen not set up. Cannot process draw at cursor command.");
        }
        if (data.size() != 2)
        {
            throw std::invalid_argument("Draw at cursor command requires exactly 2 data bytes.");
        }
        char character = static_cast<char>(data[0]);
        uint8_t color = data[1];

        current_screen->draw_at_cursor(character, color);
        return current_screen;
    }
    else if (command_byte == 0x7)
    {
        if (!current_screen)
        {
            throw std::runtime_error("Screen not set up. Cannot process clear screen command.");
        }
        if (!data.empty())
        {
            throw std::invalid_argument("Clear screen command does not require any data bytes.");
        }

        current_screen->clear_screen();
        return current_screen;
    }
    else if (command_byte == 0xFF)
    {
        std::cout << "End of command stream detected." << std::endl;
        return nullptr; // Signal to stop processing further commands
    }
    else
    {
        std::cerr << "Unknown command byte: " << (int)command_byte << '\n';
        return current_screen;
    }
}

// Main function
int main()
{
    try
    {
        // Byte stream: Screen setup (80x24, 256 colors), Draw 'A' at (10, 5) in color 3, Draw line, Render text, Move cursor, Draw at cursor, Clear screen, End stream
        std::vector<uint8_t> stream = {
            0x1, 3, 80, 24, 0x02,                                                              // Screen Setup
            0x2, 4, 10, 5, 3, 'A',                                                             // Draw Character
            0x3, 6, 5, 5, 20, 10, 2, '*',                                                      // Draw Line
            0x4, 10, 2, 2, 3, 'H', 'e', 'l', 'l', 'o', ',', ' ', 'W', 'o', 'r', 'l', 'd', '!', // Render Text
            0x5, 2, 15, 10,                                                                    // Move Cursor
            0x6, 2, '@', 4,                                                                    // Draw At Cursor
            0x7, 0,                                                                            // Clear Screen
            0xFF, 0                                                                            // End of Stream
        };

        Screen *screen = nullptr;

        size_t i = 0;
        while (i < stream.size())
        {
            uint8_t command_byte = stream[i++];
            uint8_t length_byte = stream[i++];
            std::vector<uint8_t> data(stream.begin() + i, stream.begin() + i + length_byte);
            i += length_byte;

            screen = process_command(command_byte, length_byte, data, screen);

            if (!screen)
            {
                break;
            }
        }

        if (screen)
        {
            screen->display();
            delete screen;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << '\n';
    }

    return 0;
}
