#include <stdlib.h>
#include "opt.h"
#include "video.h"
#include "screen.h"
#include "image.h"

extern struct options opt;

int
main(int argc, char *argv[])
{
  options_init();
  options_deal(argc, argv);
  video_init();			/* 初始化video4linux2 */
  if (opt.image) {
    image_init();
    image_get();
    image_quit();
  }
  else {
    screen_init();		/* 初始化窗口 */
    screen_mainloop();
    screen_quit();
  }
  video_quit();
  exit(EXIT_SUCCESS);
}
