#pragma once

#include "../Types.h"
#include "../Cpu.h"
#include "CpuDebugger.h"
#include "VideoDebugger.h"


enum class DebuggerMode : byte 
{
    IDLE,
    BREAKPOINT,
    V_SYNC,
    RUNNING
};

struct GLFWwindow;
class Video;

class Debugger
{
public:
    Debugger( Cpu *cpu, Memory *memory, Video *video );
    Debugger( Debugger & ) = delete;

    void StartDebugger();
    void Update( float deltaMilliseconds, u32 cycles );
    void CloseDebugger();

private:
    
    /* Systems */
    Cpu             *cpu;
    Memory          *memory;
    Video           *video;

    /* Specific Debuggers */
    CpuDebugger     cpuDebugger;
    VideoDebugger   videoDebugger;

    GLFWwindow      *window;
    DebuggerMode    mode;

    void ComposeView( u32 cycles );
    void Render();
};