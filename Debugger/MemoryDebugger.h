#pragma once

#include "../Types.h"

class Memory;

class MemoryDebugger
{
public:
    MemoryDebugger() = default;

    void ComposeView( const Memory *memory );

private:
    static constexpr u32 MEMORY_VIEW_ROWS = 16;
    static constexpr u32 MEMORY_VIEW_MEMORY_SIZE = 0x10000;
    static constexpr u32 MEMORY_VIEW_BASE_ADDRESS = 0x0000;
};