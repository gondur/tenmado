/* $Id: option.h,v 1.17 2003/01/04 03:06:46 oohara Exp $ */

#ifndef __TENMADO_OPTION_H__
#define __TENMADO_OPTION_H__

struct _option
{
  /* 0 -- 5 if set, -1 if unset */
  int dump;
  /* 1 if set, 0 if unset */
  int full_screen;
  /* 1 if set, 0 if unset */
  int help;
  /* 1 if set, 0 is unset */
  int slow;
  /* 1 -- 5 if set, 0 is unset */
  int stage;
  /* 0 -- 5 if set, -1 if unset */
  int undump;
  /* 1 if set, 0 if unset */
  int undump_default;
  /* 1 if set, 0 if unset */
  int version;
};
typedef struct _option option;

int set_option(int argc, char *argv[]);
const option *get_option(void);
void do_help(void);
void do_version(void);

#endif /* not __TENMADO_OPTION_H__ */
