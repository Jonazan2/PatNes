#pragma once

#include "../Types.h"
#include "../Cpu.h"

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
    void Update( float deltaMilliseconds, unsigned int cycles );
    void CloseDebugger();

private:

    Cpu             *cpu;

    GLFWwindow      *window;
    DebuggerMode    mode;

    void AddFlagCheckbox( Cpu::Flags flag, const char *name );
    void ComposeView( float deltaMilliseconds, unsigned int cycles );
    void Render();
};