#include "Debugger.h"

#include <unordered_map>
#include <chrono>
#include <thread>

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include "ImguiWrapper/imgui_impl_glfw_gl3.h"

#include "../Video.h"
#include "../CpuTypes.h"


Debugger::Debugger( Cpu *cpu, Memory *memory, Video *video )
    : cpu( cpu )
    , memory( memory )
    , video( video )
    , window( nullptr )
    , mode( DebuggerMode::IDLE )
    , reset( false )
{
}

void Debugger::StartDebugger()
{
    glfwInit();
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 5 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    window = glfwCreateWindow( 2560, 1440, "PatNes", nullptr, nullptr );
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

    glfwSetFramebufferSizeCallback( window, []( GLFWwindow *window, i32 width, i32 height )
        {
            glViewport( 0, 0, width, height );
        }
    );

    ImGuiGLFW::Init( window, true );
    ImGui::StyleColorsDark();
    ImGuiIO& io = ImGui::GetIO();
    io.FontGlobalScale = 2.f;
    videoDebugger.CreateTextures( *video );
    cpuDebugger.GenerateDisassemblerInstructionMask( *memory );

    Update(0.f,0);
}

void Debugger::ResetDebugger()
{
    mode = DebuggerMode::IDLE;
}

void Debugger::CloseDebugger()
{
    ImGuiGLFW::Shutdown();
    glfwDestroyWindow( window );
    glfwTerminate();
}

DebuggerUpdateResult Debugger::Update( float deltaMilliseconds, u32 cycles )
{
    memoryDebugger.UpdateWatcher( memory, mode );

    if ( mode == DebuggerMode::BREAKPOINT  || mode == DebuggerMode::IDLE || cpuDebugger.HasAddressABreakpoint( cpu->GetPC().value ) )
    {
        /* if the emulator has reached a breakpoint we render the debugger at 60fps */

        std::chrono::time_point<std::chrono::high_resolution_clock> current, previous;
        previous = std::chrono::high_resolution_clock::now();

        mode = DebuggerMode::IDLE;
        while ( mode == DebuggerMode::IDLE )
        {
            current = std::chrono::high_resolution_clock::now();
            const auto elapsed = std::chrono::duration_cast<std::chrono::duration<float, std::milli>> (current - previous);
            previous = current;

            ComposeView( cycles );
            Render();

            if ( ShouldCloseWindow() )
            {
                return DebuggerUpdateResult::QUIT;
            }

            if ( reset )
            {
                reset = false;
                return DebuggerUpdateResult::RESET;
            }

            if ( elapsed.count() < 16.6f ) 
            {
                std::this_thread::sleep_for( std::chrono::duration< float, std::milli > ( 16.6F - elapsed.count() ) );
            }
        }
    }
    else
    {
        /* In normal mode just render the debugger at the same rate as the emulator */
        if ( cycles >=  AVERAGE_CYCLES_PER_FRAME )
        {
            ComposeView( cycles );
            Render();

            if ( ShouldCloseWindow() )
            {
                return DebuggerUpdateResult::QUIT;
            }
        }
    }

    if ( reset )
    {
        reset = false;
        mode = DebuggerMode::IDLE;
        return DebuggerUpdateResult::RESET;
    }

    return DebuggerUpdateResult::CONTINUE;
}

void Debugger::ComposeView( u32 cycles )
{
    glfwPollEvents();
    ImGuiGLFW::NewFrame();

    ComposeEmulatorControlView();
    cpuDebugger.ComposeView( *cpu, *memory, cycles, mode );
    videoDebugger.ComposeView( cycles, *video, *memory );
    memoryDebugger.ComposeView( memory, video, mode );
}

void Debugger::ComposeEmulatorControlView()
{
    ImGui::SetNextWindowPos( ImVec2( 0, 0 ) );
    ImGui::Begin( "PatNes Control" );

    if ( ImGui::Button( "Reset" ) )
    {
        reset = true;
    }

    ImGui::End();
}

void Debugger::Render()
{
    i32 width, height;
    glfwGetFramebufferSize( window, &width, &height );
    glViewport( 0, 0, width, height );
    glClear( GL_COLOR_BUFFER_BIT );
    ImGui::Render();
    ImGuiGLFW::RenderDrawLists( ImGui::GetDrawData() );
    glfwSwapBuffers( window );
}

bool Debugger::ShouldCloseWindow() const
{
    return glfwGetKey( window, GLFW_KEY_ESCAPE ) || glfwWindowShouldClose( window );
}