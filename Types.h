#pragma once

using byte = unsigned char;
using word = unsigned short;

using ui64 = unsigned long long;
using i64 = long long;

using ui32 = unsigned int;
using i32 = int;

union Register 
{
    struct 
    {
        byte low;
        byte hi;
    };
    word value;
};
