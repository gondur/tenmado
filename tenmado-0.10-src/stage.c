/* $Id: stage.c,v 1.1 2002/07/10 11:08:47 oohara Exp $ */

#include <stdio.h>

#include "stage.h"

static int stage = 1;

void
set_stage(int n)
{
  stage = n;
}

int
get_stage(void)
{
  return stage;
}

/* return the new value of stage */
int
add_stage(int delta)
{
  if (stage + delta <= 0)
  {
    fprintf(stderr, "add_stage: trying to let stage negative, assuming "
            "1 instead\n");
    stage = 1;
  }
  else
  {
    stage += delta;
  }

  return stage;
}
