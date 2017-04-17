/* $Id: close-system.c,v 1.101 2012/01/29 04:02:03 oohara Exp $ */

/* the close system
 * I heard it is a alchemy term (or is it "the close principle"?)
 */

#include <stdio.h>
/* malloc */
#include <stdlib.h>

#include "tenm_primitive.h"
#include "tenm_object.h"
#include "const.h"
#include "tenm_table.h"
#include "tenm_graphic.h"
#include "util.h"
#include "laser.h"
#include "normal-enemy.h"

#include "close-system.h"

static double close_system_scroll_total(int t);
static double close_system_scroll_speed(int t);

static int close_system_wall_move(tenm_object *my, double turn_per_frame);
static int close_system_wall_act(tenm_object *my, const tenm_object *player);
static int close_system_wall_draw(tenm_object *my, int priority);

static double
close_system_scroll_total(int t)
{
  double total;
  if (t < 75)
    total = 3.0 * ((double) t);
  else if (t < 2770)
    total = 225.0 + 0.1 * ((double) (t - 75));
  else if (t < 3800)
    total = 494.5;
  else if (t < 5075)
    total = 494.5 + 0.1 * ((double) (t - 3800));
  else
    total = 622.0 + 3.0 * ((double) (t - 5075));

  return total;
}

static double
close_system_scroll_speed(int t)
{
  return close_system_scroll_total(t + 1) - close_system_scroll_total(t);
}

