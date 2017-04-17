/* $Id: hit-status.c,v 1.10 2003/01/12 02:08:41 oohara Exp $ */

#include <stdio.h>
/* strlen */
#include <string.h>

#include "const.h"
#include "util.h"

#include "hit-status.h"

static char hit_status[64];
static int time = 0;

void
clear_hit_status(void)
{
  time = 0;
}

void
set_hit_status(const char *name, int hit_point, int remaining)
{
  sprintf(hit_status, "%4d/%4d %.50s", remaining, hit_point, name);
  time = 30;
}

/* return 0 on success, 1 on error */
int
show_hit_status(void)
{
  if (time <= 0)
    return 0;
  time--;
  if (draw_string(10, 10, hit_status, (int) strlen(hit_status)) != 0)
  {
    fprintf(stderr, "show_hit_status: tenm_draw_string failed\n");
    return 1;
  }
  return 0;
}
