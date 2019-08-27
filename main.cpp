#include <iostream>

#include "Cartridge.h"
#include "Memory.h"
#include "Cpu.h"
#include "Video.h"
#include "Debugger/Debugger.h"

static constexpr u32 AVERAGE_CYCLES_PER_FRAME = 29780;

int main(int argc, char** argv)
{
    if ( argc < 2 )
    {
        std::cout << "Please provide the rom path";
        return -1;
    }


    Cartridge cartridge( argv[1] );
    if ( cartridge.IsLoaded() )
    {
        cartridge.PrintDetails();

        Memory memory( &cartridge );
        Cpu cpu( &memory );
        Video video( &cartridge );

        Debugger debugger( &cpu, &memory, &video );
        debugger.StartDebugger();

        /* Run the first frame for now */
        u32 currentCycles = 0;
        while ( currentCycles < AVERAGE_CYCLES_PER_FRAME )
        {
            currentCycles += cpu.Update();
            debugger.Update(0.f, currentCycles);
        }

        return 0;
    }

    std::cout << "The cartridge couldn't be loaded";
    return -1;
}