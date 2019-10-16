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
    |  0x2000 - 0x23BF    |  Nametable 0                |
    |                     |                             |
    |  0x23C0 - 0x23FF    |  Attribute table 0          |
    |                     |                             |
    |  0x2400 - 0x27BF    |  Nametable 1                |
    |                     |                             |
    |  0x27C0 - 0x27FF    |  Attribute table 1          |
    |                     |                             |
    |  0x2800 - 0x2BFF    |  Nametable 2                |
    |                     |                             |
    |  0x2BC0 - 0x2BFF    |  Attribute table 2          |
    |                     |                             |
    |  0x2C00 - 0x2FBF    |  Nametable 3                |
    |                     |                             |
    |  0x2FC0 - 0x2FFF    |  Attribute table 3          |
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

    /* PPU Register addresses */
    static constexpr word PPUCTRL_REGISTER      = 0x2000;
    static constexpr word PPUMASK_REGISTER      = 0x2001;
    static constexpr word PPUSTATUS_REGISTER    = 0x2002;
    static constexpr word OAMA_REGISTER         = 0x2003;
    static constexpr word OAMADATA_REGISTER     = 0x2004;
    static constexpr word PPUSCROLL_REGISTER    = 0x2005;
    static constexpr word PPUADDR_REGISTER      = 0x2006;
    static constexpr word PPUDATA_ADDRESS       = 0x2007;


    Video( Cartridge *cartridge );
    ~Video();

    void Reset();

    void Update( u32 cycles );

    /* PPU memory management */
    const byte * const GetPPUMemory() const;
    byte Read( word address ) const;
    void Write( word address, byte data );

    /* Frame buffer */
    RGB* GetFrameBuffer() const;

private:
    /* Associated Systems */
    Cartridge       *cartridge;

    /* PPU memory layout */
    byte            *memory;

    /* Frame buffer */
    RGB             *frameBuffer;


    void MapCartridgeCHRToPPU();
};