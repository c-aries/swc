#ifndef IMAGE_H
#define IMAGE_H

struct image {
  int width;
  int height;
  unsigned char *pixels;
  int pixels_num;
};

void image_init();
void image_quit();
void image_get();

#endif
