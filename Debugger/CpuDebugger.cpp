#include "CpuDebugger.h"

#include "../Cpu.h"
#include "Imgui/imgui.h"

void CpuDebugger::ComposeView( Cpu &cpu, u32 cycles )
{
    ImGui::SetNextWindowPos( ImVec2( 0, 0 ) );
    ImGui::SetNextWindowSize( ImVec2( 400, 250.f ) );
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
        ImGui::Separator();
    }
    ImGui::End();
}