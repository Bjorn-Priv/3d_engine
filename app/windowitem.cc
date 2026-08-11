#include "include/windowitem.h"

void WindowItem::updateSize(double w, double h) {
  x = x * w;
  width = width * w;
  y = y * h;
  height = height * h;
}

void WindowItem::setSize(int nx, int ny, int nwidth, int nheight) {
  x = nx;
  y = ny;
  width = nwidth;
  height = nheight;
}