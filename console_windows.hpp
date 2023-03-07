#include <windows.h>

void clear_console()
{
    static const auto handle = GetStdHandle(STD_OUTPUT_HANDLE);
    // https://learn.microsoft.com/en-us/windows/console/clearing-the-screen
    constexpr PCWSTR clear_console_sequence = L"\x1b[2J";
    DWORD written = 0;
    if (!WriteConsoleW(handle, clear_console_sequence, (DWORD)wcslen(clear_console_sequence), &written, NULL))
    {
        // TODO send help...
        exit(GetLastError());
    }
}

void hide_cursor()
{
    static const auto handle = GetStdHandle(STD_OUTPUT_HANDLE);
    constexpr PCWSTR hide_cursor_sequence = L"\033[?25l";
    DWORD written = 0;
    // TODO this alters terminal state which should be recovered after exit/crash...
    if (!WriteConsoleW(handle, hide_cursor_sequence, (DWORD)wcslen(hide_cursor_sequence), &written, NULL))
    {
        // TODO send help...
        exit(GetLastError());
    }
}

void show_cursor()
{
    static const auto handle = GetStdHandle(STD_OUTPUT_HANDLE);
    constexpr PCWSTR hide_cursor_sequence = L"\033[?25h";
    DWORD written = 0;
    // TODO this alters terminal state which should be recovered after exit/crash...
    if (!WriteConsoleW(handle, hide_cursor_sequence, (DWORD)wcslen(hide_cursor_sequence), &written, NULL))
    {
        // TODO send help...
        exit(GetLastError());
    }
}

void reset_console_cursor_pos()
{
    static const auto handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(handle, {0, 0});
}
