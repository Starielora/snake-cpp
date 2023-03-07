#pragma once
#include "vec2i.h"

#include <string>
#include <list>
#include <iostream>

#ifdef WIN32
#include "console_windows.hpp"
#elif linux
#include "console_linux.hpp"
#endif

class console_printer final
{
    static constexpr auto apple_char = '@';
    static constexpr auto snake_char = 'o';
    static constexpr auto frame_corner_char = '+';
    static constexpr auto frame_vertical_char = '|';
    static constexpr auto frame_horizontal_char = '-';
    static constexpr auto newline_char = '\n';

public:
    console_printer(const vec2i& game_size, const std::list<vec2i>& initial_snake)
        : _game_size(game_size)
        , _buffer((2*game_size.x + 3) * (game_size.y + 2), ' ')
        , _previous_tail(initial_snake.back())
    {
        clear_console();
        hide_cursor();
        init_game_buffer(game_size, initial_snake);
    }

    ~console_printer()
    {
        // TODO restore terminal state in general
        // also handle signals like abort etc.
        show_cursor();
    }

    void game_state(const std::list<vec2i>& snake, const vec2i& apple, std::size_t score, std::size_t max_score)
    {
        reset_console_cursor_pos();

        draw(snake);
        draw(apple_char, apple);

        std::cout.write(_buffer.data(), _buffer.size());
        // Profiler says that printing formatted oputput is super bad
        //std::cout << "Score: " << score << '/' << max_score << '\n';
        std::cout << std::flush;
    }

    void win()
    {
        std::cout << "You won!" << std::endl;
    }

    void loss()
    {
        std::cout << "You lost!" << std::endl;
    }

private:
    void init_game_buffer(const vec2i& game_size, const std::list<vec2i>& initial_snake)
    {
        auto it = _buffer.begin();

        // fill first row
        *it = frame_corner_char;
        for (std::size_t x = 0; x < 2 * game_size.x; ++x)
        {
            *(++it) = frame_horizontal_char;
        }
        *(++it) = frame_corner_char;
        *(++it) = newline_char;

        for (std::size_t y = 0; y < game_size.y; ++y)
        {
            // fill left border char
            *(++it) = frame_vertical_char;
            const auto beg = ++it;
            const auto end = it + 2 * game_size.x;

            it = end;
            // fill right border char
            *(it) = frame_vertical_char;
            *(++it) = newline_char;
        }

        // fill last row
        *(++it) = frame_corner_char;
        for (std::size_t x = 0; x < 2 * game_size.x; ++x)
        {
            *(++it) = frame_horizontal_char;
        }
        *(++it) = frame_corner_char;
        *(++it) = newline_char;

        for (const auto& node : initial_snake)
        {
            draw(snake_char, node);
        }
    }

    std::size_t game_xy_to_buffer_index(const vec2i& pos) const
    {
        // first skip first row: 2 char wide game cells + 2 char for frame + 1 for newline
        // then skip y times such rows as first one
        // lastly skip frame char, x double game cells and add 1 to print on right side of double game cell
        // alternative: (2 * w + 3) * (pos.y + 1) + 2 * pos.x + 2;
        const auto w = _game_size.x;
        return 2 * w + 3 + pos.y * (2 * w + 3) + 1 + 2 * pos.x + 1;

    }

    void draw(char c, const vec2i& pos)
    {
        const auto index = game_xy_to_buffer_index(pos);
        _buffer[index] = c;
    }

    void draw(const std::list<vec2i>& snake)
    {
        draw(' ', { _previous_tail.x, _previous_tail.y });
        _previous_tail = snake.back();

        draw(snake_char, snake.front());
        draw(snake_char, snake.back());
    }

private:
    const vec2i _game_size;
    std::string _buffer;
    vec2i _previous_tail;
};

