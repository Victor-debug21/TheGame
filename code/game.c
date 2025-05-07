#include <windows.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define internal static
#define local_persist static
#define global_variable static

#include "game.h"


typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;



global_variable bool GlobalRunning;
global_variable struct win32_offscreen_buffer GlobalBackBuffer;
global_variable bool ShowDebugInfo = false;

internal struct win32_window_dimension
Win32GetWindowDimension(HWND Window)
{
  struct win32_window_dimension Result;

  RECT ClientRect;
  GetClientRect(Window, &ClientRect);
  Result.Width = ClientRect.right - ClientRect.left;
  Result.Height = ClientRect.bottom - ClientRect.top;

  return(Result);
}

internal void
Win32ResizeDIBSection(struct win32_offscreen_buffer *Buffer, int Width, int Height)
{
  if(Buffer->Memory)
    {
      VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
    }

  Buffer->Width = Width;
  Buffer->Height = Height;

  Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
  Buffer->Info.bmiHeader.biWidth = Buffer->Width;
  Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
  Buffer->Info.bmiHeader.biPlanes = 1;
  Buffer->Info.bmiHeader.biBitCount = 32;
  Buffer->Info.bmiHeader.biCompression = BI_RGB;

  int BytesPerPixel = 4;
  int BitmapMemorySize = (Buffer->Width*Buffer->Height)*BytesPerPixel;
  Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
  Buffer->Pitch = Width*BytesPerPixel;
}

internal void
Win32GetBufferInWindow(struct win32_offscreen_buffer *Buffer, HDC DeviceContext, int WindowWidth, int WindowHeight,
                       char *FPSString)
{
  StretchDIBits(DeviceContext,
		0, 0, WindowWidth, WindowHeight,
		0, 0, Buffer->Width, Buffer->Height,
		Buffer->Memory,
		&Buffer->Info,
		DIB_RGB_COLORS,
		SRCCOPY);

  
  SetBkMode(DeviceContext, TRANSPARENT);
  SetTextColor(DeviceContext, RGB(255, 255, 0));
  if(ShowDebugInfo)
    {
      TextOutA(DeviceContext, 10, 10, FPSString, (int)strlen(FPSString));
    }
}


internal void
ProcessPlayerInput(HWND Window)
{
  short EscapeKeyIsDown = GetAsyncKeyState(VK_ESCAPE);
  
  if(EscapeKeyIsDown)
    {
      SendMessageA(Window, WM_CLOSE, 0, 0);
    }
  
}


internal void
RenderFrameGraphics(struct win32_offscreen_buffer *Buffer, uint8 Red, uint8 Green, uint8 Blue)
{
    // Each pixel is 32 bits: 0x00BBGGRR (little-endian)
    uint8 *Row = (uint8 *)Buffer->Memory;

    for (int Y = 0; Y < Buffer->Height; ++Y)
    {
        uint32 *Pixel = (uint32 *)Row;
        for (int X = 0; X < Buffer->Width; ++X)
        {
            uint8 R = Red;
            uint8 G = Green;
            uint8 B = Blue;

            *Pixel++ = ((R << 16) | (G << 8) | B); // 0x00RRGGBB
        }

        Row += Buffer->Pitch;
    }
}


LRESULT CALLBACK
Win32MainWindowCallBack(HWND Window,
                        UINT Message,
                        WPARAM WParam,
                        LPARAM LParam)
{
  LRESULT Result = 0;
  switch(Message)
    {
    case WM_CLOSE:
      {
        GlobalRunning = false;
      }break;
    case WM_ACTIVATEAPP:
      {
        OutputDebugStringA("WM_ACTIVATEAPP");
      }break;
    case WM_DESTROY:
      {
        GlobalRunning = false;
      }break;
    case WM_SYSKEYUP:
    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
      {
        uint8 VKCode = (uint8)(WParam & 0xFF);
        bool IsDown = ((LParam & (1 << 30)) == 0);
        bool WasDown = ((LParam & (1 << 31)) != 0);
        if(WasDown != IsDown)
          {
            if(VKCode == VK_F10 && IsDown)
              {
                ShowDebugInfo = !ShowDebugInfo;
              }
          }
      }
    case WM_KEYUP:
      {
        
        uint8 VKCode = (uint8)(WParam & 0xFF);
        bool IsDown = ((LParam & (1 << 30)) == 0);
        bool WasDown = ((LParam & (1 << 31)) != 0);
        if(WasDown != IsDown)
          {
            if(VKCode == 'W')
              {
              }
            else if(VKCode == 'S')
              {
              }
            else if(VKCode == 'A')
              {
              }
            else if(VKCode == 'D')
              {
              }
            else if(VKCode == 'Q')
              {
              }
            else if(VKCode == 'E')
              {
              }
            else if(VKCode == VK_SPACE)
              {
              }
            else if(VKCode == VK_ESCAPE)
              {
                OutputDebugStringA("Escape:");
                if(IsDown)
                  {
                    OutputDebugStringA("IsDown");
                  }
                if(WasDown)
                  {
                    OutputDebugStringA("WasDown");
                  }
                OutputDebugStringA("\n");
              }
          }
        
        int32 AltKeyWasDown = ((LParam & (1 << 29)) != 0);
        
        if(VKCode == VK_F4 && AltKeyWasDown)
          {
            OutputDebugStringA("\t\nAltKeyWasDown");
            GlobalRunning = false;
          }
      }break;
    case WM_PAINT:
      {
        PAINTSTRUCT Paint;
        HDC DeviceContext = BeginPaint(Window, &Paint);
        static char FPSBuffer[256] = "FPS: calculating...";
        struct win32_window_dimension Dimension = Win32GetWindowDimension(Window);
        Win32GetBufferInWindow(&GlobalBackBuffer, DeviceContext, Dimension.Width, Dimension.Height, FPSBuffer);
        EndPaint(Window, &Paint);
      }break;
    default:
      {
        Result = DefWindowProc(Window, Message, WParam, LParam);
      }break;
    }
  return(Result);
}

