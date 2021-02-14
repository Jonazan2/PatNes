#include "VideoDebugger.h"

#include <stdio.h>

#include "ImguiWrapper/imgui_impl_glfw_gl3.h"
#include "../Video.h"
#include "../Memory.h"
#include "../PaletteColors.h"


VideoDebugger::~VideoDebugger()
{
    delete[] leftPatternTableBuffer;
    delete[] rightPatternTableBuffer;
    delete[] nesPaletteTextureBuffer;
    delete universalBackgroundColorBuffer;

    for ( int i = 0; i < 4; ++i )
    {
        delete backgroundPalettesTextureBuffer[i];
        delete spritePalettesTextureBuffer[i];
    }

    delete[] nametableTextureBuffer;
}

void VideoDebugger::CreateTextures( const Video &video )
{
    leftPatternTableBuffer = new RGB[ 128 * 128 ];
    ImGuiGLFW::Texture leftPatternTexture = { 0, 128, 128, leftPatternTableBuffer };
    leftPatternTableTextureID = ImGuiGLFW::CreateTexture( leftPatternTexture );

    rightPatternTableBuffer = new RGB[ 128 * 128 ];
    ImGuiGLFW::Texture rightPatternTexture = { 0, 128, 128, rightPatternTableBuffer };
    rightPatternTableTextureID = ImGuiGLFW::CreateTexture( rightPatternTexture );

    nesPaletteTextureBuffer = new RGB[ 64 ];
    ImGuiGLFW::Texture paletteTexture = { 0, 16, 4, nesPaletteTextureBuffer };
    nesPaletteTextureID = ImGuiGLFW::CreateTexture( paletteTexture );
    GenerateNesPaletteTexture();

    ImGuiGLFW::Texture frameBufferTexture = { 0, 256, 240, video.GetFrameBuffer() };
    frameBufferTextureID = ImGuiGLFW::CreateTexture( frameBufferTexture );

    universalBackgroundColorBuffer = new RGB();
    ImGuiGLFW::Texture universalBackgroundTexture = { 0, 1, 1, universalBackgroundColorBuffer };
    universalBackgroundColorID = ImGuiGLFW::CreateTexture( universalBackgroundTexture );

    for ( int i = 0; i < 4; ++i )
    {
        backgroundPalettesTextureBuffer[ i ] = new RGB [ 3 ];
        ImGuiGLFW::Texture backgroundPaletteTexture = { 0, 3, 1, backgroundPalettesTextureBuffer[ i ] };
        backgroundPalettesTextureID[ i ] = ImGuiGLFW::CreateTexture( backgroundPaletteTexture );

        spritePalettesTextureBuffer[ i ] = new RGB [ 3 ];
        ImGuiGLFW::Texture spritePaletteTexture = { 0, 3, 1, spritePalettesTextureBuffer[ i ] };
        spritePalettesTextureID[ i ] = ImGuiGLFW::CreateTexture( spritePaletteTexture );
    }

    nametableTextureBuffer = new RGB[ 256 * 240 ];
    ImGuiGLFW::Texture nametableTexture = { 0, 256, 240, nametableTextureBuffer };
    nametableTextureID = ImGuiGLFW::CreateTexture( nametableTexture );
}

void VideoDebugger::ComposeView( u32 cycles, const Video &video, const Memory &memory )
{
    ImGui::SetNextWindowSize( ImVec2( 560, 510 ), ImGuiCond_FirstUseEver );
    ImGui::Begin( "FrameBuffer" );
    ImGui::Image( frameBufferTextureID, ImVec2( 512, 480 ) );
    ImGui::End();

    UpdatePatternTable( video, 0x0000, leftPatternTableBuffer );
    ImGui::SetNextWindowSize( ImVec2( 560, 560 ), ImGuiCond_FirstUseEver );
    ImGui::Begin( "VRAM Left" );
    ImGui::Image( leftPatternTableTextureID, ImVec2( 512, 512 ) );
    ImGui::End();

    UpdatePatternTable( video, 0x1000, rightPatternTableBuffer );
    ImGui::SetNextWindowSize( ImVec2( 560, 560 ), ImGuiCond_FirstUseEver );
    ImGui::Begin( "VRAM Right" );
    ImGui::Image( rightPatternTableTextureID, ImVec2( 512, 512 ) );
    ImGui::End();

    {
        ImGui::SetNextWindowSize( ImVec2( 260, 70 ), ImGuiCond_FirstUseEver );
        ImGui::Begin( "NES Palette" );
        ImGui::Image( nesPaletteTextureID, ImVec2( 256, 64 ) );

        UpdateUniversalBackgroundColour( video );
        ImGui::Text( "Background universal color:\t" );
        ImGui::SameLine();
        ImGui::Image( universalBackgroundColorID, ImVec2( 24, 24 ) );

        UpdateTexturesOfCurrentPalettes( video, 0x3F01, backgroundPalettesTextureBuffer );
        for ( byte paletteIndex = 0; paletteIndex < 4; ++paletteIndex )
        {
            char text[ 32 ];
            sprintf( text, "Background Palette %i:\t\t", paletteIndex );
            ImGui::Text( text );
            ImGui::SameLine();
            ImGui::Image( backgroundPalettesTextureID[ paletteIndex ], ImVec2( 96, 24 ) );
        }

        UpdateTexturesOfCurrentPalettes( video, 0x3F11, spritePalettesTextureBuffer );
        for ( byte paletteIndex = 0; paletteIndex < 4; ++paletteIndex )
        {
            char text[ 32 ];
            sprintf( text, "Sprite Palette %i:\t\t\t", paletteIndex );
            ImGui::Text( text );
            ImGui::SameLine();
            ImGui::Image( spritePalettesTextureID[ paletteIndex ], ImVec2( 96, 24 ) );
        }
        ImGui::End();
    }

    UpdateNameTable( video, memory, 0x2000, nametableTextureBuffer );
    ImGui::SetNextWindowSize( ImVec2( 560, 560 ), ImGuiCond_FirstUseEver );
    ImGui::Begin( "Nametable 0" );
    ImGui::Image( nametableTextureID, ImVec2( 512, 500 ) );
    ImGui::End();
}

