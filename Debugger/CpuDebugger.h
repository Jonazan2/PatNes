#pragma once

#include <set>

#include "../Types.h"


class Cpu;
class Memory;

enum class DebuggerMode : byte;

class CpuDebugger
{
public:
    CpuDebugger();

    void ComposeView( Cpu &cpu, Memory &memory, u32 cycles, DebuggerMode& mode );

    /* Breakpoint handling */
    bool HasAddressABreakpoint( word address ) const;

private:
    bool instructionJump;
    std::set< word > breakpoints;
};
