#pragma once

#include "../Types.h"


class Cpu;

class CpuDebugger
{
public:

    CpuDebugger() = default;
    CpuDebugger( CpuDebugger & ) = delete;
    CpuDebugger( CpuDebugger && ) = delete;

    void ComposeView( Cpu &cpu, u32 cycles );
};