void VideoDebugger::UpdatePatternTable( const Video &video, word address, RGB *buffer )
{
    const byte * const ppuMemory = video.GetPPUMemory();
    assert( buffer != nullptr );
    assert( ppuMemory != nullptr );

    u32 vramPosition = 0;
    for ( u32 tile = 0; tile < Video::NES_PATTERN_TILE_AMOUNT; ++tile )
    { 
        const u32 tileAddress = ( tile * 0x10 ) + address;

        u32 localvramPosition = vramPosition;
        for ( byte row = 0; row < 8; ++row )
        {
            const byte firstByte = ppuMemory[ tileAddress + row ];
            const byte secondByte = ppuMemory[ tileAddress + row + 0x07 ];

            for ( byte column = 0; column < 8; ++column )
            {
                byte mask = 0x01 << column;
                const byte value = ( ( secondByte & mask ) >> column )  << 1;
                const byte value2 = ( firstByte & mask ) >> column;
                const byte finalValue = value | value2;

                buffer[ localvramPosition + ( 7 - column ) ] = NES_PALETTE_COLORS[ palette[ finalValue ] ];
            }
            localvramPosition += 128;
        }
        vramPosition += 8;

        if ( tile > 0 && (tile + 1) % 16 == 0)
        {
            vramPosition += 128 * 7;
        }
    }
}

void VideoDebugger::GenerateNesPaletteTexture()
{
    for ( byte color = 0; color < 64; ++color )
    {
        nesPaletteTextureBuffer[ color ] = NES_PALETTE_COLORS[ color ];
    }
}

void VideoDebugger::UpdateUniversalBackgroundColour( const Video &video )
{
    const byte * const ppuMemory = video.GetPPUMemory();
    assert( ppuMemory != nullptr );
    assert( universalBackgroundColorBuffer != nullptr );

    const byte colorIndex = ppuMemory[ 0x3F00 ];
    *universalBackgroundColorBuffer = NES_PALETTE_COLORS[ colorIndex ];
}

void VideoDebugger::UpdateTexturesOfCurrentPalettes( const Video &video, word address, RGB **buffer )
{
    const byte * const ppuMemory = video.GetPPUMemory();
    assert( ppuMemory != nullptr );
    assert( buffer != nullptr );

    word paletteAddress = address;
    for ( byte index = 0; index < 4; ++index )
    {
        for ( byte i = 0; i < 3; ++i )
        {
            const byte colorIndex = ppuMemory[ paletteAddress ];
            buffer[ index ][ i ] = NES_PALETTE_COLORS[ colorIndex ];
            paletteAddress++;
        }
        paletteAddress++;
    }
}

void VideoDebugger::UpdateNameTable( const Video &video, const Memory &memory, word nametableAddress, RGB *buffer )
{
    /* We use the mal directly since reading some of the values in the map through memory.Read() has side effects */
    const byte *const memoryMap = memory.GetMemoryMap();
    assert( memoryMap != nullptr );

    /* Check which pattern table is selected at the moment */
    const byte ppuControl = memoryMap[ Video::PPUCTRL_REGISTER ];
    const u32 patternTableAddress = ( ppuControl & 0b0001'0000 ) ? 0x1000 : 0x0000;

    /* Traverse the nametable and construct the background */
    for ( u32 row = 0; row < Video::NES_VIDEO_HEIGHT; ++row )
    {
        for ( u32 column = 0; column < Video::NES_VIDEO_WIDTH; ++column )
        {
            const byte tileOffset = video.Read( nametableAddress );
            ++nametableAddress;

            /* Update the background buffer */
            const u32 tileAddress = patternTableAddress + tileOffset;
            u32 localvramPosition = tileAddress;
            for ( byte tileRow = 0; tileRow < 8; ++tileRow )
            {
                const byte firstByte = video.Read( tileAddress + tileRow );
                const byte secondByte = video.Read( tileAddress + tileRow + 0x07 );

                for ( byte tileColumn = 0; tileColumn < 8; ++tileColumn )
                {
                    const byte mask = 0x01 << tileColumn;
                    const byte value = ( ( secondByte & mask ) >> tileColumn )  << 1;
                    const byte value2 = ( firstByte & mask ) >> tileColumn;
                    const byte finalValue = value | value2;

                    const u32 textureIndex = column + row * Video::NES_VIDEO_WIDTH; 
                    buffer[ textureIndex ] = NES_PALETTE_COLORS[ palette[ finalValue ] ];
                }
                localvramPosition += 128;
            }
        }
    }
}