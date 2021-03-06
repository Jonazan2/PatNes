#include <iostream>

#include "Cartridge.h"
#include "Memory.h"
#include "Cpu.h"
#include "CpuTypes.h"
#include "Video.h"
#include "Debugger/Debugger.h"

#include <assert.h>


int main(int argc, char** argv)
{
    if ( argc < 2 )
    {
        std::cout << "Please provide the rom path";
        return -1;
    }

    Cartridge cartridge( argv[1] );
    if ( !cartridge.IsLoaded() )
    {
        std::cout << "The cartridge couldn't be loaded";
        return -1;
    }


    cartridge.PrintDetails();

    Video video( &cartridge );
    Memory memory( &cartridge, &video );
    video.Init( &memory );
    Cpu cpu( &memory );

    Debugger debugger( &cpu, &memory, &video );
    debugger.StartDebugger();

    bool quit = false;
    u32 currentCycles = 0;
    while ( !quit )
    {
        currentCycles += cpu.Update();
        video.Update( currentCycles );

        const DebuggerUpdateResult result = debugger.Update( 0.f, currentCycles );
        switch ( result )
        {
            case DebuggerUpdateResult::QUIT:
            {
                quit = true;
            }
            break;

            case DebuggerUpdateResult::RESET:
            {
                debugger.ResetDebugger();
                memory.Reset();
                video.Reset();
                cpu.Reset();
                currentCycles = 0;
            }
            break;

            case DebuggerUpdateResult::CONTINUE:
            default:
            {
                // Nothing to do here
            }
        }

        if ( currentCycles >= AVERAGE_CYCLES_PER_FRAME )
        {
            currentCycles = 0;
        }
    }

    return 0;
}