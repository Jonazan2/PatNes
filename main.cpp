#include <iostream>

#include "Cartridge.h"
#include "Memory.h"
#include "Cpu.h"
#include "Debugger/Debugger.h"


int main(int argc, char** argv)
{
    if ( argc < 2 )
    {
        std::cout << "Please provide the rom path";
        return -1;
    }

    Debugger debugger;
    debugger.StartDebugger();

    Cartridge cartridge( argv[1] );
    if ( cartridge.IsLoaded() )
    {
        cartridge.PrintDetails();

        Memory memory( &cartridge );
        Cpu cpu( &memory );
        
        short currentCycles = 0;
        while (currentCycles < 1000)
        {
            //currentCycles += cpu.Update();
            debugger.Update(0.f, currentCycles);
        }

        return 0;
    }

    std::cout << "The cartridge couldn't be loaded";
    return -1;
}