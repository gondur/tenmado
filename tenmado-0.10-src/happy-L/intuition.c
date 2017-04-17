/* $Id: intuition.c,v 1.40 2003/01/15 01:47:44 oohara Exp $ */

#include <stdio.h>
/* malloc, rand */
#include <stdlib.h>
/* strcmp */
#include <string.h>

#include "tenm_primitive.h"
#include "tenm_object.h"
#include "const.h"
#include "tenm_table.h"
#include "score.h"
#include "tenm_graphic.h"
#include "hit-status.h"
#include "explosion.h"
#include "chain.h"
#include "util.h"
#include "laser.h"
#include "tenm_math.h"
#include "image.h"

#include "intuition.h"

#define INTUITION_HIT_POINT 750

static int intuition_move(tenm_object *my, double turn_per_frame);
static int intuition_hit(tenm_object *my, tenm_object *your);
static int intuition_signal(tenm_object *my, int n);
static int intuition_act(tenm_object *my, const tenm_object *player);
static int intuition_draw(tenm_object *my, int priority);

static tenm_object *wave_shot_new(double x, double y,
                                  double target_x, double target_y);
static int wave_shot_move(tenm_object *my, double turn_per_frame);
static int wave_shot_hit(tenm_object *my, tenm_object *your);
static int wave_shot_act(tenm_object *my, const tenm_object *player);
static int wave_shot_draw(tenm_object *my, int priority);

