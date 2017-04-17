/* $Id: midnight.c,v 1.72 2003/08/21 20:38:22 oohara Exp $ */

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
#include "laser.h"
#include "normal-shot.h"
#include "image.h"

#include "midnight.h"

#define MIDNIGHT_HIT_POINT 400

static int midnight_move(tenm_object *my, double turn_per_frame);
static int midnight_hit(tenm_object *my, tenm_object *your);
static int midnight_signal(tenm_object *my, int n);
static int midnight_act(tenm_object *my, const tenm_object *player);
static int midnight_draw(tenm_object *my, int priority);

static tenm_object *midnight_bit_new(int parent_index, int n,
                                     double x, double y);
static int midnight_bit_move(tenm_object *my, double turn_per_frame);
static int midnight_bit_hit(tenm_object *my, tenm_object *your);
static int midnight_bit_signal_to_parent(tenm_object *my, int n);
static int midnight_bit_signal_to_bit(tenm_object *my, int n);
static int midnight_bit_explosion(const tenm_object *my);
static int midnight_bit_act(tenm_object *my, const tenm_object *player);
static int midnight_bit_draw(tenm_object *my, int priority);

tenm_object *
midnight_new(void)
{
  tenm_primitive **p = NULL;
  tenm_object *new = NULL;
  int *count = NULL;
  double *count_d = NULL;
  double x = (double) (WINDOW_WIDTH / 2);
  double y = -47.0;

  p = (tenm_primitive **) malloc(sizeof(tenm_primitive *) * 2);
  if (p == NULL)
  {
    fprintf(stderr, "midnight_new: malloc(p) failed\n");
    return NULL;
  }

  p[0] = (tenm_primitive *) tenm_polygon_new(3,
                                             x - 64.0, y - 48.0,
                                             x + 32.0, y - 48.0,
                                             x - 32.0, y + 48.0);
  if (p[0] == NULL)
  {
    fprintf(stderr, "midnight_new: cannot set p[0]\n");
    free(p);
    return NULL;
  }

  p[1] = (tenm_primitive *) tenm_polygon_new(3,
                                             x + 64.0, y - 48.0,
                                             x - 32.0, y - 48.0,
                                             x + 32.0, y + 48.0);
  if (p[1] == NULL)
  {
    fprintf(stderr, "midnight_new: cannot set p[0]\n");
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }

  count = (int *) malloc(sizeof(int) * 7);
  if (count == NULL)
  {
    fprintf(stderr, "midnight_new: malloc(count) failed\n");
    (p[1])->delete(p[1]);
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }

  count_d = (double *) malloc(sizeof(double) * 2);
  if (count_d == NULL)
  {
    fprintf(stderr, "midnight_new: malloc(count_d) failed\n");
    (p[1])->delete(p[1]);
    (p[0])->delete(p[0]);
    free(p);
    if (count != NULL)
      free(count);
    return NULL;
  }

  /* list of count
   * [0] timer
   * [1] mode
   * [2] left bit index
   * [3] right bit index
   * [4] center bit index
   * [5] number of bit killed
   * [6] "damaged" flag
   */
  /* list of count_d
   * [0] speed x
   * [1] speed y
   */

  count[0] = 0;
  count[1] = 0;
  count[2] = -1;
  count[3] = -1;
  count[4] = -1;
  count[5] = 0;
  count[6] = 0;

  count_d[0] = 0.0;
  count_d[1] = 3.0;

  new = tenm_object_new("Midnight", ATTR_ENEMY, ATTR_PLAYER_SHOT,
                        MIDNIGHT_HIT_POINT, x, y,
                        7, count, 2, count_d, 2, p, 
                        (int (*)(tenm_object *, double)) midnight_move,
                        (int (*)(tenm_object *, tenm_object *)) midnight_hit,
                        (int (*)(tenm_object *, const tenm_object *))
                        midnight_act,
                        (int (*)(tenm_object *, int)) midnight_draw);
  if (new == NULL)
  {
    fprintf(stderr, "midnight_new: tenm_object_new failed\n");
    if (count_d != NULL)
      free(count_d);
    if (count != NULL)
      free(count);
    (p[1])->delete(p[1]);
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }

  return new;
}

static int
midnight_move(tenm_object *my, double turn_per_frame)
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
    fprintf(stderr, "midnight_move: the object is out of the window\n");
    return 1;
  }

  return 0;
}

