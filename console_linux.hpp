#include <iostream>

void clear_console()
{
    std::cout << "\033c";
}

void hide_cursor()
{
    std::cout << "\033[?25l";
}

void show_cursor()
{
    std::cout << "\033[?25h";
}

void reset_console_cursor_pos()
{
    std::cout << "\033[0;0f";
}
