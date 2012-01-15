#include "colorspace.h"

static int clamp(double x);

static int
clamp(double x)
{
  int r = x;
  if (r < 0)
    return 0;
  else if (r > 255)
    return 255;
  else
    return r;
}

void
yuv2rgb(unsigned char Y,
	unsigned char Cb,
	unsigned char Cr,
	int *ER,
	int *EG,
	int *EB)
{
  double y1, pb, pr, r, g, b;

  y1 = (255 / 219.0) * (Y - 16);
  pb = (255 / 224.0) * (Cb - 128);
  pr = (255 / 224.0) * (Cr - 128);
  r = 1.0 * y1 + 0 * pb + 1.402 * pr;
  g = 1.0 * y1 - 0.344 * pb - 0.714 * pr;
  b = 1.0 * y1 + 1.722 * pb + 0 * pr;
/* 用GDB调试了这么久终于将BUG找出来了:), 是v4l2的文档有问题 */
/* 不应该为clamp(r * 255) */
  *ER = clamp(r);
  *EG = clamp(g);
  *EB = clamp(b);
}
