#include "MemoryDebugger.h"

#include <stdio.h>

#include "Imgui/imgui.h"

#include "Debugger.h"
#include "../Memory.h"

MemoryDebugger::MemoryDebugger()
    : watcherAsBreakpoint( false )
{
}


void MemoryDebugger::ComposeView( const Memory *memory, DebuggerMode& mode )
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


    /* Watcher */
    ImGui::Separator();

    ImGui::PushItemWidth( 160 );
    ImGui::AlignTextToFramePadding();
    ImGui::Text( "Add watcher:" );
    ImGui::SameLine();
    ImGui::PopItemWidth();

    ImGui::PushItemWidth( 70 );
    char input[ 64 ];
    memset( input, 0, sizeof( char ) * 64 );
    if ( ImGui::InputText( "##addr", input, 64, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_EnterReturnsTrue ) )
    {
        u32 address;
        if ( sscanf( input, "%X", &address ) )
        {
            word validAddress = address & 0xFFFF;
            watcher.insert( { validAddress, map[ validAddress ] } );
        }
    }
    ImGui::PopItemWidth();

    if ( !watcher.empty() )
    {
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, ImColor::HSV(1.0f, 0.6f, 0.6f).Value);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImColor::HSV(0.95f, 0.5f, 0.5f).Value);
        if ( ImGui::Button( "Clear all watchers" ) )
        {
            watcher.clear();
            mode = DebuggerMode::RUNNING;
        }
        ImGui::PopStyleColor(2);
        ImGui::SameLine();

        ImGui::Checkbox("Data breakpoint", &watcherAsBreakpoint);

        ImGui::Separator();
        ImGui::Columns(3, "breakpoints");

        std::map<word, byte>::iterator it;
        for (it = watcher.begin(); it != watcher.end(); ) {
            word address = it->first;
            byte data = it->second;

            char label[64];
            sprintf(label, "0x%04X:  0x%02X  %c", address, data, (data >= 32 && data < 128) ? data : '.');
            if (ImGui::Selectable(label)) {
                it = watcher.erase(it);
            } else {
                ++it;
            }
            ImGui::NextColumn();
        }
        ImGui::Columns(1);
        ImGui::Separator();
    }

    ImGui::End();
}

void MemoryDebugger::UpdateWatcher( const Memory* memory, DebuggerMode& mode )
{
    assert( memory != nullptr );
    const byte *map = memory->GetMemoryMap();
    assert( map != nullptr );

    if ( !watcher.empty() )
    {
        if ( watcherAsBreakpoint && HasWatcherDataChanged( map ) )
        {
            mode = DebuggerMode::BREAKPOINT;
        }
        UpdateWatcherData( map );
    }
}

bool MemoryDebugger::HasWatcherDataChanged( const byte * const memory ) const
{
    for (const auto &[ address, value ] : watcher )
    {
        if ( value != memory[ address ] )
        {
            return true;
        }
    }
    return false;
}

void MemoryDebugger::UpdateWatcherData( const byte * const memory )
{
    for ( auto &[ address, value ] : watcher )
    {
        const byte data = memory[ address ];
        if ( value != data )
        {
            value = data;
        }
    }
}
