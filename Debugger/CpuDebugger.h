#pragma once

#include "../Types.h"


class Cpu;
class Memory;

enum class DebuggerMode : byte;

class CpuDebugger
{
public:
    CpuDebugger();

    void ComposeView( Cpu &cpu, Memory &memory, u32 cycles, DebuggerMode& mode );

private:
    bool instructionJump;
};
