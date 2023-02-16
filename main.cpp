#include <Windows.h>
#include <stdio.h>
#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include <GL/glew.h>
#include <gl/GL.h>
#include <GL/wglext.h>
#include <tchar.h>

#include "dragAndDropHandler.h"

//                               GLOBAL VARIABLES
// =============================================================================
HGLRC g_GLRenderContext;
HDC g_HDCDeviceContext;
HWND g_hwnd;
PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT;

int g_display_w = 800;
int g_display_h = 600;

char fileDropPath[300];
bool fileOkay = true;
bool hovering = false;

void setLastFileDropPath(char *tempFilePath, size_t filePathLength)
{
    memcpy_s(fileDropPath, 300, tempFilePath, filePathLength);
}

void fileDragHovering(bool isHovering, bool acceptedType)
{
    std::cout << "File Drag Hovering: " << (isHovering) << std::endl;
    hovering = isHovering;
}

// =============================================================================
//                             FOWARD DECLARATIONS
// =============================================================================
void CreateGlContext();
void SetCurrentContext();
bool SetSwapInterval(int interval); // 0 - No Interval, 1 - Sync whit VSYNC, n - n times Sync with VSYNC
bool WGLExtensionSupported(const char *extension_name);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// =============================================================================
//                            CORE MAIN FUNCTIONS
// =============================================================================
//
// Aplication Entry
//------------------------------------------------------------------------------
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    OleInitialize(NULL);
    std::function<void(char *, size_t)> fileDropCallback = setLastFileDropPath;
    std::function<void(bool, bool)> fileHoverCallback = fileDragHovering;
    DragAndDropHandler g_pDropTarget(fileDropCallback, fileHoverCallback);

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
    wc.lpszClassName = "NCUI";
    wc.style = CS_OWNDC;
    if (!RegisterClass(&wc))
        return 1;
    g_hwnd = CreateWindow(wc.lpszClassName, "teste", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, 640, 480, 0, 0, hInstance, 0);

    HRESULT result = RegisterDragDrop(g_hwnd, &g_pDropTarget);
    // Show the window
    ShowWindow(g_hwnd, SW_SHOWDEFAULT);
    UpdateWindow(g_hwnd);

    // Prepare OpenGlContext
    CreateGlContext();
    SetSwapInterval(1);
    glewInit();

    // Setup Dear ImGui binding
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    // Init Win32
    ImGui_ImplWin32_Init(g_hwnd);

    // Init OpenGL Imgui Implementation
    //  GL 3.0 + GLSL 130
    const char *glsl_version = "#version 130";
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Set Window bg color
    if (hovering)
    {
        std::cout << "Hovering!" << std::endl;
    }
    ImVec4 clear_color = hovering ? ImVec4(0.52F, 0.98F, 0.67F, 1.0F) : ImVec4(0.32F, 0.78F, 0.47F, 0.7F);

    // Setup style
    ImGui::StyleColorsClassic();

    io.Fonts->AddFontFromFileTTF("./fonts/RobotoMono-Regular.ttf", 24);

    // Main loop
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    ImGui::GetStyle().Colors[ImGuiCol_MenuBarBg] = ImVec4(0, 0, 0, 0.2f);
    while (msg.message != WM_QUIT)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        char buf[100];
        float f = 0;
        ImGui::NewFrame();
        // show Main Window

        // ImGui::Text("%s", fileDropPath);
        bool my_tool_active = true;
        ImGui::Begin("My First Tool", &my_tool_active,
                     ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
        ImVec2 wSize = ImVec2(g_display_w, g_display_h);
        ImGui::SetWindowPos(ImVec2(0, 0));
        ImGui::SetWindowSize(wSize, ImGuiCond_Always);
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open..", "Ctrl+O"))
                { /* Do stuff */
                }
                if (ImGui::MenuItem("Save", "Ctrl+S"))
                { /* Do stuff */
                }
                if (ImGui::MenuItem("Close", "Ctrl+W"))
                {
                    my_tool_active = false;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::TextColored(ImVec4(1, 1, 0, 1), "Important Stuff");

        ImGui::End();

        // Rendering
        ImGui::Render();
        wglMakeCurrent(g_HDCDeviceContext, g_GLRenderContext);
        glViewport(0, 0, g_display_w, g_display_h); // Display Size got from Resize Command
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        wglMakeCurrent(g_HDCDeviceContext, g_GLRenderContext);
        SwapBuffers(g_HDCDeviceContext);
    }
    RevokeDragDrop(g_hwnd);

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    wglDeleteContext(g_GLRenderContext);
    ImGui::DestroyContext();
    ImGui_ImplWin32_Shutdown();

    DestroyWindow(g_hwnd);
    UnregisterClass(_T("NCUI"), wc.hInstance);
    OleUninitialize();
    return 0;
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam != SIZE_MINIMIZED)
        {
            g_display_w = (UINT)LOWORD(lParam);
            g_display_h = (UINT)HIWORD(lParam);
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

void CreateGlContext()
{

    PIXELFORMATDESCRIPTOR pfd =
        {
            sizeof(PIXELFORMATDESCRIPTOR),
            1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, // Flags
            PFD_TYPE_RGBA,                                              // The kind of framebuffer. RGBA or palette.
            32,                                                         // Colordepth of the framebuffer.
            0, 0, 0, 0, 0, 0,
            0,
            0,
            0,
            0, 0, 0, 0,
            24, // Number of bits for the depthbuffer
            8,  // Number of bits for the stencilbuffer
            0,  // Number of Aux buffers in the framebuffer.
            PFD_MAIN_PLANE,
            0,
            0, 0, 0};

    g_HDCDeviceContext = GetDC(g_hwnd);

    int pixelFormal = ChoosePixelFormat(g_HDCDeviceContext, &pfd);
    SetPixelFormat(g_HDCDeviceContext, pixelFormal, &pfd);
    g_GLRenderContext = wglCreateContext(g_HDCDeviceContext);
    wglMakeCurrent(g_HDCDeviceContext, g_GLRenderContext);
}

bool SetSwapInterval(int interval)
{
    if (WGLExtensionSupported("WGL_EXT_swap_control"))
    {
        // Extension is supported, init pointers.
        wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

        // this is another function from WGL_EXT_swap_control extension
        wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC)wglGetProcAddress("wglGetSwapIntervalEXT");

        wglSwapIntervalEXT(interval);
        return true;
    }

    return false;
}

// Got from https://stackoverflow.com/questions/589064/how-to-enable-vertical-sync-in-opengl/589232
bool WGLExtensionSupported(const char *extension_name)
{
    // this is pointer to function which returns pointer to string with list of all wgl extensions
    PFNWGLGETEXTENSIONSSTRINGEXTPROC _wglGetExtensionsStringEXT = NULL;

    // determine pointer to wglGetExtensionsStringEXT function
    _wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)wglGetProcAddress("wglGetExtensionsStringEXT");

    if (strstr(_wglGetExtensionsStringEXT(), extension_name) == NULL)
    {
        // string was not found
        return false;
    }

    // extension is supported
    return true;
}
