#pragma once

#include "../Types.h"


class Cpu;

enum class DebuggerMode : byte;

class CpuDebugger
{
public:
    CpuDebugger();

    void ComposeView( Cpu &cpu, u32 cycles, DebuggerMode& mode );

private:
    bool instructionJump;
};