tenm_object *
intuition_new(int parent_index)
{
  tenm_primitive **p = NULL;
  tenm_object *new = NULL;
  int *count = NULL;
  double *count_d = NULL;
  double x = ((double) WINDOW_WIDTH) / 2.0;
  double y = -42.3;

  p = (tenm_primitive **) malloc(sizeof(tenm_primitive *) * 1);
  if (p == NULL)
  {
    fprintf(stderr, "intuition_new: malloc(p) failed\n");
    return NULL;
  }

  p[0] = (tenm_primitive *) tenm_polygon_new(3,
                                             x, y - 43.3,
                                             x + 50.0, y + 43.3,
                                             x - 50.0, y + 43.3);
  if (p[0] == NULL)
  {
    fprintf(stderr, "intuition_new: cannot set p[0]\n");
    free(p);
    return NULL;
  }

  count = (int *) malloc(sizeof(int) * 4);
  if (count == NULL)
  {
    fprintf(stderr, "intuition_new: malloc(count) failed\n");
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }

  count_d = (double *) malloc(sizeof(double) * 2);
  if (count_d == NULL)
  {
    fprintf(stderr, "intuition_new: malloc(count_d) failed\n");
    (p[0])->delete(p[0]);
    free(p);
    if (count != NULL)
      free(count);
    return NULL;
  }

  /* list of count
   * [0] table index of parent
   * [1] total timer
   * [2] shoot timer
   * [3] "damaged" flag
   */
  /* list of count_d
   * [0] speed x
   * [1] speed y
   */

  count[0] = parent_index;
  count[1] = 0;
  count[2] = 0;
  count[3] = 0;

  count_d[0] = 0.0;
  count_d[1] = 10.0;

  new = tenm_object_new("Intuition", ATTR_ENEMY, ATTR_PLAYER_SHOT,
                        INTUITION_HIT_POINT, x, y,
                        4, count, 2, count_d, 1, p, 
                        (int (*)(tenm_object *, double)) intuition_move,
                        (int (*)(tenm_object *, tenm_object *)) intuition_hit,
                        (int (*)(tenm_object *, const tenm_object *))
                        intuition_act,
                        (int (*)(tenm_object *, int)) intuition_draw);
  if (new == NULL)
  {
    fprintf(stderr, "intuition_new: tenm_object_new failed\n");
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
intuition_move(tenm_object *my, double turn_per_frame)
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

  if (!in_window_object(my))
  {
    tenm_table_apply(my->count[0],
                     (int (*)(tenm_object *, int)) intuition_signal,
                     0);
    return 1;
  }

  return 0;
}

static int
intuition_hit(tenm_object *my, tenm_object *your)
{
  /* sanity check */
  if (my == NULL)
    return 0;
  if (your == NULL)
    return 0;

  if (!(your->attr & ATTR_PLAYER_SHOT))
    return 0;

  my->hit_point -= add_damage_score(my->hit_point, your->hit_point);
  my->count[3] = 1;
  set_hit_status(my->name, INTUITION_HIT_POINT, my->hit_point);
  if (my->hit_point <= 0)
  {
    tenm_table_add(explosion_new(my->x, my->y, 0.0, 0.0,
                                 1, 3000, COLOR_PURPLE, 6.0, 12));
    tenm_table_add(explosion_new(my->x, my->y, 0.0, 0.0,
                                 2, 500, COLOR_PURPLE, 4.0, 12));
    tenm_table_apply(my->count[0],
                     (int (*)(tenm_object *, int)) intuition_signal,
                     0);
    add_killed(COLOR_PURPLE);
    add_score(500);
    return 1;
  }

  return 0;
}

static int
intuition_signal(tenm_object *my, int n)
{
  /* sanity check */
  if (my == NULL)
    return 0;
  if (strcmp(my->name, "stage 2 more 1") != 0)
    return 0;

  my->count[3] = 1;

  return 0;
}

static int
intuition_act(tenm_object *my, const tenm_object *player)
{
  double speed;
  int theta;

  /* sanity check */
  if (my == NULL)
    return 0;
  if (player == NULL)
    return 0;

  /* speed change */
  (my->count[1])++;
  if (my->count[1] == 13)
  {
    my->count_d[0] = 0.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 145)
  {
    my->count_d[0] = -10.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 155)
  {
    my->count_d[0] = 0.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 345)
  {
    my->count_d[0] = 10.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 355)
  {
    my->count_d[0] = 0.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 405)
  {
    my->count_d[0] = 10.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 415)
  {
    my->count_d[0] = 0.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 590)
  {
    my->count_d[0] = -10.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 600)
  {
    my->count_d[0] = 0.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 645)
  {
    my->count_d[0] = -10.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 655)
  {
    my->count_d[0] = 0.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 830)
  {
    my->count_d[0] = 10.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 840)
  {
    my->count_d[0] = 0.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 885)
  {
    my->count_d[0] = 10.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 895)
  {
    my->count_d[0] = 0.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 1065)
  {
    my->count_d[0] = -10.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 1075)
  {
    my->count_d[0] = 0.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 1310)
  {
    my->count_d[0] = 10.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 1322)
  {
    my->count_d[0] = 0.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 1450)
  {
    my->count_d[0] = -10.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 1462)
  {
    my->count_d[0] = 0.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 1555)
  {
    my->count_d[0] = -10.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 1567)
  {
    my->count_d[0] = 0.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 1690)
  {
    my->count_d[0] = 10.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 1702)
  {
    my->count_d[0] = 0.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 1828)
  {
    my->count_d[0] = 10.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 1840)
  {
    my->count_d[0] = 0.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 1862)
  {
    my->count_d[0] = -10.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 1886)
  {
    my->count_d[0] = 0.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 1926)
  {
    my->count_d[0] = 10.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 1950)
  {
    my->count_d[0] = 0.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 1986)
  {
    my->count_d[0] = -10.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 2010)
  {
    my->count_d[0] = 0.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 2046)
  {
    my->count_d[0] = 10.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 2070)
  {
    my->count_d[0] = 0.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 2100)
  {
    my->count_d[0] = -10.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 2112)
  {
    my->count_d[0] = 0.0;
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] == 2300)
  {
    my->count_d[0] = 0.0;
    my->count_d[1] = -10.0;
  }

  /* shoot */
  (my->count[2])++;
  if ((my->count[2] >= 40) && (my->count[2] <= 70) && (my->count[2] % 5 == 0))
  {
    speed = ((double) my->count[2]) / 5.0 - 4.0;
    theta = my->count[2] * 2 - 20;
    tenm_table_add(laser_angle_new(my->x, my->y, speed, theta,
                                   30.0, COLOR_PURPLE, 1000));
  }
  else if ((my->count[2] >= 115) && (my->count[2] <= 145)
           && (my->count[2] % 5 == 0))
  {
    speed = ((double) (my->count[2] - 75)) / 5.0 - 4.0;
    theta = 350 - my->count[2] * 2;
    tenm_table_add(laser_angle_new(my->x, my->y, speed, theta,
                                   30.0, COLOR_PURPLE, 1000));
  }

  if (my->count[2] % 15 == 0)
    tenm_table_add(wave_shot_new(my->x, my->y, player->x, player->y));

  if (my->count[2] >= 150)
    my->count[2] = 0;

  return 0;
}

