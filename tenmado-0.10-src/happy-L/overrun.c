/* $Id: overrun.c,v 1.69 2003/08/21 20:39:25 oohara Exp $ */

#include <stdio.h>
/* malloc, rand */
#include <stdlib.h>
/* strcmp */
#include <string.h>

#include "tenm_primitive.h"
#include "tenm_object.h"
#include "const.h"
#include "tenm_table.h"
#include "stage-clear.h"
#include "score.h"
#include "tenm_graphic.h"
#include "hit-status.h"
#include "explosion.h"
#include "chain.h"
#include "util.h"
#include "normal-shot.h"
#include "tenm_math.h"
#include "laser.h"
#include "image.h"

#include "overrun.h"

#define OVERRUN_HIT_POINT 500
#define OVERRUN_BIT_HIT_POINT 250

static int overrun_move(tenm_object *my, double turn_per_frame);
static int overrun_hit(tenm_object *my, tenm_object *your);
static int overrun_signal(tenm_object *my, int n);
static int overrun_act(tenm_object *my, const tenm_object *player);
static int overrun_draw(tenm_object *my, int priority);

static tenm_object *overrun_bit_new(double x, double y, int n,
                                    int parent_index);
static int overrun_bit_move(tenm_object *my, double turn_per_frame);
static int overrun_bit_hit(tenm_object *my, tenm_object *your);
static int overrun_bit_explosion(const tenm_object *my);
static int overrun_bit_signal_to_overrun(tenm_object *my, int n);
static int overrun_bit_signal_to_bit(tenm_object *my, int n);
static int overrun_bit_act(tenm_object *my, const tenm_object *player);
static int overrun_bit_draw(tenm_object *my, int priority);

