#pragma once

#include "../Types.h"

class Video;

class VideoDebugger
{
public:
    VideoDebugger() = default;
    ~VideoDebugger();

    void CreateTextures( const Video &video );
    void ComposeView( u32 cycles, const Video &video );

private:
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


    void UpdatePatternTable( const Video &video, word address, RGB *buffer );
    void GenerateNesPaletteTexture();
    void UpdateUniversalBackgroundColour( const Video &video );
    void UpdateTexturesOfCurrentPalettes( const Video &video, word address, RGB **buffer );
};