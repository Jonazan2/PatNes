#include "CpuDebugger.h"

#include "Debugger.h"
#include "../Cpu.h"
#include "../Memory.h"
#include "../CpuTypes.h"
#include "Imgui/imgui.h"

CpuDebugger::CpuDebugger()
    : instructionJump( false )
{
}

void CpuDebugger::ComposeView( Cpu &cpu, Memory &memory, u32 cycles, DebuggerMode& mode )
{
    ImGui::SetNextWindowPos( ImVec2( 0, 100 ) );
    ImGui::Begin( "Cpu" );
    {
        ImGui::Columns( 2 );
        ImGui::SetColumnWidth( 0, 200.f );
        ImGui::SetColumnWidth( 1, 200.f );
        {
            ImGui::Text( "Flags:" );

            for (const auto &[ flag, name ] : Cpu::FLAGS_STRING )
            {
                bool dummy = cpu.IsFlagSet( flag );
                if ( ImGui::Checkbox( name, &dummy ) )
                {
                    cpu.ToggleFlag( flag );
                }
            }
        }
        ImGui::NextColumn();
        {
            ImGui::Text( "Registers:" );

            char pcValue[ 32 ];
            sprintf( pcValue, "PC: 0x%04X", cpu.GetPC().value );
            ImGui::Text( pcValue );

            char accumulator[ 32 ];
            sprintf( accumulator, "Accumulator: 0x%02X", cpu.GetAccumulator() );
            ImGui::Text( accumulator );

            char stackPointer[ 32 ];
            sprintf( stackPointer, "Stack Pointer: 0x%04X", cpu.GetAbsoluteStackAddress() );
            ImGui::Text( stackPointer );

            char pRegisterValue[ 32 ];
            sprintf( pRegisterValue, "Status Register: 0x%02X", cpu.GetStateRegister() );
            ImGui::Text( pRegisterValue );

            char xRegister[ 32 ];
            sprintf( xRegister, "X: 0x%02X", cpu.GetRegisterX() );
            ImGui::Text( xRegister );

            char yRegister[ 32 ];
            sprintf( yRegister, "Y: 0x%02X", cpu.GetRegisterY() );
            ImGui::Text( yRegister );
        }
        ImGui::NextColumn();
        ImGui::Columns(1);

        bool goToPcPosition = false;
        ImGui::Separator();
        {
            if (ImGui::Button("Next instruction")) {
                mode = DebuggerMode::BREAKPOINT;
                instructionJump = false;
            }

            ImGui::SameLine();
            if (ImGui::Button("Run")) {
                mode = DebuggerMode::RUNNING;
                instructionJump = false;
            }

            ImGui::SameLine();
            if (ImGui::Button("Run until vsync")) {
                mode = DebuggerMode::V_SYNC;
            }

            ImGui::SameLine();
            if (ImGui::Button("Go to PC instruction")) {
                mode = DebuggerMode::IDLE;
                goToPcPosition = true;
            }
        }
        {
            int perItemWidth = 25;
            ImGui::Text("%-*s%-*s%-*s", perItemWidth, "Address", perItemWidth, "Opcode", perItemWidth, "Mnemonic", perItemWidth, "Address Mode");
            ImGui::Separator();

            ImGui::BeginGroup();
            ImGui::BeginChild("##scrollingregion");
            ImGuiListClipper clipper(0xFFFF, ImGui::GetTextLineHeightWithSpacing());

            if ( ( mode == DebuggerMode::IDLE || mode == DebuggerMode::BREAKPOINT ) && !instructionJump )
            {
                ImGui::SetScrollFromPosY( ImGui::GetCursorStartPos().y + ( cpu.GetPC().value * ImGui::GetTextLineHeightWithSpacing() ), 0.5f );
                instructionJump = true;
                clipper.DisplayEnd += 16;

                if ( goToPcPosition )
                {
                    clipper.DisplayStart = cpu.GetPC().value;
                }
            }
            else
            {
                clipper.DisplayEnd = clipper.DisplayStart + 30;
            }

            u32 opcodeLengthOffset = 1;
            for ( u32 i = clipper.DisplayStart; i <= clipper.DisplayEnd; i += opcodeLengthOffset ) 
            {
                char address[32];
                sprintf( address, "0x%02X", i);
                
                char text[128];

                const byte opcode = memory.Read( i );
                std::unordered_map< byte, OpcodeInfo >::const_iterator it = NES_OPCODE_INFO.find( opcode );
                if ( it == NES_OPCODE_INFO.end() )
                {
                    /* Invalid opcode */
                    sprintf(text, "%-*s%-*s%-*s", perItemWidth, address, perItemWidth, "INVALID", perItemWidth, "-" );
                }
                else
                {
                    OpcodeInfo opcodeInfo = NES_OPCODE_INFO.at( opcode );
                                        
                    char mnemonic[ 32 ];
                    sprintf( mnemonic, "%s", opcodeInfo.mnemonic );

                    const byte addressModeIndex = static_cast< byte >( opcodeInfo.addressMode );
                    const char *addressMode = ADDRESS_MODE_STRING[ addressModeIndex ]; 
                    sprintf( text, "%-*s%-*s%-*s", perItemWidth, address, perItemWidth, mnemonic, perItemWidth, addressMode );
                    
                    const byte opcodeLength = ADDRESS_MODE_OPCODE_LENGTH [ addressModeIndex ];
                    opcodeLengthOffset = opcodeLength;
                }

                if ( ( mode == DebuggerMode::IDLE || mode == DebuggerMode::BREAKPOINT ) && i == cpu.GetPC().value )
                {
                    ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 0, 1, 0, 1 ) );
                }

                bool alreadySelected = HasAddressABreakpoint( i );
                if ( ImGui::Selectable( text, alreadySelected, ImGuiSelectableFlags_AllowDoubleClick ) )
                {
                    if ( alreadySelected )
                    {
                        breakpoints.erase(i);
                    }
                    else 
                    {
                        breakpoints.insert(i);
                    }
                }

                if ( ( mode == DebuggerMode::IDLE || mode == DebuggerMode::BREAKPOINT ) && i == cpu.GetPC().value ) 
                {
                    ImGui::PopStyleColor();
                }
            }

            clipper.End();
            ImGui::EndChild();
            ImGui::EndGroup();
        }
    }
    ImGui::End();
}

bool CpuDebugger::HasAddressABreakpoint( word address ) const
{
    if (breakpoints.empty())
    {
        return false;
    }

    for ( word breakpoint : breakpoints ) 
    {
        if ( breakpoint == address)
        {
            return true;
        }
        else if (breakpoint > address)
        {
            return false;
        }
    }
    return false;
}
