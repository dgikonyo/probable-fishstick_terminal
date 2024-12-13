#include <iostream>
#include <vector>
#include <string>
#include "headers/Screen.h"
#include "headers/parse_screen_setup.h"
#include "headers/DrawCharacter.h"
#include "headers/DrawLine.h"
#include "headers/RenderText.h"
#include "headers/MoveCursor.h"
#include "headers/DrawAtCursor.h"
#include "headers/ClearScreen.h"

// Main function
int main()
{
    try
    {
        std::vector<uint8_t> stream = {
            0x1, 3, 80, 24, 0x02,                                                         // Screen Setup
            0x2, 4, 10, 5, 3, 'A',                                                        // Draw Character
            0x3, 6, 5, 5, 20, 10, 2, '*',                                                 // Draw Line
            0x4, 10, 2, 2, 3, 'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!', // Render Text
            0x5, 2, 15, 10,                                                               // Move Cursor
            0x6, 2, '@', 4,                                                               // Draw At Cursor
            0x7, 0,                                                                       // Clear Screen
            0xFF, 0                                                                       // End of Stream
        };

        Screen *screen = nullptr;

        size_t i = 0;
        while (i < stream.size())
        {
            uint8_t command_byte = stream[i++];
            uint8_t length_byte = stream[i++];
            std::vector<uint8_t> data(stream.begin() + i, stream.begin() + i + length_byte);
            i += length_byte;

            if (command_byte == 0x1)
            {
                uint8_t width, height;
                ColorMode color_mode;
                parse_screen_setup(data, width, height, color_mode);
                screen = new Screen(width, height, color_mode);
            }
            else if (command_byte == 0x2 && screen)
            {
                draw_character(*screen, data[0], data[1], static_cast<char>(data[3]), data[2]);
            }
            else if (command_byte == 0x3 && screen)
            {
                draw_line(*screen, data[0], data[1], data[2], data[3], static_cast<char>(data[5]), data[4]);
            }
            else if (command_byte == 0x4 && screen)
            {
                std::string text(data.begin() + 4, data.end());
                render_text(*screen, data[0], data[1], data[2], text);
            }
            else if (command_byte == 0x5 && screen)
            {
                move_cursor(*screen, data[0], data[1]);
            }
            else if (command_byte == 0x6 && screen)
            {
                draw_at_cursor(*screen, static_cast<char>(data[0]), data[1]);
            }
            else if (command_byte == 0x7 && screen)
            {
                clear_screen(*screen);
            }
            else if (command_byte == 0xFF)
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