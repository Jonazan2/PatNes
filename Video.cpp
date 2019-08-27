#include "Video.h"


#include <assert.h>

#include "Cartridge.h"


Video::Video( Cartridge *cartridge )
    : cartridge( cartridge )
{
    memory = new byte[ 16_KB ];
    memset( memory, 0x00, 16_KB );
    MapCartridgeCHRToPPU();
}

void Video::MapCartridgeCHRToPPU()
{
    assert( cartridge != nullptr );

    /* TODO(Jonathan): Support more mappers here */
    const Cartridge::Header &header = cartridge->GetHeader();
    const byte * const cartridgeRom = cartridge->GetRom();
    const u32 offset = (header.prgRomSizeKB * 1_KB) + 0x0010;
    const u32 dataSize = header.chrRomSizeKB * 1_KB;
    memcpy( memory, &cartridgeRom[ offset ], dataSize );
}

const byte * const Video::GetPPUMemory() const
{
    return memory;
}