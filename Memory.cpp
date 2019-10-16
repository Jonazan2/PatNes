#pragma once

#include "Memory.h"

#include <assert.h>
#include "Cartridge.h"
#include "Video.h"


Memory::Memory( const Cartridge *cartridge )
    : cartridge( cartridge )
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

    MapCartridge();
}

byte Memory::Read( word address ) const
{
    if ( address >= 0x2000 && address <= 0x3FFF )
    {
        const word mirroredAddress = ( address % 8 ) + 0x2000;
        return map[ mirroredAddress ];
    }
    else
    {
        return map[ address ];
    }
}

void Memory::Write( word address, byte data )
{
    if ( address >= 0x2000 && address <= 0x3FFF )
    {
        const word mirroredAddress = ( address % 8 ) + 0x2000;
        map[ mirroredAddress ] = data;
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

void Memory::MapCartridge()
{
    assert( cartridge != nullptr );

    /* For now only support NROM with PRG ROM of 16KB and no ram */
    Cartridge::Header header = cartridge->GetHeader();
    assert( header.mapper == 0x00 && header.prgRomSizeKB == 16  && !header.hasPRGRam);

    const byte * const rom = cartridge->GetRom();

    /* Map the PRG ROM to 0x8000 */
    memcpy(&map[0x8000], &rom[0x0010], 16_KB );

    /* Mirror the PRG ROM in 0xC000 */
    memcpy(&map[0xC000], &rom[0x0010], 16_KB );
}
