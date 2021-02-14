#pragma once

#include "../Types.h"
#include "../Cpu.h"
#include "CpuDebugger.h"
#include "VideoDebugger.h"
#include "MemoryDebugger.h"


enum class DebuggerMode : byte 
{
    IDLE = 0,
    BREAKPOINT,
    V_SYNC,
    RUNNING
};

enum class DebuggerUpdateResult : byte
{
    QUIT = 0,
    CONTINUE,
    RESET
};


struct GLFWwindow;
class Video;

class Debugger
{
public:
    Debugger( Cpu *cpu, Memory *memory, Video *video );
    Debugger( Debugger & ) = delete;

    void StartDebugger();
    void ResetDebugger();
    DebuggerUpdateResult Update( float deltaMilliseconds, u32 cycles );
    void CloseDebugger();

private:
    
    /* Systems */
    Cpu             *cpu;
    Memory          *memory;
    Video           *video;

    /* Specific Debuggers */
    CpuDebugger     cpuDebugger;
    VideoDebugger   videoDebugger;
    MemoryDebugger  memoryDebugger;

    GLFWwindow      *window;
    DebuggerMode    mode;
    bool            reset;

    void ComposeView( u32 cycles );
    void ComposeEmulatorControlView();
    void Render();
    bool ShouldCloseWindow() const;
};