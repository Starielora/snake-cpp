#pragma once
#include "vec2i.h"

#include <array>
#include <list>
#include <deque>
#include <queue>
#include <random>
#include <algorithm>

namespace game_logic
{
    class game_logic
    {
    private:
        static auto create_initial_snake()
        {
            auto snake = std::list<vec2i>{};
            for (const auto& node : initial_snake)
            {
                snake.push_back(node);
            }
            return snake;
        }

        static auto generate_available_cells(const vec2i& game_size, const std::list<vec2i>& snake)
        {
            auto available_cells = std::deque<vec2i>{};
            for (int y = 0; y < game_size.y; ++y)
            {
                for (int x = 0; x < game_size.x; ++x)
                {
                    available_cells.push_back({ .x = x, .y = y });
                }
            }

            // TODO no std::less operator :(
            //std::set_difference(allCells.begin(), allCells.end(), snake.begin(), snake.end(), std::back_inserter(availableCells));
            for (const auto& node : snake)
            {
                const auto removeIt = std::remove_if(available_cells.begin(), available_cells.end(), [&node](const auto& cell) { return equal(cell, node); });

                available_cells.erase(removeIt);
            }

            return available_cells;
        }

        static auto generate_random_indices_for_available_cells(std::size_t size)
        {
            std::queue<std::size_t> indices;
            std::random_device rd;
            std::mt19937 gen(rd());

            for (std::size_t i = size - 1; i > 0; --i)
            {
                // generate from range one less for each apple
                // to ensure index is always valid in availableCells
                std::uniform_int_distribution<> rand(0, i);
                indices.push(rand(gen));
            }

            indices.push(0);

            return indices;
        }

    public:
        game_logic(int width, int height)
            : _game_size({ width, height })
            , _max_score(_game_size.x * _game_size.y - initial_snake.size())
            , _delta({ 1, 0 })
            , _score(0)
            , _snake(create_initial_snake())
            , _available_cells(generate_available_cells(_game_size, _snake))
            , _random_indices(generate_random_indices_for_available_cells(_available_cells.size()))
            , _apple(get_next_apple())
        {
        }

        auto score() const { return _score; }
        auto max_score() const { return _max_score; }
        bool win() { return _score == _max_score; }
        const auto& snake() const { return _snake; }
        const auto& apple() const { return _apple; }

        bool update(const vec2i& input)
        {
            process_input(input);

            auto head = _snake.front();
            auto tail = _snake.back();
            head.x += _delta.x;
            head.y += _delta.y;

            _snake.push_front(head);

            if (equal(head, _apple))
            {
                _score++;
                if (_score == _max_score)
                {
                    return false;
                }
                _apple = get_next_apple();
                return false;
            }
            else
            {
                _snake.pop_back();
                _available_cells.push_back(tail);

                const auto head_it = std::find_if(_available_cells.begin(), _available_cells.end(), [head](const auto& node) { return equal(head, node); });

                if (head_it == _available_cells.end()) // it's a collision
                {
                    return true;
                }

                _available_cells.erase(head_it);
                return false;
            }
        }

    private:
        vec2i get_next_apple()
        {
            const auto index = _random_indices.front();
            const auto apple = _available_cells.at(index);
            _available_cells.erase(_available_cells.begin() + index);
            _random_indices.pop();
            return apple;
        }

        void process_input(const vec2i& input)
        {
            if (equal(input, {0, 0}))
                return;

            // check illegal input - snake going [1,0] cannot go [-1,0] etc.
            const auto resultant_x = input.x + _delta.x;
            const auto resultant_y = input.y + _delta.y;
            if (resultant_x == 0 && resultant_y == 0)
                return;

            _delta = input;
        }

    private:
        static constexpr auto initial_snake = std::array{ vec2i{1,0}, vec2i{0,0} };
        const vec2i _game_size;
        const std::size_t _max_score;
        vec2i _delta;
        std::size_t _score{ 0 };
        std::list<vec2i> _snake;
        std::deque<vec2i> _available_cells;
        std::queue<std::size_t> _random_indices;
        vec2i _apple;
    };
}
