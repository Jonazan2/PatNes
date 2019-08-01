#pragma once

#include "../Types.h"


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
    Debugger();
    Debugger(Debugger &) = delete;

    void StartDebugger();
    void Update( float deltaMilliseconds, unsigned int cycles );
    void CloseDebugger();

private:
    GLFWwindow* window;
    DebuggerMode mode;


    void ComposeView( float deltaMilliseconds, unsigned int cycles );
    void Render();
};