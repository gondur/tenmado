/* $Id: balloon.c,v 1.11 2002/06/22 09:44:42 oohara Exp $ */

#include <stdio.h>
/* malloc */
#include <stdlib.h>

#include "tenm_primitive.h"
#include "tenm_object.h"
#include "const.h"
#include "tenm_table.h"
#include "tenm_graphic.h"
#include "util.h"

#include "balloon.h"

static int balloon_move(tenm_object *my, double turn_per_frame);
static int balloon_act(tenm_object *my, const tenm_object *player);
static int balloon_draw(tenm_object *my, int priority);

tenm_object *
balloon_new(double x, double radius, double speed_y, double speed_r,
            int time_swell, int time_swell_initial, int time_wait)
{
  tenm_primitive **p = NULL;
  tenm_object *new = NULL;
  int *count = NULL;
  double *count_d = NULL;
  double y = 1.0 - radius;

  /* sanity check */
  if (radius <= 0.0)
    return NULL;
  if (time_swell <= 0)
    return NULL;
  if (time_wait < 0)
    return NULL;

  p = (tenm_primitive **) malloc(sizeof(tenm_primitive *) * 1);
  if (p == NULL)
  {
    fprintf(stderr, "balloon_new: malloc(p) failed\n");
    return NULL;
  }

  p[0] = (tenm_primitive *) tenm_circle_new(x, y, radius);
  if (p[0] == NULL)
  {
    fprintf(stderr, "balloon_new: cannot set p[0]\n");
    free(p);
    return NULL;
  }

  count = (int *) malloc(sizeof(int) * 3);
  if (count == NULL)
  {
    fprintf(stderr, "balloon_new: malloc(count) failed\n");
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }

  count_d = (double *) malloc(sizeof(double) * 3);
  if (count_d == NULL)
  {
    fprintf(stderr, "balloon_new: malloc(count_d) failed\n");
    (p[0])->delete(p[0]);
    free(p);
    if (count != NULL)
      free(count);
    return NULL;
  }

  /* list of count
   * [0] timer
   * [1] time_swell
   * [2] time_wait
   */
  /* list of count_d
   * [0] speed y
   * [1] current speed r
   * [2] speed r (for calculating count_d[1])
   */

  count[0] = time_swell_initial;
  count[1] = time_swell;
  count[2] = time_wait;

  count_d[0] = speed_y;
  count_d[1] = speed_r;
  count_d[2] = speed_r * (-1.0);

  new = tenm_object_new("balloon", ATTR_OBSTACLE | ATTR_OPAQUE, 0,
                        1, x, y,
                        3, count, 3, count_d, 1, p, 
                        (int (*)(tenm_object *, double)) balloon_move,
                        (int (*)(tenm_object *, tenm_object *)) NULL,
                        (int (*)(tenm_object *, const tenm_object *))
                        balloon_act,
                        (int (*)(tenm_object *, int)) balloon_draw);
  if (new == NULL)
  {
    fprintf(stderr, "balloon_new: tenm_object_new failed\n");
    if (count_d != NULL)
      free(count_d);
    if (count != NULL)
      free(count);
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }

  return new;
}

static int
balloon_move(tenm_object *my, double turn_per_frame)
{
  double dy_temp;
  double dr_temp;

  /* sanity check */
  if (my == NULL)
    return 0;
  if (turn_per_frame <= 0.5)
    return 0;

  dy_temp = my->count_d[0] / turn_per_frame;
  dr_temp = my->count_d[1] / turn_per_frame;
  my->y += dy_temp;
  tenm_move_mass(my->mass, 0.0, dy_temp);
  ((tenm_circle *) my->mass->p[0])->r += dr_temp;

  if (!in_window_object(my))
    return 1;

  return 0;
}

static int
balloon_act(tenm_object *my, const tenm_object *player)
{
  /* sanity check */
  if (my == NULL)
    return 0;

  (my->count[0])++;
  if (my->count[0] >= my->count[1] + my->count[2])
  {
    my->count[0] = 0;
    my->count_d[1] = my->count_d[2];
    my->count_d[2] *= -1.0;
  }
  else if (my->count[0] >= my->count[1])
  {
    my->count_d[1] = 0.0;
  }

  return 0;
}

static int
balloon_draw(tenm_object *my, int priority)
{
  tenm_color color;

  /* sanity check */
  if (my == NULL)
    return 0;
  if (priority != 0)
    return 0;

  color = tenm_map_color(59, 156, 71);
  if (tenm_draw_mass(my->mass, color) != 0)
  {
    fprintf(stderr, "balloon_draw: tenm_draw_mass failed\n");
    return 1;
  }
  return 0;
}
