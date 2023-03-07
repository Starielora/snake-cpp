#include "game_logic.hpp"
#include "windows_console_input.hpp"
#include "windows_stdout_render.hpp"
#include "bot.hpp"

#include <thread>
#include <chrono>
#include <functional>

int play_game(const std::function<vec2i(void)>& next_move, const vec2i& game_size)
{
    constexpr auto delay = std::chrono::milliseconds(500);

    auto game = game_logic::game_logic(game_size);
    auto render = windows_console_printer(game_size, game.snake());

    for(;;)
    {
        render.game_state(game.snake(), game.apple(), game.score(), game.max_score());

        if (game.win())
        {
            render.win();
            return 0;
        }

        std::this_thread::sleep_for(delay);
        const auto input = next_move();

        const auto collision = game.update(input);
        if (collision)
        {
            render.loss();
            return 1;
        }
    }
}

int main(int argc, const char** argv)
{
    constexpr auto h = 10;
    constexpr auto w = h;

    if (argc > 1 && 0 == strcmp(argv[1], "test"))
    {
        auto bot = game_logic::bot({ w, h });
        return play_game([bot]() mutable { return bot.next_move(); }, {w, h});
    }

    return play_game([]() { return windows_console_input::read(); }, {w, h});
}
