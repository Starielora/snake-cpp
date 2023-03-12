#pragma once
#include "glm/glm.hpp"

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
            auto snake = std::list<glm::u32vec2>{};
            for (const auto& node : initial_snake)
            {
                snake.push_back(node);
            }
            return snake;
        }

        static auto generate_available_cells(const glm::u32vec2& game_size, const std::list<glm::u32vec2>& snake)
        {
            auto available_cells = std::deque<glm::u32vec2>{};
            for (int y = 0; y < game_size.y; ++y)
            {
                for (int x = 0; x < game_size.x; ++x)
                {
                    available_cells.push_back({ x, y });
                }
            }

            // TODO no std::less operator :(
            //std::set_difference(allCells.begin(), allCells.end(), snake.begin(), snake.end(), std::back_inserter(availableCells));
            for (const auto& node : snake)
            {
                const auto removeIt = std::remove_if(available_cells.begin(), available_cells.end(), [&node](const auto& cell) { return glm::all(glm::equal(cell, node)); });

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
        game_logic(const glm::u32vec2& game_size)
            : _game_size(game_size)
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
        bool win() { return _available_cells.size() == 0; }
        const auto& snake() const { return _snake; }
        const auto& apple() const { return _apple; }

        bool update(const glm::u32vec2& input)
        {
            process_input(input);

            auto head = _snake.front();
            auto tail = _snake.back();
            head.x += _delta.x;
            head.y += _delta.y;

            _snake.push_front(head);

            if (glm::all(glm::equal(head, _apple)))
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

                const auto head_it = std::find_if(_available_cells.begin(), _available_cells.end(), [head](const auto& node) { return glm::all(glm::equal(head, node)); });

                if (head_it == _available_cells.end()) // it's a collision
                {
                    return true;
                }

                _available_cells.erase(head_it);
                return false;
            }
        }

    private:
        glm::u32vec2 get_next_apple()
        {
            const auto index = _random_indices.front();
            const auto apple = _available_cells.at(index);
            _available_cells.erase(_available_cells.begin() + index);
            _random_indices.pop();
            return apple;
        }

        void process_input(const glm::u32vec2& input)
        {
            if (glm::all(glm::equal(input, {0, 0})))
                return;

            // check illegal input - snake going [1,0] cannot go [-1,0] etc.
            const auto resultant_x = input.x + _delta.x;
            const auto resultant_y = input.y + _delta.y;
            if (resultant_x == 0 && resultant_y == 0)
                return;

            _delta = input;
        }

    private:
        static constexpr auto initial_snake = std::array{ glm::u32vec2{1,0}, glm::u32vec2{0,0} };
        const glm::u32vec2 _game_size;
        const std::size_t _max_score;
        glm::u32vec2 _delta;
        std::size_t _score{ 0 };
        std::list<glm::u32vec2> _snake;
        std::deque<glm::u32vec2> _available_cells;
        std::queue<std::size_t> _random_indices;
        glm::u32vec2 _apple;
    };
}
