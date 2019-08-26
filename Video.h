#pragma once

#include "Types.h"

/*

    +---------------------------------------------------+
    |                                                   |
    |  PPU memory map                                   |
    +---------------------+-----------------------------+
    |                     |                             |
    |  0x0000 - 0x0FFF    |  Pattern table 0            |
    |                     |                             |
    |  0x1000 - 0x1FFF    |  Pattern table 1            |
    |                     |                             |
    |  0x2000 - 0x23FF    |  Nametable 0                |
    |                     |                             |
    |  0x2400 - 0x27FF    |  Nametable 1                |
    |                     |                             |
    |  0x2800 - 0x2BFF    |  Nametable 2                |
    |                     |                             |
    |  0x2C00 - 0x2FFF    |  Nametable 3                |
    |                     |                             |
    |  0x3000 - 0x3EFF    |  Mirrors of 0x2000 0x2EFF   |
    |                     |                             |
    |  0x3F00 - 0x3F1F    |  Palette RAM indexes        |
    |                     |                             |
    |  0x3F20 - 0x3FFF    |  Mirrors of 0x3F00 0x3F1F   |
    |                     |                             |
    +---------------------+-----------------------------+

*/


class Cartridge;

class Video
{
public:
    Video( Cartridge *memory );

private:

    /* Associated Systems */
    Cartridge       *cartridge;

    /* PPU memory layout */
    byte            *memory;
    

    void MapCartridgeCHRToPPU();
};