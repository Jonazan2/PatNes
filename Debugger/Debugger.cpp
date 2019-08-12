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
    if ( cycles % 10000 )
    {
        ComposeView( cycles );
        Render();
    }
}

void Debugger::ComposeView( u32 cycles )
{
    glfwPollEvents();
    ImGuiGLFW::NewFrame();

    cpuDebugger.ComposeView( *cpu, cycles );
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
