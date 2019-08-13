#pragma once

#include "../Types.h"
#include "../Cpu.h"
#include "CpuDebugger.h"

enum class DebuggerMode : byte 
{
    IDLE,
    BREAKPOINT,
    V_SYNC,
    RUNNING
};

struct GLFWwindow;

class Debugger
{
public:
    Debugger( Cpu *cpu, Memory *memory );
    Debugger(Debugger &) = delete;

    void StartDebugger();
    void Update( float deltaMilliseconds, u32 cycles );
    void CloseDebugger();

private:
    
    /* Systems */
    Cpu             *cpu;
    Memory          *memory;

    /* Specific Debuggers */
    CpuDebugger     cpuDebugger;

    GLFWwindow      *window;
    DebuggerMode    mode;

    void ComposeView( u32 cycles );
    void Render();
};