static int
intuition_draw(tenm_object *my, int priority)
{
  int n = 0;
  tenm_color color;

  /* sanity check */
  if (my == NULL)
    return 0;
  if (priority != 0)
    return 0;

  if (my->count[3] == 0)
    n = 0;
  else
    n = 1;

  if (intuition_blit((int) (my->x), (int) (my->y), n)  != 0)
  {
    fprintf(stderr, "intuition_draw: intuition_blit failed\n");
    my->count[3] = 0;
    return 1;
  }

  my->count[3] = 0;
  return 0;
  /* the below may be useful for debugging */

  color = tenm_map_color(78, 59, 156);
  if (tenm_draw_mass(my->mass, color) != 0)
  {
    fprintf(stderr, "intuition_draw: tenm_draw_mass failed\n");
    return 1;
  }
  return 0;
}

static tenm_object *
wave_shot_new(double x, double y, double target_x, double target_y)
{
  tenm_primitive **p = NULL;
  tenm_object *new = NULL;
  int *count = NULL;
  double *count_d = NULL;
  double dx;
  double dy;
  double temp;
  int temp_theta;

  dx = target_x - x;
  dy = target_y - y;
  temp = tenm_sqrt((int) (dx * dx + dy * dy));
  if (temp <= 0.0)
  {
    /* shoot at a random direction */
    temp_theta = rand() % 360;
    dx = tenm_cos(temp_theta);
    dy = tenm_sin(temp_theta);
    temp = 1.0;
  }

  p = (tenm_primitive **) malloc(sizeof(tenm_primitive *) * 1);
  if (p == NULL)
  {
    fprintf(stderr, "wave_shot_new: malloc(p) failed\n");
    return NULL;
  }

  p[0] = (tenm_primitive *) tenm_circle_new(x, y, 5.0);
  if (p[0] == NULL)
  {
    fprintf(stderr, "wave_shot_new: cannot set p[0]\n");
    free(p);
    return NULL;
  }

  count = (int *) malloc(sizeof(int) * 2);
  if (count == NULL)
  {
    fprintf(stderr, "wave_shot_new: malloc(count) failed\n");
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }
  count_d = (double *) malloc(sizeof(double) * 2);
  if (count_d == NULL)
  {
    fprintf(stderr, "wave_shot_new: malloc(count_d) failed\n");
    free(count);
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }

  /* list of count
   * [0] color (just for delete_enemy_shot)
   * [1] ddx timer
   */
  /* list of count_d
   * [0] speed x
   * [1] speed y
   */

  count[0] = COLOR_PURPLE;
  if (target_x >= x)
    count[1] = 16;
  else
    count[1] = 48;

  count_d[0] = 6.0 * dx / temp;
  count_d[1] = 6.0 * dy / temp;

  new = tenm_object_new("wave shot", ATTR_ENEMY_SHOT, ATTR_OPAQUE,
                        1, x, y,
                        2, count, 2, count_d, 1, p, 
                        (int (*)(tenm_object *, double)) wave_shot_move,
                        (int (*)(tenm_object *, tenm_object *))
                        wave_shot_hit,
                        (int (*)(tenm_object *, const tenm_object *))
                        wave_shot_act,
                        (int (*)(tenm_object *, int)) wave_shot_draw);
  if (new == NULL)
  {
    fprintf(stderr, "wave_shot_new: tenm_object_new failed\n");
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
wave_shot_move(tenm_object *my, double turn_per_frame)
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

  if (!in_window_object(my))
    return 1;
  return 0;
}

static int
wave_shot_hit(tenm_object *my, tenm_object *your)
{
  /* sanity check */
  if (my == NULL)
    return 0;
  if (your == NULL)
    return 0;

  if (your->attr & ATTR_OPAQUE)
    return 1;

  return 0;
}

static int
wave_shot_act(tenm_object *my, const tenm_object *player)
{
  /* sanity check */
  if (my == NULL)
    return 0;

  (my->count[1])++;
  if (my->count[1] <= 32)
    my->count_d[0] += 0.5;
  else
    my->count_d[0] -= 0.5;

  if (my->count[1] >= 64)
    my->count[1] = 0;

  return 0;
}

static int
wave_shot_draw(tenm_object *my, int priority)
{
  tenm_color color;

  /* sanity check */
  if (my == NULL)
    return 0;
  if (priority != 1)
    return 0;

  color = tenm_map_color(127, 0, 255);

  if (tenm_draw_circle((int) (my->x), (int) (my->y), 5, 2, color) != 0)
    return 1;

  /* may be useful for debugging */
  /*
  if (tenm_draw_primitive(my->mass->p[0], color) != 0)
    return 1;
  */

  return 0;
}
