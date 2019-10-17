#include "MemoryDebugger.h"


#include "Imgui/imgui.h"

#include "../Memory.h"


void MemoryDebugger::ComposeView( const Memory *memory )
{
    assert( memory != nullptr );
    const byte *map = memory->GetMemoryMap();
    assert( map != nullptr );

    ImGui::SetNextWindowPos( ImVec2( 650, 150 ), ImGuiCond_FirstUseEver );
    ImGui::Begin( "Memory" );
    ImGui::BeginChild( "##scrolling", ImVec2( 0, 450 ) );

    ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 0, 0 ) );
    ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 0, 0 ) );

    i32 addressDigits = 0;
    for ( i32 n = MEMORY_VIEW_BASE_ADDRESS + MEMORY_VIEW_MEMORY_SIZE - 1; n > 0; n >>= 4 )
    {
        addressDigits++;
    }

    float glyphWidth = ImGui::CalcTextSize("F").x;
    float cellWidth = glyphWidth * 3;

    float height = ImGui::GetTextLineHeight();
    u32 totalLines = static_cast< i32 >( ( ( MEMORY_VIEW_MEMORY_SIZE + MEMORY_VIEW_ROWS - 1 ) / MEMORY_VIEW_ROWS ) );
    ImGuiListClipper clipper( totalLines, height );
    i32 visibleStartAddr = clipper.DisplayStart * MEMORY_VIEW_ROWS;
    i32 visibleEndAddr = clipper.DisplayEnd * MEMORY_VIEW_ROWS;

    bool drawSeparator = true;
    for ( i32 lineNumber = clipper.DisplayStart; lineNumber < clipper.DisplayEnd; lineNumber++ )
    {
        i32 addr = lineNumber * MEMORY_VIEW_ROWS;
        ImGui::Text("%0*X: ", addressDigits, MEMORY_VIEW_BASE_ADDRESS + addr);
        ImGui::SameLine();

        /* Draw Hexadecimal */
        float lineStartX = ImGui::GetCursorPosX();
        for ( i32 n = 0; n < MEMORY_VIEW_ROWS && addr < MEMORY_VIEW_MEMORY_SIZE; n++, addr++ )
        {
            ImGui::SameLine( lineStartX + cellWidth * n );
            ImGui::Text("%02X ", map[ addr ] );
        }

        ImGui::SameLine( lineStartX + cellWidth * MEMORY_VIEW_ROWS + glyphWidth * 2 );

        if ( drawSeparator )
        {
            ImVec2 screenPosition = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddLine( ImVec2( screenPosition.x - glyphWidth, screenPosition.y - 9999 ), ImVec2( screenPosition.x - glyphWidth, screenPosition.y + 9999 ), ImColor( ImGui::GetStyle().Colors[ ImGuiCol_Border ] ) );
            drawSeparator = false;
        }

        /* Draw ASCII values */
        addr = lineNumber * MEMORY_VIEW_ROWS;
        for ( i32 n = 0; n < MEMORY_VIEW_ROWS && addr < MEMORY_VIEW_MEMORY_SIZE; n++, addr++ )
        {
            if ( n > 0 ) ImGui::SameLine();
            const i32 c = map[ addr ];
            ImGui::Text( "%c", ( c >= 32 && c < 128 ) ? c : '.' );
        }
    }

    clipper.End();
    ImGui::PopStyleVar( 2 );
    ImGui::EndChild();
    ImGui::End();
}