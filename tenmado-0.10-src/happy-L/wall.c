/* $Id: wall.c,v 1.4 2002/06/07 09:38:59 oohara Exp $ */

#include <stdio.h>
/* malloc */
#include <stdlib.h>

#include "const.h"
#include "tenm_object.h"
#include "tenm_graphic.h"
#include "util.h"
#include "tenm_primitive.h"

#include "wall.h"

static int wall_move(tenm_object *my, double turn_per_frame);
static int wall_draw(tenm_object *my, int priority);

tenm_object *
wall_new(double x, double width, double height, double speed)
{
  tenm_primitive **p = NULL;
  tenm_object *new = NULL;
  double *count_d = NULL;
  double w;
  double h;
  double y;

  /* sanity check */
  if (width <= 0.0)
    return NULL;
  if (height <= 0.0)
    return NULL;
  if (speed <= 0.0)
    return NULL;

  w = width / 2.0;
  h = height / 2.0;
  y = 1.0 - h;

  p = (tenm_primitive **) malloc(sizeof(tenm_primitive *) * 1);
  if (p == NULL)
  {
    fprintf(stderr, "wall_new: malloc(p) failed\n");
    return NULL;
  }

  p[0] = (tenm_primitive *) tenm_polygon_new(4,
                                             x - w, y - h, 
                                             x - w, y + h,
                                             x + w, y + h, 
                                             x + w, y - h);
  if (p[0] == NULL)
  {
    fprintf(stderr, "wall_new: cannot set p[0]\n");
    free(p);
    return NULL;
  }

  count_d = (double *) malloc(sizeof(double) * 1);
  if (count_d == NULL)
  {
    fprintf(stderr, "wall_new: malloc(count_d) failed\n");
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }

  /* list of count_d
   * [0] speed
   */
  count_d[0] = speed;

  new = tenm_object_new("wall", ATTR_OBSTACLE | ATTR_OPAQUE, 0,
                        1, x, y,
                        0, NULL, 1, count_d, 1, p, 
                        (int (*)(tenm_object *, double)) wall_move,
                        NULL,
                        (int (*)(tenm_object *, const tenm_object *))
                        NULL,
                        (int (*)(tenm_object *, int)) wall_draw);
  if (new == NULL)
  {
    fprintf(stderr, "wall_new: tenm_object_new failed\n");
    free(count_d);
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }

  return new;
}

static int
wall_move(tenm_object *my, double turn_per_frame)
{
  double dy_temp;

  /* sanity check */
  if (my == NULL)
    return 0;
  if (turn_per_frame <= 0.5)
    return 0;

  dy_temp = my->count_d[0] / turn_per_frame;
  my->y += dy_temp;
  tenm_move_mass(my->mass, 0.0, dy_temp);

  if (!in_window_object(my))
    return 1;
  return 0;
}

static int
wall_draw(tenm_object *my, int priority)
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
    fprintf(stderr, "wall_draw: tenm_draw_mass failed\n");
    return 1;
  }
  return 0;
}
