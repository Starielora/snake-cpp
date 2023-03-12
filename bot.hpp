#pragma once
#include "glm/vec2.hpp"
#include <vector>

// TODO works only on even width game size
namespace game_logic
{
    struct bot
    {
        bot(const glm::u32vec2& game_size)
            : _moves(generate_moves(game_size))
            , _current_move(0)
        {

        }

        glm::u32vec2 next_move()
        {
            // I want to avoid branch here so badly :(
            if (_current_move == _moves.size())
                _current_move = 0;
            return _moves[_current_move++];
        }

    private:
        static std::vector<glm::u32vec2> generate_moves(const glm::u32vec2& game_size)
        {
            auto v = std::vector<glm::u32vec2>{};

            const auto none = glm::u32vec2{ 0, 0 };
            const auto down = glm::u32vec2{ 0, 1 };
            const auto left = glm::u32vec2{ -1, 0 };
            const auto up = glm::u32vec2{ 0, -1 };
            const auto right = glm::u32vec2{ 1, 0 };

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
        std::vector<glm::u32vec2> _moves;
        std::size_t _current_move;
    };
}
