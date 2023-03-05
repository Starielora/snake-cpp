#include "game_logic.hpp"
#include "windows_console_input.hpp"
#include "windows_stdout_render.hpp"

#include <thread>
#include <chrono>

int main()
{
    constexpr auto h = 10;
    constexpr auto w = h;
    constexpr auto delay = std::chrono::milliseconds(100);

    auto game = game_logic::game_logic(w, h);
    auto render = windows_console_printer({ w, h }, game.snake());

    for(;;)
    {
        render.game_state(game.snake(), game.apple(), game.score(), game.max_score());

        if (game.win())
        {
            render.win();
            return 0;
        }

        // TODO some kind of input buffering would probably improve UX
        std::this_thread::sleep_for(delay);
        const auto input = windows_console_input::read();

        const auto collision = game.update(input);
        if (collision)
        {
            render.loss();
            return 1;
        }
    }
}
