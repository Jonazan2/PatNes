#include "VideoDebugger.h"

#include "ImguiWrapper/imgui_impl_glfw_gl3.h"
#include "../Video.h"
#include "../PaletteColors.h"


VideoDebugger::~VideoDebugger()
{
    delete leftPatternTableBuffer;
    delete rightPatternTableBuffer;
    delete nesPaletteTextureBuffer;
    delete[] backgroundPalettesTextureBuffer;
    delete[] spritePalettesTextureBuffer;
}

void VideoDebugger::CreateTextures()
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

    for ( int i = 0; i < 4; ++i )
    {
        backgroundPalettesTextureBuffer[ i ] = new RGB [ 4 ];
        ImGuiGLFW::Texture backgroundPaletteTexture = { 0, 4, 1, backgroundPalettesTextureBuffer[ i ] };
        backgroundPalettesTextureID[ i ] = ImGuiGLFW::CreateTexture( backgroundPaletteTexture );

        spritePalettesTextureBuffer[ i ] = new RGB [ 4 ];
        ImGuiGLFW::Texture spritePaletteTexture = { 0, 4, 1, spritePalettesTextureBuffer[ i ] };
        spritePalettesTextureID[ i ] = ImGuiGLFW::CreateTexture( spritePaletteTexture );
    }
}

void VideoDebugger::ComposeView( u32 cycles, const Video &video )
{
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

        UpdateTexturesOfCurrentPalettes( video, 0x3F01, backgroundPalettesTextureBuffer );
        for ( byte paletteIndex = 0; paletteIndex < 4; ++paletteIndex )
        {
            char text[ 32 ];
            sprintf( text, "Background Palette %i: \t", paletteIndex );
            ImGui::Text( text );
            ImGui::SameLine();
            ImGui::Image( backgroundPalettesTextureID[ paletteIndex ], ImVec2( 64, 12 ) );
        }

        UpdateTexturesOfCurrentPalettes( video, 0x3F11, spritePalettesTextureBuffer );
        for ( byte paletteIndex = 0; paletteIndex < 4; ++paletteIndex )
        {
            char text[ 32 ];
            sprintf( text, "Sprite Palette %i: \t\t", paletteIndex );
            ImGui::Text( text );
            ImGui::SameLine();
            ImGui::Image( spritePalettesTextureID[ paletteIndex ], ImVec2( 64, 12 ) );
        }
        ImGui::End();
    }
}

void VideoDebugger::UpdatePatternTable( const Video &video, word address, RGB *buffer )
{
    constexpr static RGB palette[4] = { { 255,255,255 },{ 0xCC,0xCC,0xCC },{ 0x77,0x77,0x77 }, { 0x0,0x0,0x0 } };

    const byte * const ppuMemory = video.GetPPUMemory();
    assert( buffer != nullptr );
    assert( ppuMemory != nullptr );

    u32 vramPosition = 0;
    for ( u32 tile = 0; tile < 256; ++tile )
    {
        const u32 tileAddress = (tile * 0x10) + address;

        u32 localvramPosition = vramPosition;
        for ( byte row = 0; row < 8; ++row )
        {
            const byte firstByte = ppuMemory[ tileAddress + row ];
            const byte secondByte = ppuMemory[ tileAddress + row + 0x07 ];

            for ( byte column = 0; column < 8; ++column)
            {
                byte mask = 0x01 << column;
                const byte value = ((secondByte & mask) >> column) << 1;
                const byte value2 = (firstByte & mask) >> column;
                const byte finalValue = value | value2;

                buffer[ localvramPosition + (7 - column) ] = palette[ finalValue ];
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

void VideoDebugger::UpdateTexturesOfCurrentPalettes( const Video &video, word address, RGB **buffer )
{
    const byte * const ppuMemory = video.GetPPUMemory();
    assert( ppuMemory != nullptr );
    assert( buffer != nullptr );

    u32 paletteAddress = address;
    for ( byte index = 0; index < 4; ++index )
    {
        for ( byte i = 0; i < 4; ++i )
        {
            const byte colorIndex = ppuMemory[ paletteAddress ];
            buffer[ index ][ i ] = NES_PALETTE_COLORS[ colorIndex ];
            paletteAddress++;
        }
    }
}