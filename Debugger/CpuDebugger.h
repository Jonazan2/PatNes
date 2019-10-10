#pragma once

#include <set>
#include <bitset>

#include "../Types.h"


class Cpu;
class Memory;

enum class DebuggerMode : byte;

class CpuDebugger
{
public:
    CpuDebugger();

    void GenerateDisassemblerInstructionMask( Memory &memory );
    void ComposeView( Cpu &cpu, Memory &memory, u32 cycles, DebuggerMode& mode );

    /* Breakpoint handling */
    bool HasAddressABreakpoint( word address ) const;

private:
    bool                    instructionJump;
    std::bitset< 0x10000 >  disassemblerInstructionMask;
    std::set< word >        breakpoints;

    bool IsAddresAnInstruction( u32 address ) const;
};