tenm_object *
close_system_wall_new(int n)
{
  tenm_primitive **p = NULL;
  tenm_object *new = NULL;
  int *count = NULL;
  double *count_d = NULL;
  double x;
  double y;

  /* sanity check */
  if ((n < 0) || (n > 3))
  {
    fprintf(stderr, "close_system_wall_new: strange n (%d)\n", n);
    return NULL;
  }

  if (n == 0)
  {
    x = 324.0;
    y = -29.0;
  }
  else if (n == 1)
  {
    x = 36.0;
    y = -149.0;
  }
  else if (n == 2)
  {
    x = 156.0;
    y = -437.0;
  }
  else
  {
    x = 444.0;
    y = -317.0;
  }
  
  p = (tenm_primitive **) malloc(sizeof(tenm_primitive *) * 1);
  if (p == NULL)
  {
    fprintf(stderr, "close_system_wall_new: malloc(p) failed\n");
    return NULL;
  }

  p[0] = (tenm_primitive *) tenm_polygon_new(4,
                                             x - 30.0, y - 30.0,
                                             x - 30.0, y + 30.0,
                                             x + 30.0, y + 30.0,
                                             x + 30.0, y - 30.0);
  if (p[0] == NULL)
  {
    fprintf(stderr, "close_system_wall_new: cannot set p[0]\n");
    free(p);    return NULL;
  }

  count = (int *) malloc(sizeof(int) * 7);
  if (count == NULL)
  {
    fprintf(stderr, "close_system_wall_new: malloc(count) failed\n");
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }

  count_d = (double *) malloc(sizeof(double) * 2);
  if (count_d == NULL)
  {
    fprintf(stderr, "close_system_wall_new: malloc(count_d) failed\n");
    (p[0])->delete(p[0]);
    free(p);
    if (count != NULL)
      free(count);
    return NULL;
  }

  /* list of count
   * [0] n
   * [1] total timer
   * [2] move direction
   * [3] move timer
   * [4] shoot theta
   * [5] shoot timer
   * [6] shoot cycle
   */
  /* list of count_d
   * [0] speed x
   * [1] speed y
   */

  count[0] = n;
  count[1] = 0;
  count[2] = (n + 3) % 4;
  count[3] = 60;
  switch (n)
  {
  case 0:
    count[4] = 90;
    break;
  case 1:
    count[4] = 315;
    break;
  case 2:
    count[4] = 270;
    break;
  case 3:
    count[4] = 135;
    break;
  default:
    fprintf(stderr, "close_system_wall_new: strange n when setting "
            "count[4] (%d)\n", n);
    if (count_d != NULL)
      free(count_d);
    if (count != NULL)
      free(count);
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
    break;
  }
  count[5] = 0;
  count[6] = 7;

  count_d[0] = 0.0;
  count_d[1] = 0.0;

  /* scroll */
  count_d[1] += close_system_scroll_speed(0);
  
  new = tenm_object_new("close system wall", ATTR_OBSTACLE | ATTR_OPAQUE, 0,
                        1, x, y,
                        7, count, 2, count_d, 1, p,
                        (int (*)(tenm_object *, double))
                        close_system_wall_move,
                        (int (*)(tenm_object *, tenm_object *)) NULL,
                        (int (*)(tenm_object *, const tenm_object *))
                        close_system_wall_act,
                        (int (*)(tenm_object *, int)) close_system_wall_draw);
  if (new == NULL)
  {
    fprintf(stderr, "close_system_wall_new: tenm_object_new failed\n");
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
close_system_wall_move(tenm_object *my, double turn_per_frame)
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
  tenm_move_mass(my->mass, dx_temp, dy_temp);

  if ((my->count[1] >= 5075) && (!in_window_object(my)))
    return 1;

  return 0;
}

static int
close_system_wall_act(tenm_object *my, const tenm_object *player)
{
  /* sanity check */
  if (my == NULL)
    return 0;

  (my->count[1])++;

  if ((my->count[1] >= 2800) && (my->count[1] < 3280))
  {
    /* move direction change */
    (my->count[3])++;
    if (my->count[3] >= 60)
    {
      my->count[3] = 0;
      (my->count[2])++;
      if (my->count[2] > 3)
        my->count[2] = 0;
    }

    switch (my->count[2])
    {
    case 0:
      my->count_d[0] = -4.8;
      my->count_d[1] = -2.0;
      break;
    case 1:
      my->count_d[0] = 2.0;
      my->count_d[1] = -4.8;
      break;
    case 2:
      my->count_d[0] = 4.8;
      my->count_d[1] = 2.0;
      break;
    case 3:
      my->count_d[0] = -2.0;
      my->count_d[1] = 4.8;
      break;
    default:
      fprintf(stderr, "close_system_wall_act: strange move direction (%d)\n",
              my->count[2]);
      break;
    }
  }
  else
  {
    my->count_d[0] = 0.0;
    my->count_d[1] = 0.0;
  }
  
  /* scroll */
  my->count_d[1] += close_system_scroll_speed(my->count[1]);

  /* shoot cycle change */
  if (my->count[1] == 2770)
  {
    my->count[6] = 1000;
  }
  else if (my->count[1] == 2800)
  {
    my->count[5] = 0;
    my->count[6] = 4;
  }
  else if (my->count[1] == 3280)
  {
    my->count[6] = 1000;
  }
  else if (my->count[1] == 3330)
  {
    my->count[5] = 0;
    my->count[6] = 3;
  }
  else if (my->count[1] == 3465)
  {
    my->count[5] = 0;
    my->count[6] = 7;
  }
  else if (my->count[1] == 3758)
  {
    my->count[5] = 0;
    my->count[6] = 1000;
  }
  else if (my->count[1] == 3800)
  {
    my->count[5] = 0;
    /* 4 is too difficult for me */
    my->count[6] = 5;
  }

  /* reset shoot direction */
  if ((my->count[1] == 2800) || (my->count[1] == 3800))
  {
    switch (my->count[0])
    {
    case 0:
      my->count[4] = 90;
      break;
    case 1:
      my->count[4] = 315;
      break;
    case 2:
      my->count[4] = 270;
      break;
    case 3:
      my->count[4] = 135;
      break;
    default:
      fprintf(stderr, "close_system_wall_act: strange count[0] (%d)\n",
              my->count[0]);
      break;
    }
  }
  else if (my->count[1] == 3330)
  {
    switch (my->count[0])
    {
    case 0:
      my->count[4] = 90;
      break;
    case 1:
      my->count[4] = 180;
      break;
    case 2:
      my->count[4] = 270;
      break;
    case 3:
      my->count[4] = 0;
      break;
    default:
      fprintf(stderr, "close_system_wall_act: strange count[0] (%d)\n",
              my->count[0]);
      break;
    }
  }

  /* shoot */
  (my->count[5])++;
  if (my->count[5] >= my->count[6])
  {
    my->count[5] = 0;
    tenm_table_add(laser_angle_new(my->x, my->y, 7.0, my->count[4],
                                   25.0, COLOR_GREEN, 1000));
    if ((my->count[1] < 3330) || (my->count[1] >= 3800))
    {
      if (my->count[0] % 2 == 0)
        my->count[4] += 10;
      else
        my->count[4] -= 10;
    }
    else if (my->count[1] < 3465)
    {
      my->count[4] += 3;
    }

    if (my->count[4] >= 360)
      my->count[4] -= 360;
    else if (my->count[4] < 0)
      my->count[4] += 360;
  }

  return 0;
}

static int
close_system_wall_draw(tenm_object *my, int priority)
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
    fprintf(stderr, "close_system_wall_draw: tenm_draw_mass failed\n");
    return 1;
  }
  return 0;
}

