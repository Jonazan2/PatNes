#include "Video.h"


#include <assert.h>

#include "Cartridge.h"


Video::Video( Cartridge *cartridge )
    : cartridge( cartridge )
{
    memory = new byte[ 16_KB ];
    frameBuffer = new RGB[ NES_VIDEO_RESOLUTION ];

    Reset();
}

Video::~Video()
{
    delete[] memory;
    delete[] frameBuffer;
}


void Video::Reset()
{
    memset( memory, 0x00, 16_KB );

    for ( u32 i = 0; i < NES_VIDEO_RESOLUTION; ++i )
    {
        frameBuffer[ i ] = PINK;
    }

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

RGB* Video::GetFrameBuffer() const
{
    return frameBuffer;
}

const byte * const Video::GetPPUMemory() const
{
    return memory;
}

byte Video::Read( word address ) const
{
    return memory[ address ];
}

void Video::Write( word address, byte data )
{
    memory[ address ] = data;
}

void Video::Update( u32 cycles )
{
    // We do nothing here for now
}