static int
midnight_hit(tenm_object *my, tenm_object *your)
{
  int i;

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
  my->count[6] = 1;
  set_hit_status(my->name, MIDNIGHT_HIT_POINT, my->hit_point);

  if (my->hit_point <= 0)
  {
    tenm_table_add(explosion_new(my->x, my->y, 0.0, 0.0,
                                 1, 2000, COLOR_PURPLE, 8.0, 12));
    tenm_table_add(explosion_new(my->x, my->y, 0.0, 0.0,
                                 2, 500, COLOR_PURPLE, 6.0, 12));
    add_killed(COLOR_PURPLE);
    add_score(20000);
    tenm_table_apply_all((int (*)(tenm_object *, int)) delete_enemy_shot, 0);
    for (i = 2; i <= 4; i++)
    {
      if (my->count[i] >= 0)
        tenm_table_apply(my->count[i],
                         (int (*)(tenm_object *, int))midnight_signal, 0);
    }

    /* don't modify my->attr or my->hit_mask here, or the player shot
     * may fly through the enemy */
    if (my->mass != NULL)
      tenm_mass_delete(my->mass);
    my->mass = NULL;
    my->count[0] = 0;
    my->count[1] = 2;
    my->count[6] = 0;
    my->count_d[0] = 0.0;
    my->count_d[1] = 0.5;

    return 0;
  }

  return 0;
}

static int
midnight_signal(tenm_object *my, int n)
{
  /* sanity check */
  if (my == NULL)
    return 0;
  if ((strcmp(my->name, "Midnight side bit") != 0)
      && (strcmp(my->name, "Midnight center bit") != 0))
    return 0;

  my->count[0] = 0;
  my->count[1] = 2;

  my->count_d[0] = 0.0;
  if ((my->count[3] == 0)|| (my->count[3] == 1))
  {
    if (my->count_d[1] >= 0.0)
      my->count_d[1] = 1.0;
    else
      my->count_d[1] = -1.0;
  }
  else
  {
    my->count_d[1] = 0.0;
  }

  return 0;
}

static int
midnight_act(tenm_object *my, const tenm_object *player)
{
  int i;
  int j;
  double source_x;
  double source_y;

  if (my->count[1] == 0)
  {
    if (my->count[0] == 60)
      my->count_d[1] = 0.0;
    else if (my->count[0] == 110)
    {
      for (i = 0; i < 3; i++)
        my->count[i + 2] = tenm_table_add(midnight_bit_new(my->table_index, i,
                                                           my->x,
                                                           my->y - 20.0));
    }
    else if (my->count[0] >= 250)
    {
      my->count[0] = 0;
      my->count[1] = 1;
      my->count_d[0] = 3.0;
    }
  }
  else if (my->count[1] == 1)
  {
    if ((my->x <= 140.0) && (my->count_d[0] < 3.0))
      my->count_d[0] += 0.1;
    if ((my->x >= ((double) WINDOW_WIDTH) - 140.0) && (my->count_d[0] > -3.0))
      my->count_d[0] -= 0.1;

    if (my->count[5] <= 2)
    {
      if (my->count[0] <= 200)
      {
        if (my->count[0] % 40 == 0)
        {
          for (i = 0; i < 2; i++)
            tenm_table_add(laser_angle_new(my->x - 32.0 + (double) (i * 64),
                                           my->y, 10.0, 90,
                                           45.0, COLOR_PURPLE, 1000));
        }
        else if (my->count[0] % 40 == 20)
        {
          for (i = 0; i < 2; i++)
            for (j = -(my->count[5] + 1); j <= my->count[5] + 1; j++)
            {
              source_x = my->x - 32.0 + (double) (i * 64);
              tenm_table_add(normal_shot_angle_new(source_x, my->y, 5.5,
                                                   90 + j * 15,
                                                   COLOR_PURPLE, 1000));
            }
        }
      }
      else if ((my->count[0] > 220) && (my->count[0] <= 245))
      {
        tenm_table_add(laser_new(my->x, my->y, my->count_d[0], my->count_d[1],
                                 0.0,
                                 (double) (3 * (my->count[0] - 220)),
                                 COLOR_PURPLE, 1));
      }
      else if ((my->count[0] >= 246) && (my->count[0] <= 280))
      {
        tenm_table_add(laser_new(my->x, my->y, my->count_d[0], my->count_d[1],
                                 0.0, 700.0, COLOR_PURPLE, 1));
      }

      if (my->count[0] >= 280)
        my->count[0] = 0;
    }
    else
    {
      if (my->count[0] < 0)
      {
        tenm_table_add(laser_new(my->x, my->y, my->count_d[0], my->count_d[1],
                                 0.0,
                                 (double) (3 * my->count[0] + 25),
                                 COLOR_PURPLE, 1));
      }
      else
      {
        tenm_table_add(laser_new(my->x, my->y, my->count_d[0], my->count_d[1],
                                 0.0, 700.0, COLOR_PURPLE, 1));
        if (my->count[0] % 20 == 0)
        {
          if (my->count[0] % 40 == 0)
            source_x = my->x - 32.0;
          else
            source_x = my->x + 32.0;
          for (j = -3; j <= 3; j++)
          {
            tenm_table_add(normal_shot_point_new(source_x, my->y, 5.5,
                                                 player->x + ((double) j * 60),
                                                 player->y,
                                                 COLOR_PURPLE, 1000));
          }
        }
        if (my->count[0] >= 40)
          my->count[0] = 0;
      }
    }
  }
  else if (my->count[1] == 2)
  {
    if ((my->count[0] <= 105) && (my->count[0] % 15 == 0))
    {
      source_x = my->x + (double) (rand() % 81 - 40);
      source_y = my->y - (double) (rand() % 37 + 6);
      tenm_table_add(explosion_new(source_x, source_y, 0.0, 0.0,
                                   1, 2000, COLOR_PURPLE, 6.0, 12));
      tenm_table_add(explosion_new(source_x, source_y, 0.0, 0.0,
                                   2, 500, COLOR_PURPLE, 4.0, 12));
    }
    else if (my->count[0] == 150)
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
      tenm_table_add(stage_clear_new(1));
      return 1;
    }
  }

  (my->count[0])++;

  return 0;
}

