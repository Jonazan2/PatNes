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
#include "../Video.h"


Debugger::Debugger( Cpu *cpu, Memory *memory, Video *video )
    : cpu( cpu )
    , memory( memory )
    , video( video )
    , mode( DebuggerMode::IDLE )
    , window( nullptr )
{
}

Debugger::~Debugger()
{
    delete leftPatternTableBuffer;
    delete rightPatternTableBuffer;
}


void Debugger::StartDebugger()
{
    glfwInit();
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 5 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    window = glfwCreateWindow( 1920, 1080, "PatNes", nullptr, nullptr );
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

    leftPatternTableBuffer = new RGB[ 128 * 128 ];
    ImGuiGLFW::Texture leftPatternTexture = { 0, 128, 128, leftPatternTableBuffer };
    leftPatternTableTextureID = ImGuiGLFW::CreateTexture( leftPatternTexture );

    rightPatternTableBuffer = new RGB[ 128 * 128 ];
    ImGuiGLFW::Texture rightPatternTexture = { 0, 128, 128, rightPatternTableBuffer };
    rightPatternTableTextureID = ImGuiGLFW::CreateTexture( rightPatternTexture );

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

            if ( elapsed.count() < 16.6f ) 
            {
                std::this_thread::sleep_for( std::chrono::duration< float, std::milli > ( 16.6F - elapsed.count() ) );
            }
        }
    }
    else
    {
        /* In normal mode just render the debugger at the same rate as the emulator: random number for now */
        if ( cycles % 10000 )
        {
            ComposeView( cycles );
            Render();
        }
    }
}

void Debugger::ComposeView( u32 cycles )
{
    glfwPollEvents();
    ImGuiGLFW::NewFrame();

    cpuDebugger.ComposeView( *cpu, *memory, cycles, mode );

    UpdatePatternTable( 0x0000, leftPatternTableBuffer );
    ImGui::SetNextWindowSize( ImVec2( 560, 560 ), ImGuiCond_FirstUseEver );
    ImGui::Begin( "VRAM Left" );
    ImGui::Image( leftPatternTableTextureID, ImVec2( 512, 512 ) );
    ImGui::End();

    UpdatePatternTable( 0x1000, rightPatternTableBuffer );
    ImGui::SetNextWindowSize( ImVec2( 560, 560 ), ImGuiCond_FirstUseEver );
    ImGui::Begin( "VRAM Right" );
    ImGui::Image( rightPatternTableTextureID, ImVec2( 512, 512 ) );
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


void Debugger::UpdatePatternTable( word address, RGB *buffer )
{
    constexpr static RGB palette[4] = { { 255,255,255 },{ 0xCC,0xCC,0xCC },{ 0x77,0x77,0x77 }, { 0x0,0x0,0x0 } };

    const byte * const ppuMemory = video->GetPPUMemory();
    assert( buffer != nullptr );
    assert( ppuMemory != nullptr );

    u32 vramPosition = 0;
    for ( u32 tile = 0; tile < 256; ++tile )
    {
        const u32 tileAddress = (tile * 0x10) + address;

        u32 localvramPosition = vramPosition;
        for ( byte row = 0; row < 8; ++row )
        {
            const byte firstByte = ppuMemory[ tileAddress + row ];
            const byte secondByte = ppuMemory[ tileAddress + row + 0x07 ];

            for ( byte column = 0; column < 8; ++column)
            {
                byte mask = 0x01 << column;
                const byte value = ((secondByte & mask) >> column) << 1;
                const byte value2 = (firstByte & mask) >> column;
                const byte finalValue = value | value2;

                buffer[ localvramPosition + (7 - column) ] = palette[ finalValue ];
            }
            localvramPosition += 128;
        }
        vramPosition += 8;

        if ( tile > 0 && (tile + 1) % 16 == 0)
        {
            vramPosition += 128 * 7;
        }
    }
}