/* $Id: fud.c,v 1.124 2003/08/22 04:58:05 oohara Exp $ */

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
#include "image.h"

#include "fud.h"

#define WALL_SIZE 50.0
#define FUD_HIT_POINT 150
#define TRINITY_HIT_POINT 150

static int fud_act(tenm_object *my, const tenm_object *player);

static int fud_wall_signal(tenm_object *my, int n);

static tenm_object *fud_core_new(int parent_index, int n, double x, double y);
static int fud_core_move(tenm_object *my, double turn_per_frame);
static int fud_core_hit(tenm_object *my, tenm_object *your);
static int fud_core_act(tenm_object *my, const tenm_object *player);
static int fud_core_draw(tenm_object *my, int priority);

static int fud_core_signal(tenm_object *my, int n);
static int fud_core_speed_up(tenm_object *my, int n);

static tenm_object *fud_wall_new(int n, double x, double y);
static int fud_wall_move(tenm_object *my, double turn_per_frame);
static int fud_wall_act(tenm_object *my, const tenm_object *player);
static int fud_wall_draw(tenm_object *my, int priority);

tenm_object *
fud_new(void)
{
  int i;
  tenm_object *new = NULL;
  int *count = NULL;

  count = (int *) malloc(sizeof(int) * 17);
  if (count == NULL)
  {
    fprintf(stderr, "fud_new: malloc(count) failed\n");
    return NULL;
  }

  /* list of count
   * [0] timer
   * [1] Fear index
   * [2] Uncertainty index
   * [3] Doubt index
   * [4] Trinity index
   * [5 -- 16] wall index
   */
  count[0] = -188;
  for (i = 1; i <= 16; i++)
    count[i] = -1;

  new = tenm_object_new("FUD manager", 0, 0,
                        0, 0.0, 0.0,
                        17, count, 0, NULL, 0, NULL, 
                        (int (*)(tenm_object *, double)) NULL,
                        (int (*)(tenm_object *, tenm_object *)) NULL,
                        (int (*)(tenm_object *, const tenm_object *))
                        fud_act,
                        (int (*)(tenm_object *, int)) NULL);
  if (new == NULL)
  {
    fprintf(stderr, "fud_new: tenm_object_new failed\n");
    free(count);
    return NULL;
  }

  return new;
}

static int
fud_act(tenm_object *my, const tenm_object *player)
{
  int i;
  double temp_x;

  /* sanity check */
  if (my == NULL)
    return 0;

  if (my->count[0] < 0)
  {
    (my->count[0])++;
    if (my->count[0] == -187)
    {
      for (i = 0; i < 3; i++)
      {
        temp_x = (double) ((WINDOW_WIDTH + 50 * 2) * i / 3);
        my->count[5 + i] = tenm_table_add(fud_wall_new(0, temp_x, -49.0));
      }
      return 0;
    }
    else if (my->count[0] == -125)
    {
      for (i = 0; i < 3; i++)
      {
        temp_x = (double) ((WINDOW_WIDTH + 50 * 2) * i / 3);
        my->count[8 + i] = tenm_table_add(fud_wall_new(1, temp_x, -49.0));
      }
      return 0;
    }
    else if (my->count[0] == -94)
    {
      for (i = 0; i < 3; i++)
      {
        temp_x = (double) ((WINDOW_WIDTH + 25 * 2) * i / 3);
        my->count[i + 1] = tenm_table_add(fud_core_new(my->table_index, i,
                                                       temp_x, -24.0));
      }
      return 0;
    }
    else if (my->count[0] == -63)
    {
      for (i = 0; i < 3; i++)
      {
        temp_x = (double) ((WINDOW_WIDTH + 50 * 2) * i / 3);
        my->count[11 + i] = tenm_table_add(fud_wall_new(2, temp_x, -49.0));
      }
      return 0;
    }
    else if (my->count[0] == -1)
    {
      for (i = 0; i < 3; i++)
      {
        temp_x = (double) ((WINDOW_WIDTH + 50 * 2) * i / 3);
        my->count[14 + i] = tenm_table_add(fud_wall_new(3, temp_x, -49.0));
      }
      return 0;
    }
    return 0;
  }

  /* 0: at least one of Fear, Uncertainty and Doubt is alive
   * 1: Trinity is alive
   */
  if ((my->count[0] != 0) && (my->count[0] != 1))
    (my->count[0])++;
  
  if (my->count[0] >= 300)
  {
    tenm_table_add(stage_clear_new(4));
    return 1;
  }
  return 0;
}