static int
midnight_draw(tenm_object *my, int priority)
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

  if (my->count[6] == 0)
    n = 0;
  else
    n = 1;

  if (midnight_blit((int) (my->x), (int) (my->y), n) != 0)
  {
    fprintf(stderr, "midnight_draw: midnight_blit failed\n");
    my->count[6] = 0;
    return 1;
  }

  my->count[6] = 0;
  return 0;

  /* the below may be useful for debugging */

  color = tenm_map_color(78, 59, 156);
  if (tenm_draw_mass(my->mass, color) != 0)
  {
    fprintf(stderr, "midnight_draw: tenm_draw_mass failed\n");
    return 1;
  }
  return 0;
}

static tenm_object *
midnight_bit_new(int parent_index, int n, double x, double y)
{
  const char *name = NULL;
  tenm_primitive **p = NULL;
  tenm_object *new = NULL;
  int *count = NULL;
  double *count_d = NULL;
  double target_x;
  double target_y;

  /* sanity check */
  if ((n < 0) || (n > 2))
    return NULL;

  p = (tenm_primitive **) malloc(sizeof(tenm_primitive *) * 1);
  if (p == NULL)
  {
    fprintf(stderr, "midnight_bit_new: malloc(p) failed\n");
    return NULL;
  }

  p[0] = (tenm_primitive *) tenm_polygon_new(4,
                                             x + 32.0, y - 10.0,
                                             x - 32.0, y - 10.0,
                                             x - 32.0, y + 10.0,
                                             x + 32.0, y + 10.0);

  if (p[0] == NULL)
  {
    fprintf(stderr, "midnight_bit_new: cannot set p[0]\n");
    free(p);
    return NULL;
  }

  count = (int *) malloc(sizeof(int) * 6);
  if (count == NULL)
  {
    fprintf(stderr, "midnight_bit_new: malloc(count) failed\n");
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }

  count_d = (double *) malloc(sizeof(double) * 2);
  if (count_d == NULL)
  {
    fprintf(stderr, "midnight_bit_new: malloc(count_d) failed\n");
    (p[0])->delete(p[0]);
    free(p);
    if (count != NULL)
      free(count);
    return NULL;
  }

  if (n == 0)
  {
    target_x = 50.0;
    target_y = y;
  }
  else if (n == 1)
  {
    target_x = ((double) WINDOW_WIDTH) - 50.0;
    target_y = y;
  }
  else if (n == 2)
  {
    target_x = x;
    target_y = 40;
  }
  else
  {
    fprintf(stderr, "midnight_bit_new: strange n (%d)\n", n);
    if (count_d != NULL)
      free(count_d);
    if (count != NULL)
      free(count);
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }

  /* list of count
   * [0] timer
   * [1] mode
   * [2] Midnight index
   * [3] left/right/center
   * [4] number of bit killed
   * [5] "damaged" flag
   */
  /* list of count_d
   * [0] speed x
   * [1] speed y
   */

  count[0] = 0;
  count[1] = 0;
  count[2] = parent_index;
  count[3] = n;
  count[4] = 0;
  count[5] = 0;

  count_d[0] = (target_x - x) / 30.0;
  count_d[1] = (target_y - y) / 30.0;

  if ((n == 0) || (n == 1))
  {
    name = "Midnight side bit";
  }
  else if (n == 2)
  {
    name = "Midnight center bit";
  }
  else
  {
    fprintf(stderr, "midnight_bit_new: strange n (%d)\n", n);
    if (count_d != NULL)
      free(count_d);
    if (count != NULL)
      free(count);
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }

  new = tenm_object_new(name, ATTR_ENEMY, ATTR_PLAYER_SHOT,
                        150, x, y,
                        6, count, 2, count_d, 1, p, 
                        (int (*)(tenm_object *, double)) midnight_bit_move,
                        (int (*)(tenm_object *, tenm_object *))
                        midnight_bit_hit,
                        (int (*)(tenm_object *, const tenm_object *))
                        midnight_bit_act,
                        (int (*)(tenm_object *, int)) midnight_bit_draw);
  if (new == NULL)
  {
    fprintf(stderr, "midnight_bit_new: tenm_object_new failed\n");
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
midnight_bit_move(tenm_object *my, double turn_per_frame)
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
    fprintf(stderr, "midnight_bit_move: the object is out of the window\n");
    return 1;
  }

  return 0;
}

