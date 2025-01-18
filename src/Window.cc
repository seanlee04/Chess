#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include "GraphicsObserver.h"
#include "Window.h"
#include <memory.h>

#include "piece-imgs2/black_bishop.h"

using namespace std;

Xwindow::Xwindow(int width, int height) : width{width}, height{height} {

  d = XOpenDisplay(NULL);
  if (d == NULL) {
    cerr << "Cannot open display" << endl;
    exit(1);
  }
  s = DefaultScreen(d);
  w = XCreateSimpleWindow(d, RootWindow(d, s), 10, 10, width, height, 1,
                          BlackPixel(d, s), WhitePixel(d, s));
  XSelectInput(d, w, ExposureMask | KeyPressMask);
  XMapRaised(d, w);

  Pixmap pix = XCreatePixmap(d,w,width,
        height,DefaultDepth(d,DefaultScreen(d)));
  gc = XCreateGC(d, pix, 0,(XGCValues *)0);

  XFlush(d);
  XFlush(d);

  // Set up colours.
  XColor xcolour;
  Colormap cmap;
  int color_vals[8][3]={
    {118, 150, 86}, // green
    {238, 238, 210}, // white
    {178, 227, 185}, // coral light
    {111, 163, 162}, // coral dark
    {205, 183, 175}, // dusk light
    {113, 110, 160}, // dusk dark
    {158, 171, 255}, // marine light
    {112, 116, 211}, // marine dark
}; 

  cmap=DefaultColormap(d,DefaultScreen(d));
  for(int i=0; i < 8; ++i) {
    xcolour.red = color_vals[i][0]*257;
    xcolour.green = color_vals[i][1]*257;
    xcolour.blue = color_vals[i][2]*257;
    xcolour.flags = DoRed | DoGreen | DoBlue;
    XAllocColor(d,cmap,&xcolour);
    colours[i]=xcolour.pixel;
  }

  int count = 0;
  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      for(int k = 0; k < 4; k++){
        xcolour.red = i*9362;
        xcolour.green = j*9362;
        xcolour.blue = k*21845;
        xcolour.flags = DoRed | DoGreen | DoBlue;
        XAllocColor(d,cmap,&xcolour);
        customColours[count]=xcolour.pixel;
        count++;
      }
    }
  }

  XSetForeground(d,gc,colours[White]);

  // Make window non-resizeable.
  XSizeHints hints;
  hints.flags = (USPosition | PSize | PMinSize | PMaxSize );
  hints.height = hints.base_height = hints.min_height = hints.max_height = height;
  hints.width = hints.base_width = hints.min_width = hints.max_width = width;
  XSetNormalHints(d, w, &hints);

  XSynchronize(d,True);

  usleep(1000);
}

Xwindow::~Xwindow() {
  XFreeGC(d, gc);
  XCloseDisplay(d);
}

void Xwindow::fillRectangle(int x, int y, int width, int height, int colour) {
  XSetForeground(d, gc, colours[colour]);
  XFillRectangle(d, w, gc, x, y, width, height);
  XSetForeground(d, gc, colours[White]);
}

void Xwindow::drawPiece(int x, int y, int bgCol, const unsigned int (&pieceImg)[50*50]){
  XImage* image = XCreateImage(d, DefaultVisual(d, 0), DefaultDepth(d,s), ZPixmap, 0, nullptr, GraphicsObserver::GRID_SIZE, GraphicsObserver::GRID_SIZE, 8, 0);
  // Allocate memory for the image data
  std::unique_ptr<char[]> pixelArr = std::make_unique<char[]>(GraphicsObserver::GRID_SIZE * GraphicsObserver::GRID_SIZE * (image->bits_per_pixel / 8));
  image->data = pixelArr.get();

  // Fill the image with pixel values (grayscale values)
  for (int j = 0; j < GraphicsObserver::GRID_SIZE; ++j) {
      for (int i = 0; i < GraphicsObserver::GRID_SIZE; ++i) {
          unsigned int color = pieceImg[i*GraphicsObserver::GRID_SIZE + j];
          uint16_t r = (color >> 11) & 0x1F; // Extract bits 11-15 (5 bits for R)
          uint16_t g = (color >> 6) & 0x1F;  // Extract bits 6-10 (5 bits for G)
          uint16_t b = (color >> 1) & 0x1F;  // Extract bits 1-5 (5 bits for B)
          uint16_t a = color & 0b1;

          // Convert to 3-bit R, 3-bit G, 2-bit B
          uint8_t r8 = r >> 2; // Reduce 5 bits to 3 bits
          uint8_t g8 = g >> 2; // Reduce 5 bits to 3 bits
          uint8_t b8 = b >> 3; // Reduce 5 bits to 2 bits

          uint8_t pixel_value = (r8 << 5) | (g8 << 2) | b8;
          if(a != 0x00){
            XPutPixel(image, j, i, customColours[pixel_value]);
          }
          else{
            XPutPixel(image, j, i, colours[bgCol]);
          }
      }
  }
  XPutImage(d, w, gc, image, 0, 0, x, y, GraphicsObserver::GRID_SIZE, GraphicsObserver::GRID_SIZE);
  image->data = nullptr;
  XDestroyImage(image);
}

