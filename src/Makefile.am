AM_CFLAGS = -Werror -Wall

bin_PROGRAMS = swc

swc_SOURCES = \
	colorspace.c colorspace.h \
	common.c common.h \
	image.c image.h \
	opt.c opt.h \
	screen.c screen.h \
	swc.c \
	video.c video.h

swc_CPPFLAGS = \
	$(SWC_CFLAGS)
swc_LDADD = \
	$(SWC_LIBS)

EXTRA_DIST = \
	test.c