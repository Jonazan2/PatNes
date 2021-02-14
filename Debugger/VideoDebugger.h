#pragma once

#include "../Types.h"

class Video;
class Memory;

class VideoDebugger
{
public:
    VideoDebugger() = default;
    ~VideoDebugger();

    void CreateTextures( const Video &video );
    void ComposeView( u32 cycles, const Video &video, const Memory &memory );

private:
    constexpr static byte palette[4] = { 0x0F, 0x2C, 0x38, 0x12 };
 
    using ImTextureID = void *;

    /* Textures */
    ImTextureID     leftPatternTableTextureID;
    RGB             *leftPatternTableBuffer;

    ImTextureID     rightPatternTableTextureID;
    RGB             *rightPatternTableBuffer;

    ImTextureID     nesPaletteTextureID;
    RGB             *nesPaletteTextureBuffer;

    ImTextureID     frameBufferTextureID;

    ImTextureID     universalBackgroundColorID;
    RGB             *universalBackgroundColorBuffer;

    ImTextureID     backgroundPalettesTextureID[ 4 ];
    RGB             *backgroundPalettesTextureBuffer[ 4 ];

    ImTextureID     spritePalettesTextureID[ 4 ];
    RGB             *spritePalettesTextureBuffer[ 4 ];

    ImTextureID     nametableTextureID;
    RGB             *nametableTextureBuffer;

    void UpdatePatternTable( const Video &video, word address, RGB *buffer );
    void GenerateNesPaletteTexture();
    void UpdateUniversalBackgroundColour( const Video &video );
    void UpdateTexturesOfCurrentPalettes( const Video &video, word address, RGB **buffer );
    void UpdateNameTable( const Video &video, const Memory &memory, word nametableAddress, RGB *buffer );
};