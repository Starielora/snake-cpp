#pragma once

struct vec2i
{
    int x, y;
};

bool equal(const vec2i& l, const vec2i& r)
{
    return l.x == r.x && l.y == r.y;
}

