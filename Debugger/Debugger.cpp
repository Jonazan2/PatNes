#include "Debugger.h"

#include <unordered_map>
#include <chrono>
#include <thread>

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include "ImguiWrapper/imgui_impl_glfw_gl3.h"

#include "../CpuTypes.h"


Debugger::Debugger( Cpu *cpu, Memory *memory )
    : cpu( cpu )
    , memory ( memory )
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

    glfwSetFramebufferSizeCallback( window, []( GLFWwindow *window, i32 width, i32 height )
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

void Debugger::Update( float deltaMilliseconds, u32 cycles )
{
    if ( mode == DebuggerMode::RUNNING )
    {
        /* In normal mode just render the debugger at the same rate as the emulator: random number for now */
        if ( cycles % 10000 )
        {
            ComposeView( cycles );
            Render();
        }
    }
    else
    {
        /* if the emulator has reached a breakpoint we render the debugger at 60fps */

        std::chrono::time_point<std::chrono::high_resolution_clock> current, previous;
        previous = std::chrono::high_resolution_clock::now();
        
        mode = DebuggerMode::IDLE;
        while ( mode == DebuggerMode::IDLE )
        {
            current = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::duration<float, std::milli>> (current - previous);
            previous = current;

            ComposeView( cycles );
            Render();

            if ( elapsed.count() < 16.6f ) 
            {
                std::this_thread::sleep_for( std::chrono::duration< float, std::milli > ( 16.6F - elapsed.count() ) );
            }
        }
    }
}

void Debugger::ComposeView( u32 cycles )
{
    glfwPollEvents();
    ImGuiGLFW::NewFrame();

    cpuDebugger.ComposeView( *cpu, *memory, cycles, mode );
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