tenm_object *
overrun_new(void)
{
  tenm_primitive **p = NULL;
  tenm_object *new = NULL;
  int *count = NULL;
  double *count_d = NULL;
  double x = (double) (WINDOW_WIDTH / 2);
  double y = -49.0;
  int i;

  p = (tenm_primitive **) malloc(sizeof(tenm_primitive *) * 2);
  if (p == NULL)
  {
    fprintf(stderr, "overrun_new: malloc(p) failed\n");
    return NULL;
  }

  p[0] = (tenm_primitive *) tenm_polygon_new(4,
                                             x - 50.0, y - 50.0,
                                             x + 50.0, y - 50.0,
                                             x + 50.0, y + 50.0,
                                             x - 50.0, y + 50.0);
  if (p[0] == NULL)
  {
    fprintf(stderr, "overrun_new: cannot set p[0]\n");
    free(p);
    return NULL;
  }

  count = (int *) malloc(sizeof(int) * 9);
  if (count == NULL)
  {
    fprintf(stderr, "overrun_new: malloc(count) failed\n");
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }

  count_d = (double *) malloc(sizeof(double) * 2);
  if (count_d == NULL)
  {
    fprintf(stderr, "overrun_new: malloc(count_d) failed\n");
    (p[0])->delete(p[0]);
    free(p);
    if (count != NULL)
      free(count);
    return NULL;
  }

  /* list of count
   * [0] timer
   * [1] mode
   * [2 -- 7] bit index
   * [8] "damaged" flag
   */
  /* list of count_d
   * [0] speed x
   * [1] speed y
   */

  count[0] = 0;
  count[1] = 0;
  for (i = 2; i <= 7; i++)
    count[i] = -1;
  count[8] = 0;

  count_d[0] = 0.0;
  count_d[1] = (((double) (WINDOW_HEIGHT / 2)) - y) / 60.0;

  new = tenm_object_new("Overrun", ATTR_ENEMY, ATTR_PLAYER_SHOT,
                        OVERRUN_HIT_POINT, x, y,
                        9, count, 2, count_d, 1, p, 
                        (int (*)(tenm_object *, double)) overrun_move,
                        (int (*)(tenm_object *, tenm_object *)) overrun_hit,
                        (int (*)(tenm_object *, const tenm_object *))
                        overrun_act,
                        (int (*)(tenm_object *, int)) overrun_draw);
  if (new == NULL)
  {
    fprintf(stderr, "overrun_new: tenm_object_new failed\n");
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
overrun_move(tenm_object *my, double turn_per_frame)
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
  if (my->mass != NULL)
    tenm_move_mass(my->mass, dx_temp, dy_temp);

  if ((my->mass != NULL) && (!in_window_object(my)))
  {
    fprintf(stderr, "overrun_move: the object is out of the window\n");
    return 1;
  }
  return 0;
}

static int
overrun_hit(tenm_object *my, tenm_object *your)
{
  int i;
  int n;

  /* sanity check */
  if (my == NULL)
    return 0;
  if (your == NULL)
    return 0;

  if (!(your->attr & ATTR_PLAYER_SHOT))
    return 0;

  /* immutable time */
  if (my->count[1] != 1)
    return 0;
  my->hit_point -= add_damage_score(my->hit_point, your->hit_point);
  my->count[8] = 1;
  set_hit_status(my->name, OVERRUN_HIT_POINT, my->hit_point);

  if (my->hit_point <= 0)
  {
    tenm_table_apply_all((int (*)(tenm_object *, int)) delete_enemy_shot, 0);
    tenm_table_add(explosion_new(my->x, my->y, 0.0, 0.0,
                                 1, 2000, COLOR_PURPLE, 8.0, 12));
    tenm_table_add(explosion_new(my->x, my->y, 0.0, 0.0,
                                 2, 500, COLOR_PURPLE, 6.0, 12));
    add_killed(COLOR_PURPLE);
    add_score(20000);
    for (i = 0; i < 6; i++)
    {
      if (i == 0)
        n = 30;
      else if ((i == 1) || (i == 2))
        n = 70;
      else
        n = 10;

      if (my->count[i + 2] >= 0)
        tenm_table_apply(my->count[i + 2],
                         (int (*)(tenm_object *, int)) overrun_signal, n);
    }
    /* don't modify my->attr or my->hit_mask here, or the player shot
     * may fly through the enemy */
    if (my->mass != NULL)
      tenm_mass_delete(my->mass);
    my->mass = NULL;
    my->count[1] = 2;
    my->count_d[0] *= 0.2;
    my->count_d[1] *= 0.2;
  }

  return 0;
}

static int
overrun_signal(tenm_object *my, int n)
{
  /* sanity check */
  if (my == NULL)
    return 0;
  if (strcmp(my->name, "Overrun bit") != 0)
    return 0;

  /* don't modify my->attr or my->hit_mask here, or the player shot
   * may fly through the enemy */
  if (my->mass != NULL)
    tenm_mass_delete(my->mass);
  my->mass = NULL;
  my->count[0] = n;
  my->count[1] = 1;
  my->count_d[0] *= 0.2;
  my->count_d[1] *= 0.2;
  
  return 0;
}

static int
overrun_act(tenm_object *my, const tenm_object *player)
{
  int i;
  double temp_x;
  double temp_y;

  /* sanity check */
  if (my == NULL)
    return 0;
  if (player == NULL)
    return 0;

  (my->count[0])++;
  if (my->count[1] == 0)
  {
    if (my->count[0] == 60)
    {
      my->count_d[0] = 0.0;
      my->count_d[1] = 0.0;
    }
    else if (my->count[0] == 150)
    {
      my->count_d[0] = 6.0 * tenm_cos(45);
      my->count_d[1] = 6.0 * tenm_sin(45);
      for (i = 0; i < 6; i++)
        my->count[i + 2] = tenm_table_add(overrun_bit_new(my->x, my->y, i,
                                                          my->table_index));
      my->count[1] = 1;
    }
  }
  else if (my->count[1] == 1)
  {
    /* speed change */
    temp_x = my->x + my->count_d[0];
    temp_y = my->y + my->count_d[1];
    if ((temp_x < 0.0) || (temp_x >= ((double) WINDOW_WIDTH)))
      my->count_d[0] *= -1.0;
    if ((temp_y < 0.0) || (temp_y >= ((double) WINDOW_HEIGHT)))
      my->count_d[1] *= -1.0;

    /* shoot */
    if (my->count[0] % 6 == 0)
    {
      tenm_table_add(normal_shot_new(my->x, my->y, 0.0, 0.0,
                                     COLOR_PURPLE, 300));
    }
    if (my->count[0] >= 24)
    {
      tenm_table_add(laser_point_new(my->x, my->y, 4.5,
                                     player->x, player->y, 40.0,
                                     COLOR_PURPLE, 1000));
      my->count[0] = 0;
    }
  }
  else if (my->count[1] == 2)
  {
    /* speed change */
    temp_x = my->x + my->count_d[0];
    temp_y = my->y + my->count_d[1];
    if ((temp_x < 0.0) || (temp_x >= ((double) WINDOW_WIDTH)))
      my->count_d[0] *= -1.0;
    if ((temp_y < 0.0) || (temp_y >= ((double) WINDOW_HEIGHT)))
      my->count_d[1] *= -1.0;

    if (my->count[0] == 150)
    {
      tenm_table_add(explosion_new(my->x, my->y, 0.0, 0.0,
                                   1, 8000, COLOR_PURPLE, 10.0, 12));
      tenm_table_add(explosion_new(my->x, my->y, 0.0, 0.0,
                                   2, 2000, COLOR_PURPLE, 8.0, 12));
      tenm_table_add(explosion_new(my->x, my->y, 0.0, 0.0,
                                   3, 300, COLOR_PURPLE, 5.0, 12));
    }
    else if (my->count[0] >= 300)
    {
      tenm_table_add(stage_clear_new(2));
      return 1;
    }
  }
  
  return 0;
}

static int
overrun_draw(tenm_object *my, int priority)
{
  int n = 0;
  tenm_color color;

  /* sanity check */
  if (my == NULL)
    return 0;
  if (my->count[1] == 2)
  {
    /* dead enemy has low priority */
    if (priority != -1)
      return 0;
  }
  else
  {
    if (priority != 0)
      return 0;
  }
  
  if ((my->count[1] == 2) && (my->count[0] >= 150))
    return 0;

  if (my->count[8] == 0)
    n = 0;
  else
    n = 1;

  if (overrun_blit((int) (my->x), (int) (my->y), n) != 0)
  {
    fprintf(stderr, "overrun_draw: overrun_blit failed\n");
    my->count[8] = 0;
    return 1;
  }

  my->count[8] = 0;
  return 0;

  /* the below may be useful for debugging */

  color = tenm_map_color(78, 59, 156);
  if (tenm_draw_mass(my->mass, color) != 0)
  {
    fprintf(stderr, "overrun_draw: tenm_draw_mass failed\n");
    return 1;
  }
  return 0;
}

static tenm_object *
overrun_bit_new(double x, double y, int n,
                int parent_index)
{
  tenm_primitive **p = NULL;
  tenm_object *new = NULL;
  int *count = NULL;
  double *count_d = NULL;
  int attr;
  int hit_mask;
  int theta;
  double speed;

  p = (tenm_primitive **) malloc(sizeof(tenm_primitive *) * 2);
  if (p == NULL)
  {
    fprintf(stderr, "overrun_bit_new: malloc(p) failed\n");
    return NULL;
  }

  p[0] = (tenm_primitive *) tenm_polygon_new(4,
                                             x - 25.0, y - 25.0,
                                             x + 25.0, y - 25.0,
                                             x + 25.0, y + 25.0,
                                             x - 25.0, y + 25.0);
  if (p[0] == NULL)
  {
    fprintf(stderr, "overrun_bit_new: cannot set p[0]\n");
    free(p);
    return NULL;
  }

  count = (int *) malloc(sizeof(int) * 7);
  if (count == NULL)
  {
    fprintf(stderr, "overrun_bit_new: malloc(count) failed\n");
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }

  count_d = (double *) malloc(sizeof(double) * 2);
  if (count_d == NULL)
  {
    fprintf(stderr, "overrun_bit_new: malloc(count_d) failed\n");
    (p[0])->delete(p[0]);
    free(p);
    if (count != NULL)
      free(count);
    return NULL;
  }

  if (n <= 2)
  {
    attr = ATTR_OBSTACLE;
    hit_mask = 0;
  }
  else
  {
    attr = ATTR_ENEMY | ATTR_OPAQUE;
    hit_mask = ATTR_PLAYER_SHOT;
  }
  
  if (n == 0)
    theta = 30;
  else if (n == 1)
    theta = 50;
  else if (n == 2)
    theta = 110;
  else if (n == 3)
    theta = 140;  
  else if (n == 4)
    theta = 120;
  else
    theta = 20;

  if (n >= 3)
    speed = 9.5;
  else
    speed = 7.5;

  /* list of count
   * [0] timer
   * [1] mode
   * [2] color
   * [3] Overrun index
   * [4] n
   * [5] number of corresponding bit killed
   * [6] "damaged" flag
   */
  /* list of count_d
   * [0] speed x
   * [1] speed y
   */

  count[0] = (n % 3) * 13;
  count[1] = 0;
  if (n <= 2)
    count[2] = COLOR_GREEN;
  else if (n == 3)
    count[2] = COLOR_BLUE;
  else if (n == 4)
    count[2] = COLOR_PURPLE;
  else
    count[2] = COLOR_RED;
  count[3] = parent_index;
  count[4] = n;
  count[5] = 0;
  count[6] = 0;

  count_d[0] = speed * tenm_cos(theta);
  count_d[1] = speed * tenm_sin(theta);

  new = tenm_object_new("Overrun bit", attr, hit_mask,
                        OVERRUN_BIT_HIT_POINT, x, y,
                        7, count, 2, count_d, 1, p, 
                        (int (*)(tenm_object *, double)) overrun_bit_move,
                        (int (*)(tenm_object *, tenm_object *))
                        overrun_bit_hit,
                        (int (*)(tenm_object *, const tenm_object *))
                        overrun_bit_act,
                        (int (*)(tenm_object *, int)) overrun_bit_draw);
  if (new == NULL)
  {
    fprintf(stderr, "overrun_bit_new: tenm_object_new failed\n");
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
overrun_bit_move(tenm_object *my, double turn_per_frame)
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
  if (my->mass != NULL)
    tenm_move_mass(my->mass, dx_temp, dy_temp);

  if ((my->mass != NULL) && (!in_window_object(my)))
  {
    fprintf(stderr, "overrun_bit_move: the object is out of the window\n");
    return 1;
  }
  return 0;
}

static int
overrun_bit_hit(tenm_object *my, tenm_object *your)
{
  /* sanity check */
  if (my == NULL)
    return 0;
  if (your == NULL)
    return 0;

  if (!(your->attr & ATTR_PLAYER_SHOT))
    return 0;

  /* you can't destroy a green enemy */
  if (my->count[4] <= 2)
    return 0;

  my->hit_point -= add_damage_score(my->hit_point, your->hit_point);
  my->count[6] = 1;
  set_hit_status(my->name, OVERRUN_BIT_HIT_POINT, my->hit_point);

  if (my->hit_point <= 0)
  {
    tenm_table_apply(my->count[3],
                     (int (*)(tenm_object *, int))
                     overrun_bit_signal_to_overrun,
                     my->count[4]);
    overrun_bit_explosion(my);
    add_killed(my->count[2]);
    add_score(10000);
    return 1;
  }

  return 0;
}

static int
overrun_bit_explosion(const tenm_object *my)
{
  tenm_table_add(explosion_new(my->x, my->y, 0.0, 0.0,
                               1, 2000, my->count[2], 8.0, 12));
  tenm_table_add(explosion_new(my->x, my->y, 0.0, 0.0,
                               2, 500, my->count[2], 6.0, 12));
  return 0;
}

static int
overrun_bit_signal_to_overrun(tenm_object *my, int n)
{
  int i;

  /* sanity check */
  if (my == NULL)
    return 0;
  if (strcmp(my->name, "Overrun") != 0)
    return 0;

  for (i = 0; i < 6; i++)
  {
    if (i == n)
      my->count[i + 2] = -1;
    else if ((n >= 3) && (i == n - 3))
      tenm_table_apply(my->count[i + 2],
                       (int (*)(tenm_object *, int)) overrun_bit_signal_to_bit,
                       0);
  }
  
  return 0;
}

static int
overrun_bit_signal_to_bit(tenm_object *my, int n)
{
  /* sanity check */
  if (my == NULL)
    return 0;
  if (strcmp(my->name, "Overrun bit") != 0)
    return 0;

  (my->count[5])++;
  
  return 0;
}

static int
overrun_bit_act(tenm_object *my, const tenm_object *player)
{
  double temp_x;
  double temp_y;

  /* sanity check */
  if (my == NULL)
    return 0;
  if (player == NULL)
    return 0;

  /* speed change */
  temp_x = my->x + my->count_d[0];
  temp_y = my->y + my->count_d[1];
  if ((temp_x < 0.0) || (temp_x >= ((double) WINDOW_WIDTH)))
    my->count_d[0] *= -1.0;
  if ((temp_y < 0.0) || (temp_y >= ((double) WINDOW_HEIGHT)))
    my->count_d[1] *= -1.0;

  /* shoot */
  if (my->count[1] == 0)
  {
    (my->count[0])++;
    if (my->count[0] >= 39)
    {
      my->count[0] = 0;
      if (my->count[5] >= 1)
        tenm_table_add(laser_point_new(my->x, my->y, 5.5, player->x, player->y,
                                       20.0, my->count[2], 1000));
    }
  }
  else if (my->count[1] == 1)
  {
    (my->count[0])--;
    if (my->count[0] <= 0)
    {
      overrun_bit_explosion(my);
      return 1;
    }
  }
  else
    fprintf(stderr, "overrun_bit_act: strange mode (%d)\n", my->count[1]);
  
  return 0;
}

static int
overrun_bit_draw(tenm_object *my, int priority)
{
  tenm_color color;
  int n;

  /* sanity check */
  if (my == NULL)
    return 0;
  if (my->count[1] == 2)
  {
    /* dead enemy has low priority */
    if (priority != -1)
      return 0;
  }
  else
  {
    if (priority != 0)
      return 0;
  }

  switch (my->count[2])
  {
  case COLOR_BLUE:
    n = 0;
    break;
  case COLOR_PURPLE:
    n = 1;
    break;
  case COLOR_RED:
    n = 2;
    break;
  case COLOR_GREEN:
    n = 3;
    break;
  default:
    fprintf(stderr, "overrun_bit_draw: strange my->count[2] (%d)\n",
            my->count[2]);
    n = 0;
    break;
  }

  if ((my->count[2] != COLOR_GREEN) && (my->count[6] != 0))
    n += 4;

  if (overrun_bit_blit((int) (my->x), (int) (my->y), n) != 0)
  {
    fprintf(stderr, "overrun_bit_draw: overrun_bit_blit failed\n");
    my->count[6] = 0;
    return 1;
  }
  my->count[6] = 0;
  return 0;
  /* the below may be useful for debugging */

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

  if (tenm_draw_mass(my->mass, color) != 0)
  {
    fprintf(stderr, "overrun_bit_draw: tenm_draw_mass failed\n");
    return 1;
  }
  return 0;
}
