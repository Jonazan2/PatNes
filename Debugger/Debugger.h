#pragma once

#include "../Types.h"
#include "../Cpu.h"
#include "CpuDebugger.h"

enum class DebuggerMode : byte {
    IDLE,
    BREAKPOINT,
    V_SYNC,
    RUNNING
};

struct GLFWwindow;

class Debugger
{
public:
    Debugger( Cpu *cpu );
    Debugger(Debugger &) = delete;

    void StartDebugger();
    void Update( float deltaMilliseconds, u32 cycles );
    void CloseDebugger();

private:
    
    /* Systems */
    Cpu             *cpu;
    
    /* Specific Debuggers */
    CpuDebugger     cpuDebugger;

    
    GLFWwindow      *window;
    DebuggerMode    mode;

    void AddFlagCheckbox( Cpu::Flags flag, const char *name );
    void ComposeView( u32 cycles );
    void Render();
};