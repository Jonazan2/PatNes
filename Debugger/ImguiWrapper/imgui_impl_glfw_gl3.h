#pragma once

#include "../Imgui/imgui.h"


struct GLFWwindow;


namespace ImGuiGLFW {

    typedef struct {
        unsigned int id;
        int width;
        int height;
        void* buffer;
    } Texture;

    IMGUI_API bool			Init(GLFWwindow* window, bool install_callbacks);
    IMGUI_API ImTextureID   CreateTexture(Texture& buffer);
    IMGUI_API void			Bind_Textures();
    IMGUI_API void			UpdateTexture(ImTextureID textureId, void* buffer);

    IMGUI_API void			Shutdown();
    IMGUI_API void			NewFrame();

    IMGUI_API void			InvalidateDeviceObjects();
    IMGUI_API bool			CreateDeviceObjects();

    IMGUI_API void			MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    IMGUI_API void			ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    IMGUI_API void			KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    IMGUI_API void			CharCallback(GLFWwindow* window, unsigned int c);

    IMGUI_API void			RenderDrawLists(ImDrawData* draw_data);
}
