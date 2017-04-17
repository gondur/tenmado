/* $Id: rotate-laser.c,v 1.7 2002/08/04 16:07:24 oohara Exp $ */

#include <stdio.h>
/* malloc */
#include <stdlib.h>

#include "const.h"
#include "tenm_primitive.h"
#include "tenm_object.h"
#include "tenm_math.h"
#include "tenm_graphic.h"
#include "util.h"

#include "rotate-laser.h"

static int rotate_laser_move(tenm_object *my, double turn_per_frame);
static int rotate_laser_act(tenm_object *my, const tenm_object *player);
static int rotate_laser_draw(tenm_object *my, int priority);


tenm_object *
rotate_laser_new(double x, double y, double speed_x, double speed_y,
                 int theta, double speed_theta,
                 double length, int color, int life)
{
  tenm_primitive **p = NULL;
  tenm_object *new = NULL;
  int *count = NULL;
  double *count_d = NULL;
  int temp;

  /* sanity check */
  /* negative speed_theta is OK --- it means counterclockwise */
  if ((speed_theta > 0.1) && (speed_theta < 0.1))
    return NULL;
  if (length <= 0.0)
    return NULL;
  if (life <= 0)
    return NULL;

  if (speed_theta > 0.0)
    temp = 1;
  else
    temp = -1;

  p = (tenm_primitive **) malloc(sizeof(tenm_primitive *) * 1);
  if (p == NULL)
  {
    fprintf(stderr, "rotate_laser_new: malloc(p) failed\n");
    return NULL;
  }

  p[0] = (tenm_primitive *) tenm_polygon_new(3,
                                             x, y, 
                                             x + length * tenm_cos(theta),
                                             y + length * tenm_sin(theta),
                                             x +length * tenm_cos(theta+temp),
                                             y +length * tenm_sin(theta+temp));
  if (p[0] == NULL)
  {
    fprintf(stderr, "rotate_laser_new: cannot set p[0]\n");
    free(p);
    return NULL;
  }

  count = (int *) malloc(sizeof(int) * 3);
  if (count == NULL)
  {
    fprintf(stderr, "rotate_laser_new: malloc(count) failed\n");
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }
  count_d = (double *) malloc(sizeof(double) * 5);
  if (count_d == NULL)
  {
    fprintf(stderr, "rotate_laser_new: malloc(count_d) failed\n");
    free(count);
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }

  /* list of count
   * [0] color
   * [1] life
   * [2] theta
   */
  /* list of count_d
   * [0] speed x
   * [1] speed y
   * [2] length
   * [3] speed theta
   * [4] rotate timer
   */

  count[0] = color;
  count[1] = life;
  count[2] = theta;

  count_d[0] = speed_x;
  count_d[1] = speed_y;
  count_d[2] = length;
  count_d[3] = speed_theta;
  count_d[4] = 0.0;

  new = tenm_object_new("rotate laser", ATTR_ENEMY_SHOT, 0,
                        1, x, y,
                        3, count, 5, count_d, 1, p, 
                        (int (*)(tenm_object *, double)) rotate_laser_move,
                        NULL,
                        (int (*)(tenm_object *, const tenm_object *))
                        rotate_laser_act,
                        (int (*)(tenm_object *, int)) rotate_laser_draw);
  if (new == NULL)
  {
    fprintf(stderr, "rotate_laser_new: tenm_object_new failed\n");
    free(count_d);
    free(count);
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }

  return new;
}

static int
rotate_laser_move(tenm_object *my, double turn_per_frame)
{
  double x_temp;
  double y_temp;
  double dx_temp;
  double dy_temp;
  int dtheta_temp;

  /* sanity check */
  if (my == NULL)
    return 0;
  if (turn_per_frame <= 0.5)
    return 0;

  dx_temp = my->count_d[0] / turn_per_frame;
  dy_temp = my->count_d[1] / turn_per_frame;
  my->x += dx_temp;
  my->y += dy_temp;
  tenm_move_mass(my->mass, dx_temp, dy_temp);

  my->count_d[4] += my->count_d[3] / turn_per_frame;
  if ((my->count_d[4] >= 1.0) || (my->count_d[4] <= -1.0))
  {
    /* we need to rotate the laser */
    for (; my->count_d[4] >= 1.0; my->count_d[4] -= 1.0)
      (my->count[2])++;
    for (; my->count_d[4] <= -1.0; my->count_d[4] += 1.0)
      (my->count[2])--;
    x_temp = ((tenm_polygon *) my->mass->p[0])->v[0]->x;
    y_temp = ((tenm_polygon *) my->mass->p[0])->v[0]->y;
    if (my->count_d[3] >= 0.0)
      dtheta_temp = 1;
    else
      dtheta_temp = -1;
    ((tenm_polygon *) my->mass->p[0])->v[1]->x = x_temp
      + my->count_d[2] * tenm_cos(my->count[2]);
    ((tenm_polygon *) my->mass->p[0])->v[1]->y = y_temp
      + my->count_d[2] * tenm_sin(my->count[2]);
    ((tenm_polygon *) my->mass->p[0])->v[2]->x = x_temp
      + my->count_d[2] * tenm_cos(my->count[2] + dtheta_temp);
    ((tenm_polygon *) my->mass->p[0])->v[2]->y = y_temp
      + my->count_d[2] * tenm_sin(my->count[2] + dtheta_temp);
  }
  
  if (!in_window_object(my))
    return 1;
  return 0;
}

static int
rotate_laser_act(tenm_object *my, const tenm_object *player)
{
  (my->count[1])--;
  if (my->count[1] <= 0)
    return 1;
  return 0;
}

static int
rotate_laser_draw(tenm_object *my, int priority)
{
  int status = 0;
  tenm_color color;

  /* sanity check */
  if (my == NULL)
    return 0;
  if (priority != 1)
    return 0;

  switch (my->count[0])
  {
  case COLOR_BLUE:
    color = tenm_map_color(0, 63, 255);
    break;
  case COLOR_PURPLE:
    color = tenm_map_color(127, 0, 255);
    break;
  case COLOR_RED:
    color = tenm_map_color(255, 0, 50);
    break;
  case COLOR_GREEN:
    color = tenm_map_color(0, 255, 63);
    break;
  default:
    color = tenm_map_color(0, 0, 0);
    break;
  }

  if (tenm_draw_primitive(my->mass->p[0], color) != 0)
    status = 1;

  return status;
}