int CALLBACK
WinMain(HINSTANCE Instance,
        HINSTANCE PrevInstance,
        LPSTR CommandLine,
        int ShowCode)
{
  (void)PrevInstance;
  (void)CommandLine;
  (void)ShowCode;

  LARGE_INTEGER PerformanceFrequency;
  QueryPerformanceFrequency(&PerformanceFrequency);

  LARGE_INTEGER LastCounter;
  QueryPerformanceCounter(&LastCounter);
  
  DWORD Result;
  
  WNDCLASSA WindowClass = {};
  Win32ResizeDIBSection(&GlobalBackBuffer, 1920, 1080);
  WindowClass.style = 0;
  WindowClass.lpfnWndProc = Win32MainWindowCallBack;
  WindowClass.hInstance = Instance;
  //  WindowClass.hIcon;
  WindowClass.lpszClassName = "???????";
  
  if(RegisterClassA(&WindowClass))
    {
      HWND Window = CreateWindowExA(0,
                                    WindowClass.lpszClassName,
                                    "????????",
                                    WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                                    CW_USEDEFAULT,
                                    CW_USEDEFAULT,
                                    1920,
                                    1080,
                                    0,
                                    0,
                                    Instance,
                                    0);
      
      if(Window)
        {
          
          SetWindowLongA(Window, GWL_STYLE, WS_VISIBLE|WS_POPUP);
          SetWindowPos(Window, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN),
                       GetSystemMetrics(SM_CYSCREEN), SWP_FRAMECHANGED | SWP_SHOWWINDOW);
          
          HDC DeviceContext = GetDC(Window);
          GlobalRunning = true;
          
          while(GlobalRunning)
            {
              MSG Message = {0};
              while(PeekMessageA(&Message, Window, 0, 0, PM_REMOVE))
                {
                  if(Message.message == WM_QUIT)
                    {
                      GlobalRunning = false;
                    }
                  TranslateMessage(&Message);
                  DispatchMessageA(&Message);
                }

              ProcessPlayerInput(Window);

              float FPS = 0.0f;
              static float FPSDisplayTimer = 0.0f;
              static int FrameCounter = 0;
              static char FPSBuffer[256] = "FPS: calculating...";

              struct win32_window_dimension Dimension = Win32GetWindowDimension(Window);
              Win32GetBufferInWindow(&GlobalBackBuffer, DeviceContext, Dimension.Width, Dimension.Height, FPSBuffer);
              RenderFrameGraphics(&GlobalBackBuffer, 51, 204, 204);
              //Dont worry we are gonna change that Sleep and CPU was too much for a blank loop.
              //It was using (Ryzen 5 5600x) 1 threat almost %100 percent so this was ridiculous
              //And we just decrease that percent with sleep function
              Sleep(1);

              
              LARGE_INTEGER EndCounter;
              QueryPerformanceCounter(&EndCounter);

              uint64 CounterElapsed = EndCounter.QuadPart - LastCounter.QuadPart;
              float SecondsElapsed = (float)CounterElapsed / (float)PerformanceFrequency.QuadPart;

              FPSDisplayTimer += SecondsElapsed;
              FrameCounter++;

              if(FPSDisplayTimer >= 0.5f)
                {
                  FPS = (float)FrameCounter / FPSDisplayTimer;
                  sprintf_s(FPSBuffer, sizeof(FPSBuffer), "FPS: %.2f", FPS);

                  FPSDisplayTimer = 0.0f;
                  FrameCounter = 0;
                }

              LastCounter = EndCounter;

            }
          
        }
      else
        {
          Result = GetLastError();
          MessageBoxA(0, "Window MessageBox Failed", "Error!", MB_ICONINFORMATION|MB_OK);
          goto Exit;
        }

      
    }
  else
    {
      Result = GetLastError();
      MessageBoxA(0, "Window Registeration Failed", "Error!", MB_ICONINFORMATION|MB_OK);
      goto Exit;
    }
  
 Exit:
  return(0);
}

