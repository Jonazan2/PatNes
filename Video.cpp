#include "Video.h"


#include <assert.h>
#include <cstring>

#include "Cartridge.h"
#include "Memory.h"


Video::Video( Cartridge *cartridge )
    : cartridge( cartridge )
    , ppuCycles( 0u )
    , currentScanline( 0u )
{
    map = new byte[ 16_KB ];
    frameBuffer = new RGB[ NES_VIDEO_RESOLUTION ];

    Reset();
}

Video::~Video()
{
    delete[] map;
    delete[] frameBuffer;
}

void Video::Init( Memory *memorySystem )
{
    memory = memorySystem;
}

void Video::Reset()
{
    memset( map, 0x00, 16_KB );

    for ( u32 i = 0; i < NES_VIDEO_RESOLUTION; ++i )
    {
        frameBuffer[ i ] = color::PINK;
    }

    MapCartridgeCHRToPPU();

}

void Video::MapCartridgeCHRToPPU()
{
    assert( cartridge != nullptr );

    /* TODO(Jonathan): Support more mappers here */
    const Cartridge::Header &header = cartridge->GetHeader();
    const byte * const cartridgeRom = cartridge->GetRom();
    const u64 offset = (header.prgRomSizeKB * 1_KB) + 0x0010;
    const u64 dataSize = header.chrRomSizeKB * 1_KB;
    memcpy( map, &cartridgeRom[ offset ], dataSize );
}

RGB* Video::GetFrameBuffer() const
{
    return frameBuffer;
}

const byte * const Video::GetPPUMemory() const
{
    return map;
}

byte Video::Read( word address ) const
{
    return map[ address ];
}

void Video::Write( word address, byte data )
{
    map[ address ] = data;
}

void Video::Update( u32 cycles )
{
    // 1 CPU Cycles = 3 PPU cycle
    ppuCycles = cycles * 3;
    currentScanline = ppuCycles / CYCLE_DURATION_PER_SCANLINE;

    if ( currentScanline == VBLANK_SCANLINE )
    {
        memory->Write( PPUSTATUS_REGISTER, memory->Read( PPUSTATUS_REGISTER ) & 0b1000'0000 );
    }
    
    if ( currentScanline == MAX_SCANLINES_PER_FRAME )
    {
        // Reset everything
        ppuCycles = 0u;
        currentScanline = 0u;
    }
}
