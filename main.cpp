#include "game_logic.hpp"
#ifdef WIN32
#include "windows_console_input.hpp"
#endif
#include "stdout_render.hpp"
#include "bot.hpp"

#include "glm/glm.hpp"

#include <thread>
#include <chrono>
#include <functional>
#include <string.h>

int play_game(const std::function<glm::u32vec2(void)>& next_move, const glm::u32vec2& game_size)
{
    constexpr auto delay = std::chrono::milliseconds(10);

    auto game = game_logic::game_logic(game_size);
    auto render = console_printer(game_size, game.snake());

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

#ifdef WIN32
    return play_game([]() { return windows_console_input::read(); }, {w, h});
#endif
}
