/* $Id: explosion.c,v 1.37 2003/08/22 05:18:12 oohara Exp $ */

#include <stdio.h>
/* malloc, rand */
#include <stdlib.h>

#include "tenm_graphic.h"
#include "tenm_math.h"
#include "tenm_object.h"
#include "const.h"

#include "explosion.h"

static int explosion_act(tenm_object *my, const tenm_object *player);
static int explosion_draw(tenm_object *my, int priority);

tenm_object *
explosion_new(double x, double y, double dx, double dy,
              int size_debris, int number_debris,
              int color, double speed_debris, int life)
{
  int i;
  int suffix;
  int theta;
  double temp_speed;
  tenm_object *new = NULL;
  int *count = NULL;
  double *count_d = NULL;

  /* sanity check */
  if (size_debris <= 0)
    return NULL;
  if (number_debris <= 0)
    return NULL;
  if (speed_debris <= 0.0)
    return NULL;
  if (life <= 0)
    return NULL;

  count = (int *) malloc(sizeof(int) * 5);
  if (count == NULL)
  {
    fprintf(stderr, "explosion_new: malloc(count) failed\n");
    return NULL;
  }
  count_d = (double *) malloc(sizeof(double) * (4 + 4 * number_debris));
  if (count_d == NULL)
  {
    fprintf(stderr, "explosion_new: malloc(count_d) failed\n");
    free(count);
    return NULL;
  }
  
  /* list of count
   * [0] size of debris
   * [1] number of debris
   * [2] color
   * [3] life
   * [4] time passed
   */
  count[0] = size_debris;
  count[1] = number_debris;
  count[2] = color;
  count[3] = life;
  count[4] = 0;

  /* list of count_d
   * [0] source x
   * [1] source y
   * [2] source dx
   * [3] source dy
   * [4--] debris (x, y, dx, dy) (relative to source)
   */
  count_d[0] = x;
  count_d[1] = y;
  count_d[2] = dx;
  count_d[3] = dy;
  for (i = 0; i < number_debris; i++)
  {
    suffix = i * 4 + 4;
    theta = rand() % 360;
    temp_speed = (double) (6 + rand() % 95) / 100.0;
    count_d[suffix + 0] = 0.0;
    count_d[suffix + 1] = 0.0;
    count_d[suffix + 2] = speed_debris * temp_speed * tenm_cos(theta);
    count_d[suffix + 3] = speed_debris * temp_speed * tenm_sin(theta);
  }

  new = tenm_object_new("explosion", 0, 0, 0, x, y,
                        5, count, 4 + 4 * number_debris, count_d,
                        0, NULL,
                        (int (*)(tenm_object *, double)) NULL,
                        NULL,
                        (int (*)(tenm_object *, const tenm_object *))
                        explosion_act,
                        (int (*)(tenm_object *, int)) explosion_draw);
  if (new == NULL)
  {
    fprintf(stderr, "explosion_new: tenm_object_new failed\n");
    if (count_d != NULL)
      free(count_d);
    if (count != NULL)
      free(count);
    return NULL;
  }

  return new;
}

static int
explosion_act(tenm_object *my, const tenm_object *player)
{
  int i;
  int suffix;

  /* sanity check */
  if (my == NULL)
    return 0;
  if (player == NULL)
    return 0;

  (my->count[4])++;
  if (my->count[4] >= my->count[3])
  {
    (my->count[1]) /= 2;
    my->count[4] = 0;
  }

  if (my->count[1] <= 0)
    return 1;

  /* explosion does not collide with anything, so let't move it here,
   * not in a separate move function
   */
  my->count_d[0] += my->count_d[2];
  my->count_d[1] += my->count_d[3];
  for (i = 0; i < my->count[1]; i++)
  {
    suffix = i * 4 + 4;
    my->count_d[suffix + 0] += my->count_d[suffix + 2];
    my->count_d[suffix + 1] += my->count_d[suffix + 3];
  }

  return 0;
}

static int
explosion_draw(tenm_object *my, int priority)
{
  int i;
  int suffix;
  int status = 0;
  tenm_color color;
  int temp;

  /* sanity check */
  if (my == NULL)
    return 0;
  if (priority != -1)
    return 0;
  if (my->count[0] <= 0)
    return 0;

  switch (my->count[2])
  {
  case COLOR_BLUE:
    color = tenm_map_color(59, 112, 156);
    break;
  case COLOR_PURPLE:
    color = tenm_map_color(78, 59, 156);
    break;
  case COLOR_RED:
    color = tenm_map_color(156, 59, 72);
    break;
  case COLOR_GREEN:
    color = tenm_map_color(59, 156, 71);
    break;
  default:
    color = tenm_map_color(0, 0, 0);
    break;
  }

  temp = my->count[0] / 2;
  for (i = 0; i < my->count[1]; i++)
  {
    suffix = i * 4 + 4;
    if (tenm_draw_rectangle(((int) (my->count_d[0] + my->count_d[suffix + 0]))
                            - temp,
                            ((int) (my->count_d[1] + my->count_d[suffix + 1]))
                            - temp,
                            my->count[0], my->count[0],
                            color) != 0)
      status = 1;
  }
  return status;
}
