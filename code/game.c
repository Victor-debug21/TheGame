#include <windows.h>
#include <stdbool.h>
#include <stdint.h>

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
Win32GetBufferInWindow(struct win32_offscreen_buffer *Buffer, HDC DeviceContext, int WindowWidth, int WindowHeight)
{
  StretchDIBits(DeviceContext,
		0, 0, WindowWidth, WindowHeight,
		0, 0, Buffer->Width, Buffer->Height,
		Buffer->Memory,
		&Buffer->Info,
		DIB_RGB_COLORS,
		SRCCOPY);

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
    case WM_KEYUP:
      {
        uint8 VKCode = WParam;
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
        
        int32 AltKeyWasDown = LParam & (1 << 29) != 0;
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
        struct win32_window_dimension Dimension = Win32GetWindowDimension(Window);
        Win32GetBufferInWindow(&GlobalBackBuffer, DeviceContext, Dimension.Width, Dimension.Height);
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

  DWORD Result;
  
  WNDCLASSA WindowClass = {};
  Win32ResizeDIBSection(&GlobalBackBuffer, 1280, 720);
  WindowClass.style = CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
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
                                    CW_USEDEFAULT,
                                    CW_USEDEFAULT,
                                    0,
                                    0,
                                    Instance,
                                    0);
      
      if(Window)
        {
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

              //Dont worry we are gonna change that Sleep and CPU was too much for a blank loop.
              //It was using almost (Ryzen 5 5600x) 1 threat almost %100 percent so this was ridiculous
              //And we just decrease that percent with sleep function
              Sleep(1);
              ProcessPlayerInput(Window);
              struct win32_window_dimension Dimension = Win32GetWindowDimension(Window);
              Win32GetBufferInWindow(&GlobalBackBuffer, DeviceContext, Dimension.Width, Dimension.Height);
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
