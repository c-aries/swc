#ifndef OPT_H
#define OPT_H

struct options {
  int verbose;
  int width;
  int height;
  int image;
};

void options_init();
void options_deal(int argc, char *argv[]);

#endif
