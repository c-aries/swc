#include <stdio.h>
#include <string.h>
#include <Imlib2.h>
#include "image.h"
#include "video.h"
#include "opt.h"
#include "colorspace.h"
#include "common.h"

#define IMAGE_QUERY_TURN 5

extern struct video video;
extern struct options opt;
struct image image;

static void update_image_pixels(int index);
static void update_image_pixels_by_addr(const void *start);
static void save_image(char *filename);

static void
update_image_pixels(int index)
{
  update_image_pixels_by_addr(video.buffer.buf[index].start);
}

static void
update_image_pixels_by_addr(const void *start)
{
  unsigned char *data = (unsigned char *)start;
  unsigned char *pixels = image.pixels;
  int width = opt.width;	/* width, height应该归于video buffer结构体管理 */
  int height = opt.height;
  unsigned char Y, Cr, Cb;
  int r, g, b;
  int x, y;
  int p1, p2, p3, p4;

  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      p1 = y * width * 2 + x * 2;
      Y = data[p1];
      if (x % 2 == 0) {
	p2 = y * width * 2 + (x * 2 + 1);
	p3 = y * width * 2 + (x * 2 + 3);
      }
      else {
	p2 = y * width * 2 + (x * 2 - 1);
	p3 = y * width * 2 + (x * 2 + 1);
      }
      Cb = data[p2];
      Cr = data[p3];
      yuv2rgb(Y, Cb, Cr, &r, &g, &b);
      p4 = y * width * 4 + x * 4;
      pixels[p4] = b;
      pixels[p4 + 1] = g;
      pixels[p4 + 2] = r;
      pixels[p4 + 3] = 255;
    }
  }
}

static void
save_image(char *filename)
{
  Imlib_Image img;
  int width = image.width;
  int height = image.height;
  DATA32 *data;
  char *suffix;

  if (filename == NULL) {
    return;
  }
  suffix = strrchr(filename, '.');
  if (suffix) {
    data = (DATA32 *)image.pixels;
    img = imlib_create_image_using_data(width, height, data);
    if (img) {
      imlib_context_set_image(img);
      imlib_image_set_format(suffix + 1);
      imlib_save_image(filename);
      imlib_free_image();
    }
  }
  else {
    fprintf(stderr, "wrong file name: %s\n", filename);
    exit(EXIT_FAILURE);
  }
}

void
image_init()
{
  image.width = opt.width;
  image.height = opt.height;
  image.pixels_num = image.width * image.width * 4;
  image.pixels = (unsigned char *)malloc(image.pixels_num);
}

void
image_quit()
{
  free(image.pixels);
}

void
image_get()
{
  int i, t, index;
  char *filename;

  for (t = 0; t < IMAGE_QUERY_TURN; t++) {
    printf("%d .. ", IMAGE_QUERY_TURN - t);
    fflush(stdout);
    for (i = 0; i < video.buffer.req.count; i++) {
      buffer_dequeue(i);
      buffer_enqueue(i);
    }
  }
  printf("%d ..\n", IMAGE_QUERY_TURN - t);
  fflush(stdout);
  index = 0;
  buffer_dequeue(index);
  update_image_pixels(index);
  filename = get_file_name("swc", "jpeg");
  save_image(filename);
  printf("%s saved.\n", filename);
  free(filename);
  buffer_enqueue(index);
}
