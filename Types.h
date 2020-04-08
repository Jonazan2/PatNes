#pragma once

/* Basic types */

using byte = unsigned char;
using word = unsigned short;

using u64 = unsigned long long;
using i64 = long long;

using u32 = unsigned int;
using i32 = int;

using u16 = unsigned short;
using i16 = short;

union Register 
{
    struct 
    {
        byte low;
        byte hi;
    };
    word value;
};

struct RGB
{
    byte red;
    byte green;
    byte blue;

    bool isEqual( RGB other ) const
    {
        return red == other.red && green == other.green && blue == other.blue;
    }
};

namespace color
{
    constexpr RGB PINK = { 0xFF, 0x00, 0x80 };
}


/* Operators */

constexpr u64 operator"" _KB( u64 size )
{
    return size * 1024;
}
