## Makefile.am for libbac
## process this file with automake --foreign to generate Makefile.in
## automake 1.11 or later is required
# $Id: Makefile.am,v 1.28 2012/01/28 12:59:02 oohara Exp $

noinst_LIBRARIES = libbac.a

# automake doesn't know that bison generates .h from .y,
# so we must add .h to SOURCES and BUILT_SOURCES
libbac_a_SOURCES= bac_array.c \
  bac_array.h \
  bac_dump.c \
  bac_dump.h \
  bac_entry.c \
  bac_entry.h \
  bac_entry_to_string.c \
  bac_entry_to_string.h \
  bac_lexical.h \
  bac_lexical.l \
  bac_load.c \
  bac_load.h \
  bac_parser.h \
  bac_parser.y \
  bac_parser_public.h \
  bac_save.c \
  bac_save.h \
  bac_string_to_array.c \
  bac_string_to_array.h \
  bac_truncate.c \
  bac_truncate.h

BUILT_SOURCES = bac_parser.h

AM_YFLAGS = -d
# -olex.yy.c is a workaround for automake braindamage
AM_LFLAGS = -Pbac_ -olex.yy.c -Cfe -8

#CC_WARNINGS = -Wall
# the overkill warnings
# I don't use -Wtraditional because I use string concatenation
CC_WARNINGS = -Wall -Wshadow -Wpointer-arith -Wcast-qual \
  -Wcast-align -Wwrite-strings -Wstrict-prototypes -Wmissing-prototypes \
  -Wmissing-declarations -Wnested-externs -Winline -pedantic

# -g -O2 are in CFLAGS by default, don't use @CFLAGS@ here
AM_CFLAGS = $(CC_WARNINGS)
