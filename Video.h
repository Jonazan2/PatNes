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
    |  0x3F00 - 0x3F0F    |  BG Palette RAM indexes     |
    |                     |                             |
    |  0x3F10 - 0x3F1F    |  Sprite Palette RAM indexes |
    |                     |                             |
    |  0x3F20 - 0x3FFF    |  Mirrors of 0x3F00 0x3F1F   |
    |                     |                             |
    +---------------------+-----------------------------+

*/


class Cartridge;

class Video
{
public:

    static constexpr u32 NES_VIDEO_RESOLUTION = 256 * 240;

    Video( Cartridge *memory );
    ~Video();

    void Reset();

    /* PPU memory management */
    const byte * const GetPPUMemory() const;
    byte Read( word address ) const;
    void Write( word address, byte data );

    /* Frame buffer */
    RGB* GetFrameBuffer() const;

private:

    /* PPU Register addresses */
    static constexpr word PPUCTRL_REGISTER      = 0x2000;
    static constexpr word PPUMASK_REGISTER      = 0x2001;
    static constexpr word PPUSTATUS_REGISTER    = 0x2002;
    static constexpr word OAMA_REGISTER         = 0x2003;
    static constexpr word OAMADATA_REGISTER     = 0x2004;
    static constexpr word PPUSCROLL_REGISTER    = 0x2005;
    static constexpr word PPUADDR_REGISTER      = 0x2006;
    static constexpr word PPUDATA_ADDRESS       = 0x2007;


    /* Associated Systems */
    Cartridge       *cartridge;

    /* PPU memory layout */
    byte            *memory;

    /* Frame buffer */
    RGB             *frameBuffer;


    void MapCartridgeCHRToPPU();
};