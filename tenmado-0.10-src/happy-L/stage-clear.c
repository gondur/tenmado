/* $Id: stage-clear.c,v 1.10 2003/01/12 02:00:11 oohara Exp $ */

#include <stdio.h>
/* malloc */
#include <stdlib.h>
/* strlen */
#include <string.h>

#include "tenm_object.h"
#include "scheduler.h"
#include "util.h"
#include "ship.h"
#include "score.h"

#include "stage-clear.h"

static int stage_clear_act(tenm_object *my, const tenm_object *player);
static int stage_clear_draw(tenm_object *my, int priority);

tenm_object *
stage_clear_new(int stage)
{
  tenm_object *new = NULL;
  int *count = NULL;

  count = (int *) malloc(sizeof(int) * 2);
  if (count == NULL)
  {
    fprintf(stderr, "stage_clear_new: malloc(count) failed\n");
    return NULL;
  }

  /* list of count
   * [0] timer
   * [1] stage
   */
  count[0] = 0;
  count[1] = stage;

  new = tenm_object_new("stage clear message", 0, 0,
                        0, 0.0, 0.0,
                        2, count, 0, NULL, 0, NULL, 
                        (int (*)(tenm_object *, double)) NULL,
                        (int (*)(tenm_object *, tenm_object *)) NULL,
                        (int (*)(tenm_object *, const tenm_object *))
                        stage_clear_act,
                        (int (*)(tenm_object *, int)) stage_clear_draw);
  if (new == NULL)
  {
    fprintf(stderr, "stage_clear_new: tenm_object_new failed\n");
    free(count);
    return NULL;
  }

  return new;
}

static int
stage_clear_act(tenm_object *my, const tenm_object *player)
{
  /* sanity check */
  if (my == NULL)
    return 0;
  (my->count[0])++;

  if (my->count[0] >= 200)
  {
    /* you cannot clear the stage if you have no ship */
    if (get_ship() >= 0)
      set_stage_cleared(1);

    return 1;
  }
  return 0;
}

static int
stage_clear_draw(tenm_object *my, int priority)
{
  int status = 0;
  char temp[32];
  int x;

  /* sanity check */
  if (my == NULL)
    return 0;
  if (priority != 1)
    return 0;

  /* you cannot clear the stage if you have no ship */
  if (get_ship() >= 0)
  {  
    if ((my->count[1] >= 1) && (my->count[1] <= 4))
    {
      sprintf(temp, "stage %d cleared", my->count[1]);
      x = 180;
    }
    else
    {
      sprintf(temp, "final stage cleared");
      x = 160;
    }

    if (draw_string(x, 240, temp, (int) strlen(temp)) != 0)
    {
      fprintf(stderr, "stage_clear_draw: draw_string (line 1) failed\n");
      status = 1;
    }
  }

  sprintf(temp, "stage score: %8d", get_stage_score(my->count[1]));
  if (draw_string(150, 270, temp, (int) strlen(temp)) != 0)
  {
    fprintf(stderr, "stage_clear_draw: draw_string (line 2) failed\n");
    status = 1;
  }

  return status;
}
