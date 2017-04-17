## Makefile.am for tenmado
## process this file with automake --foreign to generate Makefile.in
## automake version 1.11 is required
# $Id: Makefile.am,v 1.69 2012/01/28 12:56:31 oohara Exp $

SUBDIRS = happy-L libbac libtenm save

bin_PROGRAMS = tenmado
tenmado_SOURCES = chain.c \
  chain.h \
  const.h \
  esc-ok.c \
  esc-ok.h \
  hit-status.c \
  hit-status.h \
  loop.c \
  loop.h \
  main.c \
  option.c \
  option.h \
  pause.c \
  pause.h \
  score.c \
  score.h \
  ship.c \
  ship.h \
  stage.c \
  stage.h \
  util.c \
  util.h

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
  -I$(top_srcdir)/happy-L \
  -I$(top_srcdir)/libbac \
  -I$(top_srcdir)/libtenm

# there is no AM_LDADD
tenmado_LDADD = $(top_srcdir)/happy-L/libhappy-L.a \
  $(top_srcdir)/libbac/libbac.a \
  $(top_srcdir)/libtenm/libtenm.a

man_MANS = tenmado.6

EXTRA_DIST = Artistic-2 \
  GPL-2 \
  LICENSE \
  icon.png \
  icon.xpm \
  tenmado.6 \
  README-ja-utf8 \
  README-ja-sjis

BUILT_SOURCES = README-ja-sjis

README-ja-sjis: README-ja-utf8
	perl -w -n -e 's/\n/\r\n/g; print;' < README-ja-utf8 | \
  iconv -f UTF-8 -t SJIS -o README-ja-sjis