tenm_object *
close_system_ball_new(int t, int n, int direction, int time_shoot,
                      int color)
{
  double x;
  double y;
  double dx;
  double dy;

  if ((direction != CLOSE_SYSTEM_VERTICAL_FORWARD)
      && (direction != CLOSE_SYSTEM_VERTICAL_BACKWARD)
      && (direction != CLOSE_SYSTEM_HORIZONTAL_FORWARD)
      && (direction != CLOSE_SYSTEM_HORIZONTAL_BACKWARD))
  {
    fprintf(stderr, "close_system_ball_new: strange direction (%d)\n",
            direction);
    return NULL;
  }

  if ((direction == CLOSE_SYSTEM_VERTICAL_FORWARD)
      || (direction == CLOSE_SYSTEM_VERTICAL_BACKWARD))
  {
    x = (close_system_scroll_total(t) - 377.0) * 5.0 / 12.0 + 10.0;
    if (n == -2)
    {
      x += 50.0;
    }
    else if (n == -1)
    {
      x += 204.0;
    }
    else if (n == 0)
    {
      x += 300.0;
    }
    else if (n == 1)
    {
      x += 396.0;
    }
    else if (n == 2)
    {
      x += 550.0;
    }
    else
    {
      fprintf(stderr, "close_system_ball_new: strange n (%d)\n", n);
      return NULL;
    }
    y = -24.0;

    dx = -1.0;
    dy = 2.4;

    if (direction == CLOSE_SYSTEM_VERTICAL_BACKWARD)
    {
      x -= 285.0;
      y += 688.0;
      dx *= -1.0;
      dy *= -1.0;
    }
  }
  else
  {
    y = close_system_scroll_total(t) - 50.0;
    if (n == -2)
    {
      y -= 573.0;
    }
    else if (n == -1)
    {
      y -= 389.0;
    }
    else if (n == 0)
    {
      y -= 293.0;
    }
    else if (n == 1)
    {
      y -= 197.0;
    }
    else if (n == 2)
    {
      y -= 13.0;
    }
    else
    {
      fprintf(stderr, "close_system_ball_new: strange n (%d)\n", n);
      return NULL;
    }
    x = -24.0;

    dx = 2.4;
    dy = 1.0;

    if (direction == CLOSE_SYSTEM_HORIZONTAL_BACKWARD)
    {
      x += 528.0;
      y += 220.0;
      dx *= -1.0;
      dy *= -1.0;
    }
  }
  
  /* scroll */
  dy += close_system_scroll_speed(t);

  return normal_enemy_new(x, y,
                          NORMAL_BALL_LARGE, 0, color,
                          270, -1, -1, 1, 1,
                          /* move 0 */
                          1000, dx, dy, 0.0, 0.0,
                          0.0, 0.0, 0.0, 0.0, 0,
                          /* shoot 0 */
                          1000, time_shoot, 0, 50, 0);
}