static int
fud_wall_signal(tenm_object *my, int n)
{
  /* sanity check */
  if (my == NULL)
    return 0;
  if (strcmp(my->name, "wall") != 0)
    return 0;

  /* dirty hack to draw dead walls */
  my->attr = 0;
  my->hit_mask = 0;
  my->count[0] = n;
  if (my->count_d[0] > 0.0)
    my->count_d[0] = 1.0;
  else
    my->count_d[0] = -1.0;
  return 0;
}

static tenm_object *
fud_core_new(int parent_index, int n, double x, double y)
{
  tenm_primitive **p = NULL;
  tenm_object *new = NULL;
  int *count = NULL;
  double *count_d = NULL;
  const char *name = NULL;
  int hit_point = FUD_HIT_POINT;

  /* sanity check */
  if ((n < 0) || (n > 3))
    return NULL;

  p = (tenm_primitive **) malloc(sizeof(tenm_primitive *) * 1);
  if (p == NULL)
  {
    fprintf(stderr, "fud_core_new: malloc(p) failed\n");
    return NULL;
  }

  p[0] = (tenm_primitive *) tenm_circle_new(x, y, 25.0);
  if (p[0] == NULL)
  {
    fprintf(stderr, "fud_core_new: cannot set p[0]\n");
    free(p);
    return NULL;
  }

  count = (int *) malloc(sizeof(int) * 7);
  if (count == NULL)
  {
    fprintf(stderr, "fud_core_new: malloc(count) failed\n");
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }

  count_d = (double *) malloc(sizeof(double) * 1);
  if (count_d == NULL)
  {
    fprintf(stderr, "fud_core_new: malloc(count_d) failed\n");
    (p[0])->delete(p[0]);
    free(p);
    if (count != NULL)
      free(count);
    return NULL;
  }

  hit_point = FUD_HIT_POINT;
  count[3] = -1;
  count_d[0] = 2.5;

  if (n == 0)
  {
    name = "Fear";
    count[0] = COLOR_BLUE;
  }
  else if (n == 1)
  {
    name = "Uncertainty";
    count[0] = COLOR_PURPLE;
  }
  else if (n == 2)
  {
    name = "Doubt";
    count[0] = COLOR_RED;
  }
  else if (n == 3)
  {
    name = "Trinity";
    hit_point = TRINITY_HIT_POINT;
    count[0] = COLOR_PURPLE;
    count[3] = 0;
    count_d[0] = 10.0;
  }
  else
  {
    fprintf(stderr, "fud_core_new: strange n (%d)\n", n);
    if (count_d != NULL)
      free(count_d);
    if (count != NULL)
      free(count);
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }
  
  /* list of count
   * [0] color
   * [1] table index of parent (FUD manager)
   * [2] shoot timer
   * [3] long laser timer (< 0 means it is disabled)
   * [4] long laser direction
   * [5] n
   * [6] "damaged" flag
   */
  /* list of count_d
   * [0] speed x
   */
  /* count[0] is set above */
  count[1] = parent_index;
  count[2] = 0;
  /* count[3] is set above */
  count[4] = 0;
  count[5] = n;
  count[6] = 0;

  /* count_d[0] is set above */

  new = tenm_object_new(name, ATTR_ENEMY, ATTR_PLAYER_SHOT,
                        hit_point, x, y,
                        7, count, 1, count_d, 1, p, 
                        (int (*)(tenm_object *, double)) fud_core_move,
                        (int (*)(tenm_object *, tenm_object *)) fud_core_hit,
                        (int (*)(tenm_object *, const tenm_object *))
                        fud_core_act,
                        (int (*)(tenm_object *, int)) fud_core_draw);
  if (new == NULL)
  {
    fprintf(stderr, "fud_core_new: tenm_object_new failed\n");
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
fud_core_move(tenm_object *my, double turn_per_frame)
{
  double dx_temp;
  double dy_temp;

  /* sanity check */
  if (my == NULL)
    return 0;
  if (turn_per_frame <= 0.5)
    return 0;

  dx_temp = my->count_d[0] / turn_per_frame;
  dy_temp = 3.0 / turn_per_frame;
  my->x += dx_temp;
  my->y += dy_temp;
  if (my->mass != NULL)
    tenm_move_mass(my->mass, dx_temp, dy_temp);

  /* don't use in_window_object() here because we want to treat
   * this as a rectangle */
  if (my->x < 1.0 - 25.0)
  {
    dx_temp = ((double) WINDOW_WIDTH) - 1.0 + 25.0 - my->x;
    my->x += dx_temp;
    if (my->mass != NULL)
      tenm_move_mass(my->mass, dx_temp, 0.0);
  }
  else if (my->x >= ((double) WINDOW_WIDTH) - 1.0 + 25.0)
  {
    dx_temp = 1.0 - 25.0 - my->x;
    my->x += dx_temp;
    if (my->mass != NULL)
      tenm_move_mass(my->mass, dx_temp, 0.0);
  }
  if (my->y >= ((double) WINDOW_HEIGHT) - 1.0 + 25.0)
  {
    dy_temp = 1.0 - 25.0 - my->y;
    my->y += dy_temp;
    if (my->mass != NULL)
      tenm_move_mass(my->mass, 0.0, dy_temp);
    my->count_d[0] *= -1.0;
  }
  
  return 0;
}

static int
fud_core_hit(tenm_object *my, tenm_object *your)
{
  int hit_point = FUD_HIT_POINT;
  int score = 10000;

  /* sanity check */
  if (my == NULL)
    return 0;
  if (your == NULL)
    return 0;

  if (your->attr != ATTR_PLAYER_SHOT)
    return 0;

  my->hit_point -= add_damage_score(my->hit_point, your->hit_point);
  my->count[6] = 1;
  if (my->count[3] < 0)
  {
    hit_point = FUD_HIT_POINT;
    score = 10000;
  }
  else
  {
    hit_point = TRINITY_HIT_POINT;
    score = 20000;
  }
  
  set_hit_status(my->name, hit_point, my->hit_point);
  if (my->hit_point <= 0)
  {
    tenm_table_add(explosion_new(my->x, my->y, my->count_d[0] / 2.0, 1.5,
                                 2, 200, my->count[0], 4.0, 16));
    add_killed(my->count[0]);
    add_score(score);
    tenm_table_apply(my->count[1],
                     (int (*)(tenm_object *, int)) fud_core_signal,
                     my->table_index);
    return 1;
  }
  return 0;
}

static int
fud_core_act(tenm_object *my, const tenm_object *player)
{
  double temp_speed;

  /* sanity check */
  if (my == NULL)
    return 0;
  if (player == NULL)
    return 0;
  
  if ((my->count_d[0] >= -7.0) && (my->count_d[0] <= 7.0))
    (my->count[2])++;
  else
    my->count[2] += 2;
  if (my->count[2] >= 30)
  {
    if (my->count_d[0] >= 0.0)
      temp_speed = 1.5 + my->count_d[0];
    else
      temp_speed = 1.5 - my->count_d[0];
    tenm_table_add(laser_point_new(my->x, my->y, temp_speed,
                                   player->x, player->y,
                                   20.0, my->count[0], 1000));
    my->count[2] = 0;
  }
  if (my->count[3] < 0)
    return 0;

  (my->count[3])++;
  if (my->count[4] == 0)
  {
    if (my->count[3] >= 75)
    {
      my->count[3] = 0;
      my->count[4] = -1;
    }
  }
  else
  {
    if (my->count[3] >= 25)
    {
      my->count[3] = 0;
      if (my->count[4] == -2)
        my->count[4] = -1;
      else if (my->count[4] == -1)
        my->count[4] = 2;
      else if (my->count[4] == 1)
        my->count[4] = -2;
      else if (my->count[4] == 2)
        my->count[4] = 1;
      else
        my->count[4] = -1;
    }
  }
  
  if ((my->count[4] == 1) || (my->count[4] == -1))
    tenm_table_add(laser_new(my->x, my->y, my->count_d[0], 3.0,
                             0.0, (double) (1000 * my->count[4]),
                             my->count[0], 1));
  return 0;
}

static int
fud_core_draw(tenm_object *my, int priority)
{
  int n = 0;
  tenm_color color;

  /* sanity check */
  if (my == NULL)
    return 0;
  if (priority != 0)
    return 0;

  n = my->count[5];
  if (my->count[6] != 0)
    n += 4;

  if (fud_core_blit((int) (my->x), (int) (my->y), n) != 0)
  {
    fprintf(stderr, "fud_core_draw: fud_core_blit failed\n");
    my->count[6] = 0;
    return 1;
  }
  my->count[6] = 0;
  return 0;
  /* the below may be useful for debugging */

  switch (my->count[0])
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
  if (tenm_draw_primitive(my->mass->p[0], color) != 0)
    return 1;
  return 0;
}

static int
fud_core_signal(tenm_object *my, int n)
{
  int i;
  int j;
  int all_destroyed = 1;
  double temp_x;
  int temp_index[4];

  /* sanity check */
  if (my == NULL)
    return 0;
  if (strcmp(my->name, "FUD manager") != 0)
    return 0;

  for (i = 0; i < 4; i++)
    if (my->count[i + 1] == n)
      my->count[i + 1] = -1;
    else if (my->count[i + 1] >= 0)
      tenm_table_apply(my->count[i + 1],
                       (int (*)(tenm_object *, int)) fud_core_speed_up,
                       my->table_index);

  for (i = 0; i < 4; i++)
    if (my->count[i + 1] >= 0)
      all_destroyed = 0;

  if (all_destroyed)
  {
    if (my->count[0] <= 0)
    {
      /* Trinity appears */
      temp_x = (double) ((WINDOW_WIDTH + 50 * 2) * 1 / 3);
      my->count[4] = tenm_table_add(fud_core_new(my->table_index, 3,
                                                 temp_x, -24.0));
      my->count[0] = 1;
      return 0;
    }
    else
    {
      my->count[0] = 2;
      /* Trinity is dead --- destroy walls */
      tenm_table_apply_all((int (*)(tenm_object *, int)) delete_enemy_shot, 0);
      for (i = 5; i <= 16; i++)
        tenm_table_apply(my->count[i],
                         (int (*)(tenm_object *, int)) fud_wall_signal,
                         150);
      for (i = 0; i < 4; i++)
      {
        temp_index[i] = 5 + rand() % 12;
        for (j = 0; j < i; j++)
          if (temp_index[j] == temp_index[i])
          {
            i--;
            break;
          }
      }
      tenm_table_apply(my->count[temp_index[0]],
                       (int (*)(tenm_object *, int)) fud_wall_signal,
                       10);
      tenm_table_apply(my->count[temp_index[1]],
                       (int (*)(tenm_object *, int)) fud_wall_signal,
                       30);
      for (i = 2; i <= 3; i++)
        tenm_table_apply(my->count[temp_index[i]],
                         (int (*)(tenm_object *, int)) fud_wall_signal,
                         75);
    }
  }

  return 0;
}

static int
fud_core_speed_up(tenm_object *my, int n)
{
  /* sanity check */
  if (my == NULL)
    return 0;
  if ((strcmp(my->name, "Fear") != 0)
      && (strcmp(my->name, "Uncertainty") != 0)
      && (strcmp(my->name, "Doubt") != 0))
    return 0;

  my->count_d[0] *= 2.0;

  return 0;
}

static tenm_object *
fud_wall_new(int n, double x, double y)
{
  tenm_primitive **p = NULL;
  tenm_object *new = NULL;
  int *count = NULL;
  double *count_d = NULL;

  /* sanity check */
  if (n < 0)
    return NULL;

  p = (tenm_primitive **) malloc(sizeof(tenm_primitive *) * 1);
  if (p == NULL)
  {
    fprintf(stderr, "fud_wall_new: malloc(p) failed\n");
    return NULL;
  }

  if (n % 2 == 0)
    p[0] = (tenm_primitive *) tenm_polygon_new(3,
                                               x - WALL_SIZE, y - WALL_SIZE,
                                               x - WALL_SIZE, y + WALL_SIZE,
                                               x + WALL_SIZE, y + WALL_SIZE);
  else
    p[0] = (tenm_primitive *) tenm_polygon_new(3,
                                               x + WALL_SIZE, y - WALL_SIZE,
                                               x + WALL_SIZE, y + WALL_SIZE,
                                               x - WALL_SIZE, y + WALL_SIZE);
  if (p[0] == NULL)
  {
    fprintf(stderr, "fud_wall_new: cannot set p[0]\n");
    free(p);
    return NULL;
  }

  count = (int *) malloc(sizeof(int) * 1);
  if (count == NULL)
  {
    fprintf(stderr, "fud_wall_new: malloc(count) failed\n");
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }

  count_d = (double *) malloc(sizeof(double) * 1);
  if (count_d == NULL)
  {
    fprintf(stderr, "fud_wall_new: malloc(count_d) failed\n");
    (p[0])->delete(p[0]);
    free(p);
    if (count != NULL)
      free(count);
    return NULL;
  }

  /* list of count
   * [0] explosion timer
   */
  /* list of count_d
   * [0] speed x
   */
  count[0] = -1;

  if (n <= 1)
    count_d[0] = 4.0;
  else
    count_d[0] = -4.0;

  new = tenm_object_new("wall", ATTR_OBSTACLE, 0,
                        100, x, y,
                        1, count, 1, count_d, 1, p, 
                        (int (*)(tenm_object *, double)) fud_wall_move,
                        (int (*)(tenm_object *, tenm_object *)) NULL,
                        (int (*)(tenm_object *, const tenm_object *))
                        fud_wall_act,
                        (int (*)(tenm_object *, int)) fud_wall_draw);
  if (new == NULL)
  {
    fprintf(stderr, "fud_wall_new: tenm_object_new failed\n");
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
fud_wall_move(tenm_object *my, double turn_per_frame)
{
  double dx_temp;
  double dy_temp;

  /* sanity check */
  if (my == NULL)
    return 0;
  if (turn_per_frame <= 0.5)
    return 0;

  dx_temp = my->count_d[0] / turn_per_frame;
  if (my->count[0] < 0)
    dy_temp = 3.0 / turn_per_frame;
  else
    dy_temp = 1.0 / turn_per_frame;
  my->x += dx_temp;
  my->y += dy_temp;
  if (my->mass != NULL)
    tenm_move_mass(my->mass, dx_temp, dy_temp);

  /* don't use in_window_object() here because we want to treat
   * this as a rectangle */
  if (my->x < 0.0 - WALL_SIZE)
  {
    dx_temp = ((double) WINDOW_WIDTH) - 1.0 + WALL_SIZE - my->x;
    my->x += dx_temp;
    if (my->mass != NULL)
      tenm_move_mass(my->mass, dx_temp, 0.0);
  }
  else if (my->x >= WALL_SIZE + (double) WINDOW_WIDTH)
  {
    dx_temp = 1.0 - WALL_SIZE - my->x;
    my->x += dx_temp;
    if (my->mass != NULL)
      tenm_move_mass(my->mass, dx_temp, 0.0);
  }
  if (my->y >= WALL_SIZE + (double) WINDOW_HEIGHT)
  {
    dy_temp = 1.0 - WALL_SIZE - my->y;
    my->y += dy_temp;
    if (my->mass != NULL)
      tenm_move_mass(my->mass, 0.0, dy_temp);
    /* don't change my->count_d[0] here */
  }
  
  return 0;
}

static int
fud_wall_act(tenm_object *my, const tenm_object *player)
{
  /* sanity check */
  if (my == NULL)
    return 0;

  if (my->count[0] < 0)
    return 0;
  (my->count[0])--;
  if (my->count[0] <= 0)
  {
    tenm_table_add(explosion_new(my->x, my->y, 0.0, 0.0,
                                 1, 5000, COLOR_GREEN, 8.0, 12));
    tenm_table_add(explosion_new(my->x, my->y, 0.0, 0.0,
                                 2, 1000, COLOR_GREEN, 5.0, 12));
    tenm_table_add(explosion_new(my->x, my->y, 0.0, 0.0,
                                 3, 200, COLOR_GREEN, 3.5, 12));
    return 1;
  }
  return 0;
}

static int
fud_wall_draw(tenm_object *my, int priority)
{
  /* sanity check */
  if (my == NULL)
    return 0;
  /* dead enemy has low priority */
  if (my->count[0] < 0)
  {
    if (priority != 0)
      return 0;
  }
  else
  {
    if (priority != -1)
      return 0;
  }

  if (my->mass == NULL)
    return 0;
  if (my->mass->p[0] == NULL)
    return 0;

  if (tenm_draw_primitive(my->mass->p[0], tenm_map_color(59, 156, 71)) != 0)
    return 1;
  return 0;
}
