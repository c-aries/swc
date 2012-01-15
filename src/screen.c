#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Imlib2.h>
#include "screen.h"
#include "opt.h"
#include "video.h"
#include "common.h"
#include "colorspace.h"

extern struct options opt;
extern struct video video;
struct screen screen;

static void sdl_init();
static void create_rgb_surface();
static void update_rgb_surface(int index);
static void update_rgb_pixels(const void *start);
static void *change_rgb_pixels();
static void save_image();

static void
sdl_init()
{
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTTHREAD) < 0) {
    perror("SDL_Init");
    exit(EXIT_FAILURE);
  }
  SDL_WM_SetCaption("Simple WebCam", NULL);
  atexit(SDL_Quit);
}

static void
create_rgb_surface()
{
  screen.rgb.rmask = 0x000000ff;
  screen.rgb.gmask = 0x0000ff00;
  screen.rgb.bmask = 0x00ff0000;
  screen.rgb.amask = 0xff000000;
  screen.rgb.width = screen.width;
  screen.rgb.height = screen.height;
  screen.rgb.bpp = screen.bpp;
  screen.rgb.pitch = screen.width * 4;
  screen.rgb.pixels_num = screen.width * screen.height * 4;
  screen.rgb.pixels = (unsigned char *)malloc(screen.rgb.pixels_num);
  memset(screen.rgb.pixels, 0, screen.rgb.pixels_num);
  screen.rgb.surface = SDL_CreateRGBSurfaceFrom(screen.rgb.pixels,
						screen.rgb.width,
						screen.rgb.height,
						screen.rgb.bpp,
						screen.rgb.pitch,
						screen.rgb.rmask,
						screen.rgb.gmask,
						screen.rgb.bmask,
						screen.rgb.amask);
}

static void
update_rgb_surface(int index)
{
  update_rgb_pixels(video.buffer.buf[index].start);
  SDL_BlitSurface(screen.rgb.surface, NULL, screen.display, NULL);
  SDL_Flip(screen.display);
}

static void
update_rgb_pixels(const void *start)
{
  unsigned char *data = (unsigned char *)start;
  unsigned char *pixels = screen.rgb.pixels;
  int width = screen.rgb.width;
  int height = screen.rgb.height;
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
      pixels[p4] = r;
      pixels[p4 + 1] = g;
      pixels[p4 + 2] = b;
      pixels[p4 + 3] = 255;
    }
  }
}

static void *
change_rgb_pixels()
{
/* pixels的RGBA转化成data的ARGB */
/* PC上DATA32的ARGB */
/* A保存在start + 24 */
/* R保存在start + 16 */
/* G保存在start + 8 */
/* B保存在start + 0 */
/* SDL和imlib2的鸿沟啊... */
  unsigned char *pixels = screen.rgb.pixels;
  unsigned char *data;
  int width = screen.rgb.width;
  int height = screen.rgb.height;
  int i;

  data = (unsigned char *)malloc(width * height * 4);
  for (i = 0; i < width * height * 4; i += 4) {
    data[i] = pixels[i + 2];
    data[i + 1] = pixels[i + 1];
    data[i + 2] = pixels[i];
    data[i + 3] = pixels[i + 3];
  }
  return (void *)data;
}

static void
save_image(char *filename)
{
  Imlib_Image img;
  int width = screen.rgb.width;
  int height = screen.rgb.height;
  DATA32 *data;
  char *suffix;

  if (filename == NULL) {
    return;
  }
  suffix = strrchr(filename, '.');
  if (suffix) {
    data = (DATA32 *)change_rgb_pixels();
    img = imlib_create_image_using_data(width, height, data);
    if (img) {
      imlib_context_set_image(img);
      imlib_image_set_format(suffix + 1);
      imlib_save_image(filename);
      imlib_free_image();
    }
    free(data);
  }
  else {
    fprintf(stderr, "wrong file name: %s\n", filename);
    exit(EXIT_FAILURE);
  }
}

void
screen_init()
{
  screen.width = opt.width;
  screen.height = opt.height;
  screen.bpp = 32;
  screen.running = 1;
  screen.display = SDL_SetVideoMode(screen.width,
				    screen.height,
				    screen.bpp,
				    SDL_SWSURFACE | SDL_DOUBLEBUF);
  if (screen.display == NULL) {
    perror("SDL_SetVideoMode");
    exit(EXIT_FAILURE);
  }
  sdl_init();
  create_rgb_surface();
}

void
screen_quit()
{
  SDL_FreeSurface(screen.display);
  SDL_FreeSurface(screen.rgb.surface);
  free(screen.rgb.pixels);
  SDL_Quit();
}

void
screen_mainloop()
{
  int i;
  char *filename;

  for (i = 0; screen.running &&
	 i <= video.buffer.req.count; i++) {
    if (i == video.buffer.req.count) {
      i = 0;
    }
    buffer_dequeue(i);
    update_rgb_surface(i);
    if (SDL_PollEvent(&screen.event)) {
      switch (screen.event.type) {
      case SDL_KEYDOWN:
	switch (screen.event.key.keysym.sym) {
	case SDLK_q:
	  puts("bye");
	  screen.running = 0;
	  break;
	case SDLK_s:
	  filename = get_file_name("swc", "jpeg");
	  save_image(filename);
	  printf("%s saved.\n", filename);
	  free(filename);
	  break;
	default:
	  break;
	}
	break;
      case SDL_QUIT:
	screen.running = 0;
	break;
      default:
	break;
      }
    }
    buffer_enqueue(i);
  }
}
