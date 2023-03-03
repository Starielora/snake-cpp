#include <iostream>
#include <windows.h>
#include <thread>
#include <chrono>
#include <string>
#include <array>
#include <list>
#include <ranges>
#include <random>
#include <deque>
#include <algorithm>
#include <queue>

void clear_screen(char fill = ' ') { 
    COORD tl = {0,0};
    CONSOLE_SCREEN_BUFFER_INFO s;
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);   
    GetConsoleScreenBufferInfo(console, &s);
    DWORD written, cells = s.dwSize.X * s.dwSize.Y;
    FillConsoleOutputCharacter(console, fill, cells, tl, &written);
    FillConsoleOutputAttribute(console, s.wAttributes, cells, tl, &written);
    SetConsoleCursorPosition(console, tl);
}

void get_input(int& dx, int& dy)
{
    HANDLE console = GetStdHandle(STD_INPUT_HANDLE);
    DWORD eventsCount;
    GetNumberOfConsoleInputEvents(console, &eventsCount);
    DWORD recordsReadCount;
    INPUT_RECORD buffer[128];
    if (eventsCount > 0)
    {
        if (!ReadConsoleInput(
            console,      // input buffer handle
            buffer,     // buffer to read into
            sizeof(buffer) / sizeof(INPUT_RECORD),         // size of read buffer
            &recordsReadCount)) // number of records read
            std::abort();

        // Dispatch the events to the appropriate handler.

        for (int i = 0; i < recordsReadCount; i++)
        {
            switch (buffer[i].EventType)
            {
            case KEY_EVENT:
            {   // keyboard input
                const auto keyCode = buffer[i].Event.KeyEvent.wVirtualKeyCode;
                if (keyCode == VK_UP && dy != 1)
                {
                    dy = -1;
                    dx = 0;
                }
                else if (keyCode == VK_DOWN && dy != -1)
                {
                    dy = 1;
                    dx = 0;
                }
                else if (keyCode == VK_LEFT && dx != 1)
                {
                    dx = -1;
                    dy = 0;
                }
                else if (keyCode == VK_RIGHT && dx != -1)
                {
                    dx = 1;
                    dy = 0;
                }
                else
                    std::cout << "Unknown key pressed";
                break;
            }
            default:
                std::cout << "Unknown event type";
                break;
            }

            return; // respect only first input to avoid handling "key up event"
        }
    }
}

struct Vec2i
{
    int x, y;
};

void draw(char c, int x, int y, int w, std::string& area)
{
    const int index = y * 2 * w + 2*x;
    area[index] = c;
}

void draw(const std::list<Vec2i>& snake, int w, std::string& area)
{
    static Vec2i previousTail;
    draw(' ', previousTail.x, previousTail.y, w, area);
    previousTail = snake.back();

    for (const auto& node : snake)
    {
        draw('o', node.x, node.y, w, area);
    }
}

bool check_collision(const std::list<Vec2i>& snake, Vec2i gameSize)
{
    const auto& head = snake.front();
    if (head.x == -1 || head.y == -1 || head.x == gameSize.x || head.y == gameSize.y)
        return true;

    for (std::size_t i = 0;const auto& node : std::ranges::reverse_view(snake))
    {
        if (++i == snake.size())
            break;
        if (head.x == node.x && head.y == node.y)
            return true;
    }

    return false;
}

bool update_snake(std::list<Vec2i>& snake, Vec2i delta, Vec2i& apple, std::size_t& score, Vec2i gameSize, std::deque<Vec2i>& availableCells, std::queue<std::size_t>& randomIndices)
{
    auto head = snake.front();
    head.x += delta.x;
    head.y += delta.y;
    snake.push_front(head);

    if (check_collision(snake, gameSize))
        return true;

    const auto newHead = snake.front();

	const auto headIt = std::find_if(availableCells.begin(), availableCells.end(), [head = newHead](const auto& node) { return node.x == head.x && node.y == head.y; });
	availableCells.erase(headIt);

	if (apple.x == newHead.x && apple.y == newHead.y)
	{
        score++;
        if (availableCells.empty())
        {
            return false;
        }
        apple = availableCells.at(randomIndices.front());
        randomIndices.pop();
	}
    else
    {
		availableCells.push_back(snake.back());
		snake.pop_back();
    }

    return false;
}

