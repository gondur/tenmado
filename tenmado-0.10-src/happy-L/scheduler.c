/* $Id: scheduler.c,v 1.72 2002/07/11 14:36:28 oohara Exp $ */

#include <stdio.h>

#include "const.h"
#include "stage.h"
#include "stage-1.h"
#include "stage-2.h"
#include "stage-3.h"
#include "stage-4.h"
#include "stage-5.h"
#include "ending.h"
#include "tutorial.h"

#include "scheduler.h"

static int stage_cleared = 0;

int
scheduler(int t)
{
  int stage = get_stage();

  /* sanity check */
  if (stage < 0)
    return SCHEDULER_SUCCESS;
  if (t < 0)
    return SCHEDULER_SUCCESS;

  if (stage_cleared != 0)
  {
    set_stage_cleared(0);
    return SCHEDULER_NEXT_STAGE;
  }

  if (stage == 1)
    return scheduler_1(t);
  else if (stage == 2)
    return scheduler_2(t);
  else if (stage == 3)
    return scheduler_3(t);
  else if (stage == 4)
    return scheduler_4(t);
  else if (stage == 5)
    return scheduler_5(t);
  else if (stage == 6)
    return ending(t);
  else if (stage == 7)
    return tutorial(t);

  fprintf(stderr, "scheduler: strange stage (%d) (t = %d)\n", stage, t);
  return SCHEDULER_ERROR;
}

void
set_stage_cleared(int n)
{
  stage_cleared = n;
}
