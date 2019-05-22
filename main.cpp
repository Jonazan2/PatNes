#include <iostream>

#include "Cartridge.h"

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
        return 0;
    }

    std::cout << "The cartridge couldn't be loaded";
    return -1;
}