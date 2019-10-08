#pragma once

#include "Memory.h"

#include <assert.h>
#include "Cartridge.h"
#include "Video.h"


Memory::Memory( const Cartridge &cartridge, Video &video )
    : cartridge( cartridge )
    , video( video )
{
    map = new byte[ 64_KB ];
    Reset();
}

Memory::~Memory()
{
    delete[] map;
}

void Memory::Reset()
{
    memset( map, 0x00, 64_KB );

    MapCartridge( cartridge );
}

byte Memory::Read( word address ) const
{
    if ( address >= 0x2000 && address <= 0x2007 ) 
    {
        /* PPU memory */
        return video.Read( address );
    }
    else
    {
        return map[ address ];
    }
}

void Memory::Write( word address, byte data )
{
    if ( address >= 0x2000 && address <= 0x2007 ) 
    {
        /* PPU memory */
        video.Write( address, data );
    }
    else
    {
        map[ address ] = data;
    }
}

const byte *const Memory::GetMemoryMap() const
{
    return map;
}

void Memory::MapCartridge( const Cartridge &cartridge )
{
    /* For now only support NROM with PRG ROM of 16KB and no ram */
    Cartridge::Header header = cartridge.GetHeader();
    assert( header.mapper == 0x00 && header.prgRomSizeKB == 16  && !header.hasPRGRam);

    const byte * const rom = cartridge.GetRom();

    /* Map the PRG ROM to 0x8000 */
    memcpy(&map[0x8000], &rom[0x0010], 16_KB );

    /* Mirror the PRG ROM in 0xC000 */
    memcpy(&map[0xC000], &rom[0x0010], 16_KB );
}
