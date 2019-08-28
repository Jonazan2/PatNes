#pragma once

#include "Memory.h"

#include <assert.h>
#include "Cartridge.h"


Memory::Memory( Cartridge *cartridge )
{
    map = new byte[ 64_KB ];
    memset( map, 0x00, 64_KB );

    MapCartridge( cartridge );
}

Memory::~Memory()
{
    delete map;
}

byte Memory::Read( word address ) const
{
    return map[ address ];
}

void Memory::Write( word address, byte data )
{
    map[ address ] = data;
}

const byte *const Memory::GetMemoryMap() const
{
    return map;
}

void Memory::MapCartridge( Cartridge *cartridge )
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
