#pragma once

#include "../Types.h"

class Video;

class VideoDebugger
{
public:
    VideoDebugger() = default;
    ~VideoDebugger();

    void CreateTextures();
    void ComposeView( u32 cycles, Video &video );

private:
    using ImTextureID = void *;

    /* Textures */
    ImTextureID     leftPatternTableTextureID;
    RGB             *leftPatternTableBuffer;

    ImTextureID     rightPatternTableTextureID;
    RGB             *rightPatternTableBuffer;

    ImTextureID     nesPaletteTextureID;
    RGB             *nesPaletteTextureBuffer;


    void UpdatePatternTable( Video &video, word address, RGB *buffer );
    void GenerateNesPaletteTexture();
};