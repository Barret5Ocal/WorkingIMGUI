#include <windows.h>

static int Running = 1; 

#include "b50_timing.h"

#include <float.h>

#define GB_MATH_IMPLEMENTATION
#include "include\gb_math.h"
typedef gbVec2 v2;
typedef gbVec3 v3;
typedef gbVec4 v4;
typedef gbMat4 m4;
typedef gbQuat quaternion;

#define STB_IMAGE_IMPLEMENTATION
#include "include\stb_image.h"

//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include "include\stb_image_write.h"

#define STB_SPRINTF_IMPLEMENTATION
#include "include\stb_sprintf.h"

#include <stdint.h>
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float real32;
typedef double real64;

#define global_variable static 
#define local_persist static 
#define internal static 

#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}
#define InvalidCodePath Assert(false)
#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#define Kilobyte(Value) 1024 * Value
#define Megabyte(Value) 1024 * Kilobyte(Value)
#define Gigabyte(Value) 1024 * Megabyte(Value)

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#include "include\win32_opengl.h"


#include "include\imgui.cpp"
#include "include\imgui.h"
#include "include\imgui_demo.cpp"
#include "include\imgui_draw.cpp"
#include "include\imgui_internal.h"
#include "include\imconfig.h"
#include "include\imgui_impl_opengl3.cpp"

int LeftMouse = 0; 

struct win32_windowdim 
{
    int Width, Height; 
    int x, y;
    //int DisplayWidth, DisplayHeight; 
};

win32_windowdim Win32GetWindowDim(HWND Window)
{
    win32_windowdim Dim = {};
    
    RECT Rect = {};
    //GetClientRect(Window, &Rect);
    GetWindowRect(Window, &Rect);
    Dim.x = Rect.left;
    Dim.y = Rect.top;
    Dim.Width = Rect.right - Rect.left;
    Dim.Height = Rect.bottom - Rect.top;
    return Dim; 
}

LRESULT CALLBACK
MainWindowProc(HWND   Window,
               UINT   Message,
               WPARAM WParam,
               LPARAM LParam)
{
    LRESULT Result = {};
    
    switch(Message)
    {
        case WM_DESTROY: 
        {
            Running = 0;
        }break;
        case WM_CLOSE:
        {
            Running = 0; 
        }break;
        case WM_LBUTTONDOWN: 
        {
            LeftMouse = 1;
        }break;
        case WM_LBUTTONUP: 
        {
            LeftMouse = 0;
        }break;
        default:
        {
            Result = DefWindowProc(Window, Message, WParam, LParam);
        }break;
    }
    
    return Result; 
}

int WinMain(HINSTANCE Instance, 
            HINSTANCE PrevInstance, 
            LPSTR CmdLine, 
            int CodeShow)
{
    WNDCLASS WindowClass = {};
    //WindowClass.sytle = 0;
    WindowClass.lpfnWndProc = MainWindowProc; 
    WindowClass.hInstance = Instance;
    WindowClass.lpszClassName = "RENDERING";
    
    if(RegisterClass(&WindowClass))
    {
        HWND Window = CreateWindowExA(0,
                                      WindowClass.lpszClassName,
                                      "Renderer",
                                      WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                                      CW_USEDEFAULT,
                                      CW_USEDEFAULT, 
                                      SCREEN_WIDTH,
                                      SCREEN_HEIGHT, 
                                      0, 0,
                                      Instance, 
                                      0);
        
        if(Window)
        {
            
            Win32InitOpenGL(Window);
            
            ImGui::CreateContext();
            
            win32_windowdim Dim = Win32GetWindowDim(Window);
            
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            io.DisplaySize = {(float)SCREEN_WIDTH, (float)SCREEN_HEIGHT};
            float FrameRate = 60;
            float dt = 1 / FrameRate; 
            io.DeltaTime = dt;
            
            ImGui_ImplOpenGL3_Init(0);
            ImGui::StyleColorsDark();
            
            bool show_demo_window = true;
            bool show_another_window = false;
            v4 ClearColor = {0.45f, 0.55f, 0.60f, 1.00f};
            
            while(Running)
            {
                MSG Message;
                while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
                {
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                }
                
                Dim = Win32GetWindowDim(Window);
                
                POINT Point = {}; 
                GetCursorPos(&Point);
                
                io.MousePos = {(float)Point.x - Dim.x - 5, (float)Point.y - Dim.y + 5};
                
                //io.MouseDown[0] = 0;
                io.MouseDown[0] = LeftMouse;
                
                ImGui_ImplOpenGL3_NewFrame();
                ImGui::NewFrame();
                
                {
                    static float f = 0.0f;
                    static int counter = 0;
                    ImGui::Text("Hello, world!");                           // Display some text (you can use a format string too)
                    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
                    ImGui::ColorEdit3("clear color", (float*)&ClearColor); // Edit 3 floats representing a color
                    
                    ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our windows open/close state
                    ImGui::Checkbox("Another Window", &show_another_window);
                    
                    if (ImGui::Button("Button"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
                        counter++;
                    ImGui::SameLine();
                    ImGui::Text("counter = %d", counter);
                    
                    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                }
                
                if (show_another_window)
                {
                    ImGui::Begin("Another Window", &show_another_window);
                    ImGui::Text("Hello from another window!");
                    if (ImGui::Button("Close Me"))
                        show_another_window = false;
                    ImGui::End();
                }
                
                // 3. Show the ImGui demo window. Most of the sample code is in ImGui::ShowDemoWindow(). Read its code to learn more about Dear ImGui!
                if (show_demo_window)
                {
                    ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
                    ImGui::ShowDemoWindow(&show_demo_window);
                }
                
                
                
                ImGui::Render();
                glClearColor(ClearColor.x, ClearColor.y, ClearColor.z, ClearColor.w);
                glClear(GL_COLOR_BUFFER_BIT);  
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                Win32RenderFrame(Window, SCREEN_WIDTH, SCREEN_HEIGHT);
                
            }
        }
    }
    return 0; 
}

