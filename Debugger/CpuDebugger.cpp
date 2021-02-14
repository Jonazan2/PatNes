#include "CpuDebugger.h"

#include "Debugger.h"
#include "../Cpu.h"
#include "../Memory.h"
#include "../CpuTypes.h"
#include "Imgui/imgui.h"

CpuDebugger::CpuDebugger()
    : instructionJump( false )
{
    //breakpoints.insert( 0xF1CE );
}

void CpuDebugger::GenerateDisassemblerInstructionMask( Memory &memory )
{
    /* Instructions are represented by 1, data as 0 */

    disassemblerInstructionMask.reset();

    u32 offset = 1;
    for ( u32 i = 0; i <= 0xFFFF; i+= offset )
    {
        if ( i >= 0x2000 && i < 0x4020)
        {
            /* 2000 to 401F is data */
            i = 0x4020;
        }
        else
        {
            const byte opcode = memory.Read( i );
            std::unordered_map< byte, OpcodeInfo >::const_iterator it = NES_OPCODE_INFO.find( opcode );
            if ( it != NES_OPCODE_INFO.end() )
            {
                const OpcodeInfo opcodeInfo = it->second;
                const byte addressModeIndex = static_cast< byte >( opcodeInfo.addressMode );
                const byte opcodeLength = ADDRESS_MODE_OPCODE_LENGTH [ addressModeIndex ];
                offset = opcodeLength;
                disassemblerInstructionMask.set( i );
            }
        }
    }
}

bool CpuDebugger::IsAddresAnInstruction( u32 address ) const
{
    return disassemblerInstructionMask[ address ];
}

void CpuDebugger::ComposeView( Cpu &cpu, Memory &memory, u32 cycles, DebuggerMode& mode )
{
    ImGui::SetNextWindowPos( ImVec2( 0, 100 ) );
    ImGui::Begin( "Cpu" );
    {
        ImGui::Columns( 2 );
        const float columnWidth = ImGui::GetWindowWidth() / 2.f;
        ImGui::SetColumnWidth( 0, columnWidth );
        ImGui::SetColumnWidth( 1, columnWidth );
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
            if ( ImGui::Button( "Next instruction" ) )
            {
                mode = DebuggerMode::BREAKPOINT;
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
            static const int PER_ITEM_WIDTH = 25;
            ImGui::Text("%-*s%-*s%-*s", PER_ITEM_WIDTH, "Address", PER_ITEM_WIDTH, "Mnemonic", PER_ITEM_WIDTH, "Data");
            ImGui::Separator();

            ImGui::BeginGroup();
            ImGui::BeginChild("##scrollingregion");

            const float height = ImGui::GetWindowHeight();
            const float lineHeight = ImGui::GetTextLineHeightWithSpacing();
            const byte rows = height / lineHeight;

            ImGuiListClipper clipper( 0xFFFF, lineHeight );
            u32 startAddress = 0;
            u32 endAddress = 0;
            if (clipper.DisplayStart != 0x0000)
            {
                startAddress = clipper.DisplayStart;
                const word PC = cpu.GetPC().value;
                if ( goToPcPosition )
                {
                    startAddress = PC;
                }
                else if ( mode == DebuggerMode::BREAKPOINT && PC < startAddress )
                {
                    startAddress = PC;
                    instructionJump = false;
                }
                else
                {
                    while ( !IsAddresAnInstruction( startAddress ))
                    {
                        startAddress--;
                    }
                }

                endAddress = startAddress;
                u32 realRows = 0;
                while ( realRows !=  rows )
                {
                    const byte opcode = memory.Read( endAddress );
                    std::unordered_map< byte, OpcodeInfo >::const_iterator it = NES_OPCODE_INFO.find( opcode );
                    if ( it == NES_OPCODE_INFO.end() )
                    {
                        endAddress++;
                    }
                    else
                    {
                        realRows++;
                        if (realRows == rows )
                        {
                            break;
                        }

                        const byte addressModeIndex = static_cast< byte >( it->second.addressMode );
                        const byte opcodeLength = ADDRESS_MODE_OPCODE_LENGTH [ addressModeIndex ];
                        endAddress += opcodeLength;
                    }
                }

                if (endAddress > 0xFFFF)
                {
                    endAddress = 0xFFFF;
                }
            }

            const word PC = cpu.GetPC().value;
            if ( mode == DebuggerMode::BREAKPOINT && PC > ( endAddress - 1 ) )
            {
                ImGui::SetScrollFromPosY( ImGui::GetCursorStartPos().y + ( PC * ImGui::GetTextLineHeightWithSpacing() ), 0.f );
            }
            else if ( ( mode == DebuggerMode::IDLE && !instructionJump ) || goToPcPosition )
            {
                ImGui::SetScrollFromPosY( ImGui::GetCursorStartPos().y + ( PC * ImGui::GetTextLineHeightWithSpacing() ), 0.f );
                instructionJump = true;
            }

            u32 opcodeLengthOffset = 1;
            for ( u32 i = startAddress; i <= endAddress; i += opcodeLengthOffset ) 
            {
                char text[128];

                const byte opcode = memory.Read( i );
                std::unordered_map< byte, OpcodeInfo >::const_iterator it = NES_OPCODE_INFO.find( opcode );
                if ( it == NES_OPCODE_INFO.end() )
                {
                    /* Invalid opcode */
                    opcodeLengthOffset = 1;
                    continue;
                }
                else
                {
                    char address[32];
                    sprintf( address, "0x%04X", i);

                    const OpcodeInfo &opcodeInfo = it->second;
                    const byte addressModeIndex = static_cast< byte >( opcodeInfo.addressMode );
                    const byte opcodeLength = ADDRESS_MODE_OPCODE_LENGTH [ addressModeIndex ];
                    
                    char data[32];
                    if ( opcodeLength == 1 )
                    {
                        sprintf(data, "");
                    }
                    else if ( opcodeLength == 2 )
                    {
                        const byte opcodeData = memory.Read( i + 1 );
                        sprintf( data, "0x%02X", opcodeData );
                    }
                    else if ( opcodeLength == 3 )
                    {
                        word wordData;
                        wordData = memory.Read( i + 2 ) << 8;
                        wordData |= memory.Read( i + 1 );
                        sprintf( data, "0x%04X", wordData );
                    }
                    
                    sprintf( text, "%-*s%-*s%-*s", PER_ITEM_WIDTH, address, PER_ITEM_WIDTH, opcodeInfo.mnemonic, PER_ITEM_WIDTH, data );

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
