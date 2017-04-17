/* $Id: solution.c,v 1.38 2003/01/12 17:03:16 oohara Exp $ */

#include <stdio.h>
/* malloc */
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

#include "solution.h"

#define SOLUTION_HIT_POINT 350

static int solution_move(tenm_object *my, double turn_per_frame);
static int solution_hit(tenm_object *my, tenm_object *your);
static int solution_signal(tenm_object *my, int n);
static int solution_act(tenm_object *my, const tenm_object *player);
static int solution_draw(tenm_object *my, int priority);

static tenm_object *solution_shot_new(double x, double y, int t, double dddx);
static int solution_shot_move(tenm_object *my, double turn_per_frame);
static int solution_shot_act(tenm_object *my, const tenm_object *player);
static int solution_shot_draw(tenm_object *my, int priority);

tenm_object *
solution_new(int parent_index)
{
  tenm_primitive **p = NULL;
  tenm_object *new = NULL;
  int *count = NULL;
  double *count_d = NULL;
  double x = ((double) WINDOW_WIDTH) + 47.0;
  double y = 100.0;

  p = (tenm_primitive **) malloc(sizeof(tenm_primitive *) * 1);
  if (p == NULL)
  {
    fprintf(stderr, "solution_new: malloc(p) failed\n");
    return NULL;
  }

  p[0] = (tenm_primitive *) tenm_polygon_new(4,
                                               x - 48.0, y - 36.0,
                                               x - 48.0, y + 36.0,
                                               x + 48.0, y + 36.0,
                                               x + 48.0, y - 36.0);
  if (p[0] == NULL)
  {
    fprintf(stderr, "solution_new: cannot set p[0]\n");
    free(p);
    return NULL;
  }

  count = (int *) malloc(sizeof(int) * 4);
  if (count == NULL)
  {
    fprintf(stderr, "solution_new: malloc(count) failed\n");
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }

  count_d = (double *) malloc(sizeof(double) * 2);
  if (count_d == NULL)
  {
    fprintf(stderr, "solution_new: malloc(count_d) failed\n");
    (p[0])->delete(p[0]);
    free(p);    if (count != NULL)
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
  count[1] = -20;
  count[2] = 0;
  count[3] = 0;

  count_d[0] = (((double) WINDOW_WIDTH) / 2.0 - x) / 20.0;
  count_d[1] = 0.0;

  new = tenm_object_new("Solution", ATTR_ENEMY, ATTR_PLAYER_SHOT,
                        SOLUTION_HIT_POINT, x, y,
                        4, count, 2, count_d, 1, p,
                        (int (*)(tenm_object *, double)) solution_move,
                        (int (*)(tenm_object *, tenm_object *)) solution_hit,
                        (int (*)(tenm_object *, const tenm_object *))
                        solution_act,
                        (int (*)(tenm_object *, int)) solution_draw);
  if (new == NULL)
  {
    fprintf(stderr, "solution_new: tenm_object_new failed\n");
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
solution_move(tenm_object *my, double turn_per_frame)
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
                     (int (*)(tenm_object *, int)) solution_signal,
                     0);
    return 1;
  }

  return 0;
}

static int
solution_hit(tenm_object *my, tenm_object *your)
{
  /* sanity check */
  if (my == NULL)
    return 0;
  if (your == NULL)
    return 0;

  if (!(your->attr & ATTR_PLAYER_SHOT))
    return 0;

  my->hit_point -= add_damage_score(my->hit_point, your->hit_point);
  set_hit_status(my->name, SOLUTION_HIT_POINT, my->hit_point);
  my->count[3] = 1;
  if (my->hit_point <= 0)
  {
    tenm_table_add(explosion_new(my->x, my->y, 0.0, 0.0,
                                 1, 3000, COLOR_PURPLE, 6.0, 12));
    tenm_table_add(explosion_new(my->x, my->y, 0.0, 0.0,
                                 2, 500, COLOR_PURPLE, 4.0, 12));
    tenm_table_apply(my->count[0],
                     (int (*)(tenm_object *, int)) solution_signal,
                     0);
    add_killed(COLOR_PURPLE);
    add_score(500);
    return 1;
  }

  return 0;
}

static int
solution_signal(tenm_object *my, int n)
{
  /* sanity check */
  if (my == NULL)
    return 0;
  if (strcmp(my->name, "stage 5 more 2") != 0)
    return 0;

  my->count[3] = 1;

  return 0;
}

