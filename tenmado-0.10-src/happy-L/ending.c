/* $Id: ending.c,v 1.9 2012/01/29 04:04:15 oohara Exp $ */

#include <stdio.h>
/* strlen */
#include <string.h>

#include "tenm_table.h"
#include "const.h"
#include "message.h"
#include "ship.h"
#include "score.h"

#include "ending.h"

int
ending(int t)
{
  int status = SCHEDULER_SUCCESS;
  char temp[32];

  /* sanity check */
  if (t < 0)
  {
    fprintf(stderr, "ending: strange t (%d)\n", t);
    return SCHEDULER_ERROR;
  }

  if (t == 30)
  {
    if (tenm_table_add(message_new(190.0, 240.0,
                                   0.0, 0.0, 200, "all cleared", 11)) < 0)
    {
      fprintf(stderr, "ending: cannot create message (line 1) "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }

    sprintf(temp, "ship bonus: %8d", get_ship() * 30000);
    if (tenm_table_add(message_new(145.0, 270.0,
                                   0.0, 0.0, 200, temp,
                                   (int) strlen(temp))) < 0)
    {
      fprintf(stderr, "ending: cannot create message (line 2) "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
    add_score(get_ship() * 30000);
  }

  if (t == 240)
    return SCHEDULER_NEXT_STAGE;

  return status;
}