std::deque<Vec2i> generate_available_cells(int w, int h, const std::list<Vec2i>& snake)
{
    auto allCells = std::deque<Vec2i>{};
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            allCells.push_back(Vec2i{ .x = x, .y = y });
        }
    }

    // TODO no std::less operator :(
    //std::set_difference(allCells.begin(), allCells.end(), snake.begin(), snake.end(), std::back_inserter(availableCells));
    for (const auto& node : snake)
    {
        const auto removeIt = std::remove_if(allCells.begin(), allCells.end(), [&node](const auto& cell) { return cell.x == node.x && cell.y == node.y; });

        allCells.erase(removeIt);
    }

    return allCells;
}

std::queue<std::size_t> generate_available_cells_random_indices(std::size_t size)
{
    std::queue<std::size_t> indices;
    std::random_device rd;
    std::mt19937 gen(rd());

    for (std::size_t i = size - 1; i > 0; --i)
    {
        // generate from range one less for each apple
        // to ensure index is always valid in availableCells
		std::uniform_int_distribution<> rand(0, i);
		const auto randIndex = rand(gen);
        indices.push(randIndex);
    }
    
    indices.push(0);

    return indices;
}

Vec2i generate_apple(const std::deque<Vec2i>& availableCells, std::queue<std::size_t>& randomIndices)
{
    const auto apple = availableCells.at(randomIndices.front());
    randomIndices.pop();
    return apple;
}

void render_scene(const std::list<Vec2i>& snake, const Vec2i& apple, const Vec2i& gameSize, std::size_t score, std::size_t maxScore, std::string& gridBuffer)
{
	clear_screen();
	draw('@', apple.x, apple.y, gameSize.x, gridBuffer);
	draw(snake, gameSize.x, gridBuffer);

    std::cout << '+' << std::string(2 * gameSize.x, '-') << "+\n";
	for(int i = 0; i < gameSize.y; ++i)
	{
		std::cout << '|' << std::string_view(gridBuffer.begin() + i * 2 * gameSize.x, gridBuffer.begin() + (i + 1) * 2 * gameSize.x) << "|\n";
	}
    std::cout << '+' << std::string(2 * gameSize.x, '-') << "+\n";
	std::cout << "Score: " << score << "/" << maxScore << '\n';
}

int main()
{
    constexpr auto h = 10;
    constexpr auto w = h;
    constexpr auto initialSnake = std::array{ Vec2i{1,0}, Vec2i{0,0} };
    constexpr auto maxScore = w * h - initialSnake.size();
    auto dx = 1, dy = 0;
    auto gridBuffer = std::string(2 * w * h, ' ');

    auto snake = std::list<Vec2i>{};
    for (const auto& node : initialSnake)
        snake.push_back(node);

    auto availableCells = generate_available_cells(w, h, snake);
    auto randomIndices = generate_available_cells_random_indices(availableCells.size());
    auto apple = generate_apple(availableCells, randomIndices);

    auto score = std::size_t{ 0 };

    for(;;)
    {
        render_scene(snake, apple, { w, h }, score, maxScore, gridBuffer);
        // kekw input buffering
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        get_input(dx, dy);

        if (availableCells.empty())
        {
            std::cout << "You won!" << std::endl;
            return 0;
        }

        const auto collision = update_snake(snake, { dx, dy }, apple, score, { w, h }, availableCells, randomIndices);
        if (collision)
        {
            std::cout << "You lost!" << std::endl;
            return 1;
        }
    }
}
