## Makefile.am for tenmado/happy-L
## process this file with automake --foreign to generate Makefile.in
## automake version 1.11 is required
# $Id: Makefile.am,v 1.56 2012/01/28 12:57:19 oohara Exp $

SUBDIRS = default image

noinst_LIBRARIES = libhappy-L.a

libhappy_L_a_SOURCES = balloon.c \
  balloon.h \
  close-system.c \
  close-system.h \
  disconnection.c \
  disconnection.h \
  ending.c \
  ending.h \
  explosion.c \
  explosion.h \
  fud.c \
  fud.h \
  high-score.c \
  high-score.h \
  image.c \
  image.h \
  intuition.c \
  intuition.h \
  laser.c \
  laser.h \
  last-boss.c \
  last-boss.h \
  message.c \
  message.h \
  midnight.c \
  midnight.h \
  normal-enemy.c \
  normal-enemy.h \
  normal-shot.c \
  normal-shot.h \
  overrun.c \
  overrun.h \
  player-shot.c \
  player-shot.h \
  player.c \
  player.h \
  result.c \
  result.h \
  rotate-laser.c \
  rotate-laser.h \
  scheduler.c \
  scheduler.h \
  solution.c \
  solution.h \
  stage-1.c \
  stage-1.h \
  stage-2.c \
  stage-2.h \
  stage-3.c \
  stage-3.h \
  stage-4.c \
  stage-4.h \
  stage-5.c \
  stage-5.h \
  stage-clear.c \
  stage-clear.h \
  title.c \
  title.h \
  tutorial.c \
  tutorial.h \
  wall.c \
  wall.h

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

AM_CPPFLAGS = -DDATADIR=\"$(datadir)\" \
  -DLOCALSTATEDIR=\"$(localstatedir)\" \
  -I$(top_srcdir) \
  -I$(top_srcdir)/libbac \
  -I$(top_srcdir)/libtenm

# I don't know why libhappy_L_a_LIBADD = $(top_srcdir)/libbac/libbac.a \
# $(top_srcdir)/libtenm/libtenm.a doesn't work

# install-exec is the right thing for the high score database
# because $(localstatedir) can't be shared
install-exec-local:
	$(mkinstalldirs) $(DESTDIR)$(localstatedir)/games/tenmado/

uninstall-local:
	-rm -f $(DESTDIR)$(localstatedir)/games/tenmado/*
	-rmdir $(DESTDIR)$(localstatedir)/games/tenmado/
