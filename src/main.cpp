#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstdint>

// enum for colorcodes
enum ColorMode
{
    MONOCHROME = 0x00,
    COLOR_16 = 0x01,
    COLOR_256 = 0x02
};

class Screen
{
public:
    uint8_t width;
    uint8_t height;
    ColorMode color_mode;
    std::vector<std::vector<char>> buffer;    // Character buffer
    std::vector<std::vector<uint8_t>> colors; // Color buffer

    // Initialize with spaces and default color ~ 0x1
    Screen(uint8_t w, uint8_t h, ColorMode color_mode)
        : width(width), height(height), color_mode(color_mode),
          buffer(height, std::vector<char>(width, ' ')),
          colors(height, std::vector<uint8_t>(width, 0)) {}

    // Display the screen in the terminal
    void display() const
    {
        std::cout << "Screen (" << (int)width << "x" << (int)height
                  << ", Color Mode: " << (int)color_mode << ")\n";

        for (int i = 0; i < buffer.size(); ++i)
        {
            for (int j = 0; j < buffer[i].size(); ++j)
            {
                std::cout << buffer[i][j];
            }
            std::cout << '\n';
        }
    }

    // 0x2
    void draw_character(uint8_t x, uint8_t y, char character, uint8_t color)
    {
        std::cout << x, y;
        if (x >= width || y >= height)
        {
            throw std::out_of_range("Coordinates are out of bounds.");
        }
        buffer[y][x] = character;
        colors[y][x] = color;
    }
};

// Function to parse the Screen Setup command data
void parse_screen_setup(const std::vector<uint8_t> &data, uint8_t &width, uint8_t &height, ColorMode &color_mode)
{
    if (data.size() != 3)
    {
        throw std::invalid_argument("Invalid data length for Screen Setup command.");
    }

    width = data[0];
    height = data[1];
    uint8_t mode = data[2];

    // Validate width and height
    if (width == 0 || height == 0)
    {
        throw std::invalid_argument("Width and height must be greater than 0.");
    }

    // Validate color mode
    switch (mode)
    {
    case MONOCHROME:
    case COLOR_16:
    case COLOR_256:
        color_mode = static_cast<ColorMode>(mode);
        break;
    default:
        throw std::invalid_argument("Invalid color mode.");
    }
}

// Function to handle the Screen Setup command
Screen handle_screen_setup(const std::vector<uint8_t>& data)
{
    uint8_t width, height;
    ColorMode color_mode;

    // Parse the data
    parse_screen_setup(data, width, height, color_mode);

    // Initialize and return the Screen object
    Screen screen(width, height, color_mode);
    return screen;
}

// Function to process a single command
Screen *process_command(uint8_t command_byte, uint8_t length_byte, const std::vector<uint8_t> &data, Screen *current_screen)
{
    if (command_byte == 0x1)
    {
        current_screen = new Screen(handle_screen_setup(data));
        return current_screen;
    }
    else if (command_byte == 0x2)
    {
        if (current_screen == nullptr)
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
    else
    {
        std::cerr << "Unknown command byte: " << (int)command_byte << '\n';
        return current_screen;
    }
}

int main()
{
    try
    {
        // Byte stream: Screen setup (80x24, 256 colors), Draw 'A' at (10, 5) in color 3
        std::vector<uint8_t> stream = {
            0x1, 3, 80, 24, 0x02, // Screen Setup
            0x2, 4, 10, 5, 3, 'A' // Draw Character
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
