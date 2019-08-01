#include "Debugger.h"

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include "ImguiWrapper/imgui_impl_glfw_gl3.h"

Debugger::Debugger()
    : mode (DebuggerMode::RUNNING)
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

    ImGuiGLFW::Init(window, true);
    ImGui::StyleColorsDark();

    Update(0.f,0);
}


void Debugger::Update( float deltaMilliseconds, unsigned int cycles )
{
    ComposeView(deltaMilliseconds, cycles);
    Render();
}

void Debugger::CloseDebugger()
{
    ImGuiGLFW::Shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Debugger::ComposeView( float deltaMilliseconds, unsigned int cycles )
{
    glfwPollEvents();
    ImGuiGLFW::NewFrame();

    ImGui::Begin("PatNes");
    ImGui::Text("First version of the PatNes debugger");
    ImGui::End();
}

void Debugger::Render()
{
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui::Render();
    ImGuiGLFW::RenderDrawLists(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}