static int
midnight_bit_hit(tenm_object *my, tenm_object *your)
{
  int color;

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
  my->count[5] = 1;
  set_hit_status(my->name, 150, my->hit_point);

  if (my->hit_point <= 0)
  {
    if (my->count[3] == 0)
    {
      color = COLOR_BLUE;
    }
    else if (my->count[3] == 1)
    {
      color = COLOR_RED;
    }
    else if (my->count[3] == 2)
    {
      color = COLOR_PURPLE;
    }
    else
    {
      fprintf(stderr, "midnight_bit_hit: strange count[3] (%d)\n",
              my->count[3]);
      color = COLOR_GREEN;
    }

    midnight_bit_explosion(my);
    add_killed(color);
    add_score(10000);
    tenm_table_apply(my->count[2],
                     (int (*)(tenm_object *,int))midnight_bit_signal_to_parent,
                     my->count[3]);
    return 1;
  }

  return 0;
}

static int
midnight_bit_signal_to_parent(tenm_object *my, int n)
{
  int i;

  /* sanity check */
  if (my == NULL)
    return 0;
  if ((n < 0) || (n > 2))
    return 0;
  if (strcmp(my->name, "Midnight") != 0)
    return 0;

  for (i = 2; i <= 4; i++)
  {
    if (i == n + 2)
      my->count[i] = -1;
    else if (my->count[i] >= 0)
      tenm_table_apply(my->count[i],
                       (int (*)(tenm_object *, int))midnight_bit_signal_to_bit,
                       0);
  }
  
  (my->count[5])++;
  if (my->count[5] >= 3)
  {
    my->count[0] = -24;
  }

  return 0;
}

static int
midnight_bit_signal_to_bit(tenm_object *my, int n)
{
  /* sanity check */
  if (my == NULL)
    return 0;
  if ((strcmp(my->name, "Midnight side bit") != 0)
      && (strcmp(my->name, "Midnight center bit") != 0))
    return 0;

  (my->count[4])++;

  return 0;
}

static int
midnight_bit_explosion(const tenm_object *my)
{
  int color;

  if (my->count[3] == 0)
  {
    color = COLOR_BLUE;
  }
  else if (my->count[3] == 1)
  {
    color = COLOR_RED;
  }
  else if (my->count[3] == 2)
  {
    color = COLOR_PURPLE;
  }
  else
  {
    fprintf(stderr, "midnight_bit_hit: strange count[3] (%d)\n",
            my->count[3]);
    color = COLOR_GREEN;
  }

  tenm_table_add(explosion_new(my->x, my->y, 0.0, 0.0,
                               1, 2000, color, 6.0, 12));
  tenm_table_add(explosion_new(my->x, my->y, 0.0, 0.0,
                               1, 400, color, 4.0, 12));
  return 0;
}

