## Makefile.am for libtenm
## process this file with automake --foreign to generate Makefile.in
## automake version 1.11 is required
# $Id: Makefile.am,v 1.22 2012/01/28 12:59:14 oohara Exp $

noinst_LIBRARIES = libtenm.a

libtenm_a_SOURCES = tenm_collision.c \
  tenm_collision.h \
  tenm_graphic.c \
  tenm_graphic.h \
  tenm_input.c \
  tenm_input.h \
  tenm_math.c \
  tenm_math.h \
  tenm_object.c \
  tenm_object.h \
  tenm_primitive.c \
  tenm_primitive.h \
  tenm_sdl_init.c \
  tenm_sdl_init.h \
  tenm_table.c \
  tenm_table.h \
  tenm_timer.c \
  tenm_timer.h

#CC_WARNINGS = -Wall
# the overkill warnings
# I don't use -Wtraditional because I use string concatenation
# use -Wno-long-long because -pedantic implies -Wlong-long
CC_WARNINGS = -Wall -Wshadow -Wpointer-arith -Wcast-qual \
  -Wcast-align -Wwrite-strings -Wstrict-prototypes -Wmissing-prototypes \
  -Wmissing-declarations -Wnested-externs -Winline -pedantic -Wno-long-long

# let's hardcode SDL flags here
SDL_CFLAGS=`sdl-config --cflags`
# sdl-config --libs is added to LIBS in configure

# -g -O2 are in CFLAGS by default, don't use @CFLAGS@ here
AM_CFLAGS = $(SDL_CFLAGS) $(CC_WARNINGS)
