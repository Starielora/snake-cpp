#pragma once
#include "vec2i.h"
#include <vector>

// TODO works only on even width game size
namespace game_logic
{
    struct bot
    {
        bot(const vec2i& game_size)
            : _moves(generate_moves(game_size))
            , _current_move(0)
        {

        }

        vec2i next_move()
        {
            // I want to avoid branch here so badly :(
            if (_current_move == _moves.size())
                _current_move = 0;
            return _moves[_current_move++];
        }

    private:
        static std::vector<vec2i> generate_moves(const vec2i& game_size)
        {
            auto v = std::vector<vec2i>{};

            const auto none = vec2i{ 0, 0 };
            const auto down = vec2i{ 0, 1 };
            const auto left = vec2i{ -1, 0 };
            const auto up = vec2i{ 0, -1 };
            const auto right = vec2i{ 1, 0 };

            for (int i = 0; i < game_size.x - 2; ++i)
            {
                v.push_back(none);
            }

            v.push_back(down);

            for (int i = 0; i < game_size.y - 2; ++i)
            {
                v.push_back(none);
            }

            for (int j = 0; j < (game_size.x / 2) - 1; ++j)
            {
                v.push_back(left);
                v.push_back(up);

                for (int i = 0; i < game_size.y - 3; ++i)
                {
                    v.push_back(none);
                }

                v.push_back(left);
                v.push_back(down);

                for (int i = 0; i < game_size.y - 3; ++i)
                {
                    v.push_back(none);
                }
            }

            v.push_back(left);
            v.push_back(up);

            for (int i = 0; i < game_size.y - 2; ++i)
            {
                v.push_back(none);
            }

            v.push_back(right);

            return v;
        }

    private:
        std::vector<vec2i> _moves;
        std::size_t _current_move;
    };
}
