#ifndef __WINDOW_H__
#define __WINDOW_H__
#include <X11/Xlib.h>
#include <iostream>
#include <string>

class Xwindow {
  Display *d;
  Window w;
  int s, width, height;
  GC gc;
  unsigned long colours[5];
  unsigned long customColours[256];

 public:
  Xwindow(int width=500, int height=500);  // Constructor; displays the window.
  ~Xwindow();                              // Destructor; destroys the window.

  enum {Green=0, White, coralLight, coralDark,
        duskLight, duskDark, marineLight, marineDark}; // Available colours.

  // Draws a rectangle
  void fillRectangle(int x, int y, int width, int height, int colour=Green);

  void drawPiece(int x, int y, int bgCol, const unsigned int (&pieceImg)[50*50]);

};

#endif