static int
solution_act(tenm_object *my, const tenm_object *player)
{
  double dddx;

  /* sanity check */
  if (my == NULL)
    return 0;
  if (player == NULL)
    return 0;

  /* speed change */
  if (my->count[1] == -1)
  {
    my->count_d[0] = 0.0;
    my->count_d[1] = 1.0;
  }
  else if (my->count[1] > 500)
  {
    my->count_d[0] = -((((double) WINDOW_WIDTH) / 2.0 + 47.0) / 20.0);
    my->count_d[1] = 0.0;
  }
  else if (my->count[1] >= 0)
  {
    if (my->count[1] % 20 == 5)
    {
      my->count_d[0] = 1.5;
      my->count_d[1] = 0.0;
    }
    else if (my->count[1] % 20 == 15)
    {
      my->count_d[0] = -1.5;
      my->count_d[1] = 0.0;
    }
  }
   
  /* shoot */
  if ((my->count[1] >= 0) && (my->count[1] <= 500))
  {  
    if (my->count[1] % 40 == 0)
    { 
      tenm_table_add(laser_point_new(my->x - 36.0, my->y, 7.0,
                                     player->x,
                                     player->y,
                                     30.0, COLOR_PURPLE, 1000));
    }
    else if (my->count[1] % 40 == 20)
    {    
      tenm_table_add(laser_point_new(my->x + 36.0, my->y, 7.0,
                                     player->x,
                                     player->y,
                                     30.0, COLOR_PURPLE, 1000));
    }

    if (my->count[1] % 5 != 4)
    {
      dddx = my->count[1] * 0.0001;
      if (dddx > 0.015)
        dddx = 0.015;
      tenm_table_add(solution_shot_new(my->x, my->y, my->count[2], dddx));
    }
    
    (my->count[2])++;
    if (my->count[2] >= 24)
      my->count[2] = 0;
  }
  
  (my->count[1])++;

  return 0;
}

static int
solution_draw(tenm_object *my, int priority)
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
  if (solution_blit((int) (my->x), (int) (my->y), n) != 0)
  {
    fprintf(stderr, "solution_draw: solution_blit failed\n");
    my->count[3] = 0;
    return 1;
  }

  my->count[3] = 0;
  return 0;
  /* the below may be useful for debugging */

  color = tenm_map_color(78, 59, 156);
  if (tenm_draw_mass(my->mass, color) != 0)
  {
    fprintf(stderr, "solution_draw: tenm_draw_mass failed\n");
    return 1;
  }
  return 0;
}

static tenm_object *
solution_shot_new(double x, double y, int t, double dddx)
{
  tenm_primitive **p = NULL;
  tenm_object *new = NULL;
  int *count = NULL;
  double *count_d = NULL;

  p = (tenm_primitive **) malloc(sizeof(tenm_primitive *) * 1);
  if (p == NULL)
  {
    fprintf(stderr, "solution_shot_new: malloc(p) failed\n");
    return NULL;
    return NULL;
  }

  p[0] = (tenm_primitive *) tenm_circle_new(x, y, 5.0);
  if (p[0] == NULL)
  {
    fprintf(stderr, "solution_shot_new: cannot set p[0]\n");
    free(p);
    return NULL;
  }

  count = (int *) malloc(sizeof(int) * 1);
  if (count == NULL)
  {
    fprintf(stderr, "solution_shot_new: malloc(count) failed\n");
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }

  count_d = (double *) malloc(sizeof(double) * 3);
  if (count_d == NULL)
  {
    fprintf(stderr, "solution_shot_new: malloc(count_d) failed\n");
    free(count);
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }

  /* list of count
   * [0] color (just for delete_enemy_shot)
   */
  /* list of count_d
   * [0] speed x
   * [1] speed y
   * [2] ddx
   */

  count[0] = COLOR_PURPLE;

  count_d[0] = 0.0;
  count_d[1] = 3.0;
  if (t <= 11)
    count_d[2] = ((double) (t - 6)) * dddx;
  else
    count_d[2] = ((double) (18 - t)) * dddx;

  new = tenm_object_new("solution shot", ATTR_ENEMY_SHOT, 0,
                        1, x, y,
                        1, count, 3, count_d, 1, p,
                        (int (*)(tenm_object *, double)) solution_shot_move,
                        (int (*)(tenm_object *, tenm_object *)) NULL,
                        (int (*)(tenm_object *, const tenm_object *))
                        solution_shot_act,
                        (int (*)(tenm_object *, int)) solution_shot_draw);
  if (new == NULL)
  {
    fprintf(stderr, "solution_shot_new: tenm_object_new failed\n");
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
solution_shot_move(tenm_object *my, double turn_per_frame)
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
solution_shot_act(tenm_object *my, const tenm_object *player)
{
  /* sanity check */
  if (my == NULL)
    return 0;

  if ((my->count_d[0] > -3.0) && (my->count_d[0] < 3.0))
    my->count_d[0] += my->count_d[2];
  my->count_d[1] += 0.1;

  return 0;
}

static int
solution_shot_draw(tenm_object *my, int priority)
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
