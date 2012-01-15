#ifndef COLORSPACE_H
#define COLORSPACE_H

void
yuv2rgb(unsigned char Y,
	unsigned char Cb,
	unsigned char Cr,
	int *ER,
	int *EG,
	int *EB);

#endif
