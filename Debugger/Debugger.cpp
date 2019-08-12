#include "Debugger.h"

#include <unordered_map>

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include "ImguiWrapper/imgui_impl_glfw_gl3.h"

#include "../CpuTypes.h"


Debugger::Debugger( Cpu *cpu )
    : cpu( cpu )
    , mode( DebuggerMode::BREAKPOINT )
    , window( nullptr )
{
}

void Debugger::StartDebugger()
{
    glfwInit();
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 5 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    window = glfwCreateWindow( 1024, 720, "PatNes", nullptr, nullptr );
    if ( window == nullptr )
    {
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent( window );
    glewExperimental = GL_TRUE;
    GLint GlewInitResult = glewInit();
    if (GLEW_OK != GlewInitResult)
    {
        return;
    }

    glfwSetFramebufferSizeCallback( window, []( GLFWwindow *window, int width, int height )
        {
            glViewport( 0, 0, width, height );
        }
    );

    ImGuiGLFW::Init( window, true );
    ImGui::StyleColorsDark();

    Update(0.f,0);
}

void Debugger::CloseDebugger()
{
    ImGuiGLFW::Shutdown();
    glfwDestroyWindow( window );
    glfwTerminate();
}

void Debugger::Update( float deltaMilliseconds, unsigned int cycles )
{
    if ( cycles % 10000 )
    {
        ComposeView(deltaMilliseconds, cycles);
        Render();
    }
}

void Debugger::ComposeView( float deltaMilliseconds, unsigned int cycles )
{
    glfwPollEvents();
    ImGuiGLFW::NewFrame();

    int width, height;
    glfwGetFramebufferSize( window, &width, &height );

    ImGui::SetNextWindowPos( ImVec2( 0, 0 ) );
    ImGui::SetNextWindowSize( ImVec2( static_cast< float >( width ), static_cast< float >( height ) ) );
    ImGui::Begin( "PatNes" );
    {
        ImGui::Columns(2, nullptr, true);
        ImGui::SetColumnWidth(0, 200.f);
        ImGui::SetColumnWidth(1, 200.f);
        {
            ImGui::Text( "Flags:" );

            for (const auto &[ flag, name ] : Cpu::FLAGS_STRING )
            {
                AddFlagCheckbox( flag, name );
            }
        }
        ImGui::NextColumn();
        {
            ImGui::Text( "Registers:" );
            char pcValue[ 32 ];
            sprintf( pcValue, "PC: 0x%04X", cpu->GetPC().value );
            ImGui::Text( pcValue );

            char accumulator[ 32 ];
            sprintf( accumulator, "Accumulator: 0x%02X", cpu->GetAccumulator() );
            ImGui::Text( accumulator );

            char stackPointer[ 32 ];
            sprintf( stackPointer, "Stack Pointer: 0x%04X", cpu->GetAbsoluteStackAddress() );
            ImGui::Text( stackPointer );
        
            char pRegisterValue[ 32 ];
            sprintf( pRegisterValue, "Status Register: 0x%02X", cpu->GetStateRegister() );
            ImGui::Text( pRegisterValue );

            char xRegister[ 32 ];
            sprintf( xRegister, "X: 0x%02X", cpu->GetRegisterX() );
            ImGui::Text( xRegister );

            char yRegister[ 32 ];
            sprintf( yRegister, "Y: 0x%04X", cpu->GetRegisterY() );
            ImGui::Text( yRegister );
        }
        ImGui::NextColumn();
        ImGui::Separator();
    }
    ImGui::End();
}

void Debugger::AddFlagCheckbox( Cpu::Flags flag, const char *name )
{
    bool breakFlag = cpu->IsFlagSet( flag );
    if ( ImGui::Checkbox( name, &breakFlag ) )
    {
        cpu->ToggleFlag( flag );
    }
}

void Debugger::Render()
{
    int width, height;
    glfwGetFramebufferSize( window, &width, &height );
    glViewport( 0, 0, width, height );
    glClear( GL_COLOR_BUFFER_BIT );
    ImGui::Render();
    ImGuiGLFW::RenderDrawLists( ImGui::GetDrawData() );
    glfwSwapBuffers(window);
}
