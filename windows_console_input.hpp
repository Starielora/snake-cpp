#include "vec2i.h"

#include <windows.h>

namespace windows_console_input
{
    vec2i read()
    {
        HANDLE console = GetStdHandle(STD_INPUT_HANDLE);
        DWORD eventsCount;
        GetNumberOfConsoleInputEvents(console, &eventsCount);
        DWORD recordsReadCount;
        INPUT_RECORD buffer[128];
        if (eventsCount > 0)
        {
            if (!ReadConsoleInput(
                console, // input buffer handle
                buffer, // buffer to read into
                sizeof(buffer) / sizeof(INPUT_RECORD), // size of read buffer
                &recordsReadCount)) // number of records read
            {
                // TODO send help
                exit(GetLastError());
            }

            // respect only first input to avoid handling "key up event"
            switch (buffer[0].EventType)
            {
            case KEY_EVENT:
            {   // keyboard input
                const auto keyCode = buffer[0].Event.KeyEvent.wVirtualKeyCode;
                if (keyCode == VK_UP)
                {
                    return { 0, -1 };
                }
                else if (keyCode == VK_DOWN)
                {
                    return { 0, 1 };
                }
                else if (keyCode == VK_LEFT)
                {
                    return { -1, 0 };
                }
                else if (keyCode == VK_RIGHT)
                {
                    return { 1, 0 };
                }
                break;
            }
            default:
                break;
            }
        }
        return { 0, 0 };
    }
}
