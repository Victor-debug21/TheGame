#if !defined(GAME_H)


struct win32_offscreen_buffer
{
  BITMAPINFO Info;
  int Width;
  int Height;
  void *Memory;
  int Pitch;
    
};

struct win32_window_dimension
{
  int Width;
  int Height;
};



#define GAME_H
#endif
