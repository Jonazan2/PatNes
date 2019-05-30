#pragma once

using byte = unsigned char;
using word = unsigned short;

using u64 = unsigned long long;
using i64 = long long;

using u32 = unsigned int;
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


namespace 
{

u64 operator"" _KB( u64 size )
{
    return size * 1024;
}

}