static int
midnight_bit_act(tenm_object *my, const tenm_object *player)
{
  int theta;
  double length;
  double source_x;
  int color;
  double speed;

  if (my->count[1] == 0)
  {
    if (my->count[0] == 30)
    {
      my->count_d[0] = 0.0;
      my->count_d[1] = 0.0;
    }
    if (my->count[0] >= 80)
    {
      my->count[0] = 0;
      my->count[1] = 1;
      if ((my->count[3] == 0) || (my->count[3] == 1))
        my->count_d[1] = 5.5 + 0.3 * ((double) (rand() % 8));
    }
  }
  else if (my->count[1] == 1)
  {
    if ((my->count[3] == 0) || (my->count[3] == 1))
    {
      if (my->y <= 0.0)
        my->count_d[1] = 5.5 + 0.3 * ((double) (rand() % 8));
      else if (my->y >= (double) WINDOW_HEIGHT)
        my->count_d[1] = -(5.5 + 0.3 * ((double) (rand() % 8)));

      if ((my->count[0] >= 20)
          || ((my->count[4] >= 2) && (my->count[0] % 10 == 0)))
      {
        if (my->count[3] == 0)
        {
          theta = 0;
          color = COLOR_BLUE;
          if (my->count[4] >= 1)
          {
            length = 60.0;
            source_x = my->x - 30;
            speed = 9.0;
          }
          else
          {
            length = 30.0;
            source_x = my->x;
            speed = 7.0;
          }
        }
        else
        {
          theta = 180;
          color = COLOR_RED;
          if (my->count[4] >= 1)
          {
            length = 60.0;
            source_x = my->x + 30;
            speed = 11.0;
          }
          else
          {
            length = 30.0;
            source_x = my->x;
            speed = 7.0;
          }
        }
        if (my->count[4] >= 2)
          speed = 11.0;
        tenm_table_add(laser_angle_new(source_x, my->y, speed, theta,
                                       length, color, 1000));
      }
      if (my->count[0] >= 20)
        my->count[0] = 0;
    }
    else if (my->count[3] == 2)
    {
      if ((my->count[0] == 16)
          || ((my->count[4] >= 2) && (my->count[0] % 8 == 0)))
      {
        if (my->count[0] == 8)
          source_x = my->x - 15.0;
        else if (my->count[0] == 16)
          source_x = my->x;
        else
          source_x = my->x + 15.0;

        if (my->count[0] >= 2)
          speed = 9.0;
        else
          speed = 7.0;

        tenm_table_add(normal_shot_point_new(source_x, my->y, speed,
                                             player->x,
                                             player->y,
                                             COLOR_PURPLE, 1000));
      }
      if (my->count[0] >= 24)
        my->count[0] = 0;
    }
  }
  else if (my->count[1] == 2)
  {
    /* don't modify my->attr or my->hit_mask here, or the player shot
     * may fly through the enemy */
    if (my->mass != NULL)
      tenm_mass_delete(my->mass);
    my->mass = NULL;

    if (my->y <= 0.0)
      my->count_d[1] = 1.0;
    else if (my->y >= (double) WINDOW_HEIGHT)
      my->count_d[1] = -1.0;

    if (my->count[0] >= 10)
    {
      midnight_bit_explosion(my);
      return 1;
    }
  }

  (my->count[0])++;
  return 0;
}

static int
midnight_bit_draw(tenm_object *my, int priority)
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

  n = my->count[3];
  if (my->count[5] != 0)
    n += 3;

  if (midnight_bit_blit((int) (my->x), (int) (my->y), n) != 0)
  {
    fprintf(stderr, "midnight_bit_draw: tenm_draw_mass failed\n");
    my->count[5] = 0;
    return 1;
  }

  my->count[5] = 0;
  return 0;

  /* the below may be useful for debugging */

  if (my->count[3] == 0)
  {
    /* blue */
    color = tenm_map_color(59, 112, 156);
  }
  else if (my->count[3] == 1)
  {
    /* red */
    color = tenm_map_color(156, 59, 72);
  }
  else if (my->count[3] == 2)
  {
    /* purple */
    color = tenm_map_color(78, 59, 156);
  }
  else
  {
    /* should not reach here */
    color = tenm_map_color(0, 0, 0);
  }

  if (tenm_draw_mass(my->mass, color) != 0)
  {
    fprintf(stderr, "midnight_bit_draw: tenm_draw_mass failed\n");
    return 1;
  }

  return 0;
}
