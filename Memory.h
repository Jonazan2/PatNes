#pragma once

#include "Types.h"


/*

    +-------------------------------------------------------------------------------+
    |                                                                               |
    |  NES Memory map                                                               |
    |                                                                               |
    +---------------------+---------------------------------------------------------+
    |                     |                                                         |
    |   0x0000 - 0x07FF   |  ( 2KB internal RAM )                                   |
    |                     |                                                         |
    |   0x0800 - 0x0FFF   |  ( Mirrors of RAM )                                     |
    |                     |                                                         |
    |   0x1000 - 0x17FF   |  ( Mirrors of RAM )                                     |
    |                     |                                                         |
    |   0x1800 - 0x1FFF   |  ( Mirrors of RAM )                                     |
    |                     |                                                         |
    |   0x2000 - 0x2007   |  ( NES PPU registers )                                  |
    |                     |                                                         |
    |   0x2008 - 0x3FFF   |  ( Mirrors of PPU registers, repeat every 8 bytes )     |
    |                     |                                                         |
    |   0x4000 - 0x4017   |  ( NES APU & IO registers )                             |
    |                     |                                                         |
    |   0x4018 - 0x401F   |  ( APU and UI Functionality that is usually disabled )  |
    |                     |                                                         |
    |   0x4020 - 0xFFFF   |  ( Cartridge and mapper registers )                     |
    |                     |                                                         |
    +---------------------+---------------------------------------------------------+

 */


class Cartridge;
class Video;

class Memory
{
public:

    Memory( const Cartridge *cartridge, Video *video );
    ~Memory();

    void Reset();

    /* Memory management */
    byte Read( word address );
    void Write( word address, byte data );

    const byte *const GetMemoryMap() const;

private:

    /* Associated NES systems */
    const Cartridge     *cartridge;
    Video               *video;

    /* NES memory map */
    byte                *map;

    bool                IsAddressLatchClear;
    word                currentVRamAddress;

    void MapCartridge();
    void ResetAddressLatch();
};