#include "Memory.h"

#include <assert.h>
#include "Cartridge.h"
#include "Video.h"
#include <cstring>

Memory::Memory( const Cartridge *cartridge, Video *video )
    : cartridge( cartridge )
    , video ( video )
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

    map[ Video::PPUCTRL_REGISTER ] = 0x00;
    map[ Video::PPUMASK_REGISTER ] = 0x00;
    map[ Video::PPUSTATUS_REGISTER ] = 0b1010'0000;
    map[ Video::OAMA_REGISTER ] = 0x00;
    map[ Video::OAMADATA_REGISTER ] = 0x00;
    map[ Video::PPUSCROLL_REGISTER ] = 0x00;
    map[ Video::PPUDATA_ADDRESS ] = 0x00;
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

byte Memory::Read( word address )
{
    if ( address >= 0x2000 && address <= 0x3FFF )
    {
        const word ppuRegister = ( address % 8 ) + 0x2000;

        if ( ppuRegister == Video::PPUSTATUS_REGISTER )
        {
            ResetAddressLatch();
            const byte ppuStatus = map[ Video::PPUSTATUS_REGISTER ];
            byte newPpuStatus = ppuStatus ^ 0b1000'0000;
            Write( Video::PPUSTATUS_REGISTER, newPpuStatus );
        }
        return map[ ppuRegister ];
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
        const word ppuRegister = ( address % 8 ) + 0x2000;
        if ( ppuRegister == Video::PPUADDR_REGISTER )
        {
            if ( IsAddressLatchClear )
            {
                IsAddressLatchClear = false;
                currentVRamAddress = ( data | currentVRamAddress ) << 8;
            }
            else
            {
                IsAddressLatchClear = true;
                currentVRamAddress = data | currentVRamAddress;
            }
            map[ Video::PPUADDR_REGISTER ] = data;

        }
        else if ( ppuRegister == Video::PPUDATA_ADDRESS )
        {
            const word address = currentVRamAddress;
            video->Write( address, data );

            const byte ppuControlRegister = map[ Video::PPUCTRL_REGISTER ];
            const byte incrementType = ( ppuControlRegister & 0b0000'0100 ) >> 2;
            if ( incrementType == 0 )
            {
                currentVRamAddress++;
            }
            else
            {
                currentVRamAddress += 32;
            }
        }
        else
        {
            map[ ppuRegister ] = data;
        }
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

void Memory::ResetAddressLatch()
{
    IsAddressLatchClear = true;
    currentVRamAddress = 0x00;
}
