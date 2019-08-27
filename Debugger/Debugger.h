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
class Video;

class Debugger
{
public:
    Debugger( Cpu *cpu, Memory *memory, Video *video );
    Debugger(Debugger &) = delete;
    ~Debugger();


    void StartDebugger();
    void Update( float deltaMilliseconds, u32 cycles );
    void CloseDebugger();

private:
    
    using ImTextureID = void *;
    
    /* Systems */
    Cpu             *cpu;
    Memory          *memory;
    Video           *video;

    /* Specific Debuggers */
    CpuDebugger     cpuDebugger;

    /* Textures */
    ImTextureID     leftPatternTableTextureID;
    RGB             *leftPatternTableBuffer;

    ImTextureID     rightPatternTableTextureID;
    RGB             *rightPatternTableBuffer;

    GLFWwindow      *window;
    DebuggerMode    mode;

    void ComposeView( u32 cycles );
    void Render();

    void UpdatePatternTable( word address, RGB *buffer );
};