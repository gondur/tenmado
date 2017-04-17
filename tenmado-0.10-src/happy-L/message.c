/* $Id: message.c,v 1.10 2003/01/03 22:22:44 oohara Exp $ */

#include <stdio.h>
/* malloc */
#include <stdlib.h>

#include "util.h"
#include "tenm_object.h"
#include "const.h"

#include "message.h"

static int message_move(tenm_object *my, double turn_per_frame);
static int message_act(tenm_object *my, const tenm_object *player);
static int message_draw(tenm_object *my, int priority);

tenm_object *
message_new(double x, double y, double dx, double dy,
            int life, const char *string, int length)
{
  int i;
  tenm_object *new = NULL;
  int *count = NULL;
  double *count_d = NULL;

  /* sanity check */
  if (life <= 0)
    return NULL;
  if (string == NULL)
    return NULL;
  if (length <= 0)
    return NULL;

  count = (int *) malloc(sizeof(int) * (length + 3));
  if (count == NULL)
  {
    fprintf(stderr, "message_new: malloc(count) failed\n");
    return NULL;
  }
  count_d = (double *) malloc(sizeof(double) * 2);
  if (count_d == NULL)
  {
    fprintf(stderr, "message_new: malloc(count_d) failed\n");
    free(count);
    return NULL;
  }

  /* list of count
   * [0] life
   * [1] length
   * [2 --] string
   */
  count[0] = life;
  count[1] = length;
  for (i = 0; i < length; i++)
    count[i + 2] = (int) string[i];
  count[length + 2] = '\0';

  /* list of count_d
   * [0] speed x
   * [1] speed y
   */
  count_d[0] = dx;
  count_d[1] = dy;

  new = tenm_object_new("message", 0, 0, 0, x, y,
                        length + 2, count, 2, count_d,
                        0, NULL,
                        (int (*)(tenm_object *, double)) message_move,
                        NULL,
                        (int (*)(tenm_object *, const tenm_object *))
                        message_act,
                        (int (*)(tenm_object *, int)) message_draw);
  if (new == NULL)
  {
    fprintf(stderr, "message_new: tenm_object_new failed\n");
    if (count_d != NULL)
      free(count_d);
    if (count != NULL)
      free(count);
    return NULL;
  }

  return new;
}

static int
message_move(tenm_object *my, double turn_per_frame)
{
  double dx_temp;
  double dy_temp;

  /* sanity check */
  if (my == NULL)
    return 0;
  if (turn_per_frame <= 0.5)
    return 0;

  dx_temp = my->count_d[0] / turn_per_frame;
  dy_temp = my->count_d[1] / turn_per_frame;
  my->x += dx_temp;
  my->y += dy_temp;

  if (my->x >= WINDOW_WIDTH)
    return 1;
  if (my->x + my->count[1] * 9 < 0)
    return 1;
  if (my->y >= WINDOW_HEIGHT)
    return 1;
  if (my->y + 18 < 0)
    return 1;
  return 0;
}

static int
message_act(tenm_object *my, const tenm_object *player)
{
  /* sanity check */
  if (my == NULL)
    return 0;

  (my->count[0])--;
  if (my->count[0] <= 0)
    return 1;
  return 0;
}

static int
message_draw(tenm_object *my, int priority)
{
  /* sanity check */
  if (my == NULL)
    return 0;
  if (priority != 1)
    return 0;

  return draw_string_int((int) (my->x), (int) (my->y),
                         (const int *) (my->count + 2), my->count[1]);
}
