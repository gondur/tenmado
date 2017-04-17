/* $Id: disconnection.c,v 1.152 2003/08/22 05:03:44 oohara Exp $ */

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
#include "laser.h"
#include "tenm_math.h"
#include "image.h"

#include "disconnection.h"

#define DISCONNECTION_CENTER_X ((double) WINDOW_WIDTH) / 2.0
#define DISCONNECTION_CENTER_Y 130.0
#define DISCONNECTION_CORE_HIT_POINT 750

static int disconnection_act(tenm_object *my, const tenm_object *player);
static int disconnection_signal_to_shroud(tenm_object *my, int n);
static int disconnection_signal_to_bit(tenm_object *my, int n);

static tenm_object *disconnection_core_new(int parent_index);
static int disconnection_core_move(tenm_object *my, double turn_per_frame);
static int disconnection_core_hit(tenm_object *my, tenm_object *your);
static int disconnection_core_signal(tenm_object *my, int n);
static int disconnection_core_act(tenm_object *my, const tenm_object *player);
static int disconnection_core_draw(tenm_object *my, int priority);

static tenm_object *disconnection_shroud_new(int parent_index, int n);
static int disconnection_shroud_hit_point(int n);
static int disconnection_shroud_move(tenm_object *my, double turn_per_frame);
static int disconnection_shroud_hit(tenm_object *my, tenm_object *your);
static int disconnection_shroud_explosion(const tenm_object *my);
static int disconnection_shroud_signal(tenm_object *my, int n);
static int disconnection_shroud_act(tenm_object *my,
                                    const tenm_object *player);
static int disconnection_shroud_draw(tenm_object *my, int priority);

static tenm_object * disconnection_bit_new(int parent_index, int n);
static int disconnection_bit_move(tenm_object *my, double turn_per_frame);
static int disconnection_bit_act(tenm_object *my, const tenm_object *player);
static int disconnection_bit_draw(tenm_object *my, int priority);

tenm_object *
disconnection_new(void)
{
  int i;
  tenm_object *new = NULL;
  int *count = NULL;

  count = (int *) malloc(sizeof(int) * 7);
  if (count == NULL)
  {
    fprintf(stderr, "disconnection_new: malloc(count) failed\n");
    return NULL;
  }
  /* list of count
   * [0] timer
   * [1] mode
   * [2 -- 3] bit index
   * [4 -- 6] shroud index
   */
  count[0] = 0;
  count[1] = 0;
  for (i = 2; i <= 6; i++)
    count[i] = -1;

  new = tenm_object_new("Disconnection manager", 0, 0,
                        0, 0.0, 0.0,
                        7, count, 0, NULL, 0, NULL,
                        (int (*)(tenm_object *, double)) NULL,
                        (int (*)(tenm_object *, tenm_object *)) NULL,
                        (int (*)(tenm_object *, const tenm_object *))
                        disconnection_act,
                        (int (*)(tenm_object *, int)) NULL);
  if (new == NULL)
  {
    fprintf(stderr, "disconnection_new: tenm_object_new failed\n");
    free(count);
    return NULL;
  }

  return new;
}

static int
disconnection_act(tenm_object *my, const tenm_object *player)
{
  int i;

  /* sanity check */
  if (my == NULL)
    return 0;

  if (my->count[1] == 0)
  {
    if (my->count[0] == 0)
    {
      for (i = 0; i < 2; i++)
      {
        my->count[i + 2] =
          tenm_table_add(disconnection_bit_new(my->table_index, i));
        if (my->count[i + 2] < 0)
          fprintf(stderr, "disconnection_act: cannot create bit (%d)  "
                  "(%d, %d)\n", i, my->count[1], my->count[0]);
      }
    }
    else if (my->count[0] == 100)
    {
      for (i = 0; i < 3; i++)
      {
        my->count[i + 4] =
          tenm_table_add(disconnection_shroud_new(my->table_index, i));
        if (my->count[i + 4] < 0)
          fprintf(stderr, "disconnection_act: cannot create shroud (%d)  "
                  "(%d, %d)\n", i, my->count[1], my->count[0]);
      }
      if (tenm_table_add(disconnection_core_new(my->table_index)) < 0)
        fprintf(stderr, "disconnection_act: cannot create core "
                "(%d, %d)\n", my->count[1], my->count[0]);
      my->count[1] = 1;
    }
    (my->count[0])++;
  }

  return 0;
}

static int
disconnection_signal_to_shroud(tenm_object *my, int n)
{
  /* sanity check */
  if (my == NULL)
    return 0;
  if ((strcmp(my->name, "Disconnection front shroud") != 0)
      && (strcmp(my->name, "Disconnection side shroud") != 0))
    return 0;

  /* don't modify my->attr or my->hit_mask here, or the player shot
   * may fly through the enemy */
  if (my->mass != NULL)
    tenm_mass_delete(my->mass);
  my->mass = NULL;
  my->count[1] = 0;
  my->count[4] = 2;

  return 0;
}

static int
disconnection_signal_to_bit(tenm_object *my, int n)
{
  /* sanity check */
  if (my == NULL)
    return 0;
  if (strcmp(my->name, "Disconnection bit") != 0)
    return 0;

  /* don't modify my->attr or my->hit_mask here, or the player shot
   * may fly through the enemy */
  if (my->mass != NULL)
    tenm_mass_delete(my->mass);
  my->mass = NULL;
  my->count[1] = 0;
  my->count[2] = 4;
  my->count_d[0] = 0.0;
  my->count_d[1] = 0.0;

  return 0;
}

static tenm_object *
disconnection_core_new(int parent_index)
{
  tenm_primitive **p = NULL;
  tenm_object *new = NULL;
  int *count = NULL;
  double *count_d = NULL;
  int hit_point = DISCONNECTION_CORE_HIT_POINT;
  double x = DISCONNECTION_CENTER_X;
  double y = -123.0;

  p = (tenm_primitive **) malloc(sizeof(tenm_primitive *) * 1);
  if (p == NULL)
  {
    fprintf(stderr, "disconnection_core_new: malloc(p) failed\n");
    return NULL;
  }

  p[0] = (tenm_primitive *) tenm_circle_new(x, y, 35.0);
  if (p[0] == NULL)
  {
    fprintf(stderr, "disconnection_core_new: cannot set p[0]\n");
    free(p);
    return NULL;
  }

  count = (int *) malloc(sizeof(int) * 4);
  if (count == NULL)
  {
    fprintf(stderr, "disconnection_core_new: malloc(count) failed\n");
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }

  count_d = (double *) malloc(sizeof(double) * 2);
  if (count_d == NULL)
  {
    fprintf(stderr, "disconnection_core_new: malloc(count_d) failed\n");
    (p[0])->delete(p[0]);
    free(p);
    if (count != NULL)
      free(count);
    return NULL;
  }

  /* list of count
   * [0] table index of parent (Disconnection manager)
   * [1] timer
   * [2] mode
   * [3] "damaged" flag
   */
  /* list of count_d
   * [0] speed x
   * [1] speed y
   */

  count[0] = parent_index;
  count[1] = 0;
  count[2] = -1;
  count[3] = 0;

  count_d[0] = (DISCONNECTION_CENTER_X - x) / 50.0;
  count_d[1] = (DISCONNECTION_CENTER_Y - y) / 50.0;

  new = tenm_object_new("Disconnection", ATTR_ENEMY, ATTR_PLAYER_SHOT,
                        hit_point, x, y,
                        4, count, 2, count_d, 1, p,
                        (int (*)(tenm_object *, double))
                        disconnection_core_move,
                        (int (*)(tenm_object *, tenm_object *))
                        disconnection_core_hit,
                        (int (*)(tenm_object *, const tenm_object *))
                        disconnection_core_act,
                        (int (*)(tenm_object *, int)) disconnection_core_draw);
  if (new == NULL)
  {
    fprintf(stderr, "disconnection_core_new: tenm_object_new failed\n");
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
disconnection_core_move(tenm_object *my, double turn_per_frame)
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

  if ((my->count[2] != -1) && (my->mass != NULL) && (!in_window_object(my)))
  {
    fprintf(stderr, "disconnection_core_move: the object is out of "
            "the window\n");
    return 1;
  }

  return 0;
}

static int
disconnection_core_hit(tenm_object *my, tenm_object *your)
{
  /* sanity check */
  if (my == NULL)
    return 0;
  if (your == NULL)
    return 0;

  if (your->attr != ATTR_PLAYER_SHOT)
    return 0;

  /* immutable time */
  if (my->count[2] == -1)
    return 0;

  my->hit_point -= add_damage_score(my->hit_point, your->hit_point);
  my->count[3] = 1;
  set_hit_status(my->name, DISCONNECTION_CORE_HIT_POINT, my->hit_point);
  if (my->hit_point <= 0)
  {
    tenm_table_add(explosion_new(my->x, my->y, 0.0, 0.0,
                                 1, 2000, COLOR_PURPLE, 8.0, 12));
    tenm_table_add(explosion_new(my->x, my->y, 0.0, 0.0,
                                 2, 500, COLOR_PURPLE, 6.0, 12));
    add_killed(COLOR_PURPLE);
    add_score(20000);
    tenm_table_apply_all((int (*)(tenm_object *, int)) delete_enemy_shot, 0);
    /* don't modify my->attr or my->hit_mask here, or the player shot
     * may fly through the enemy */
    if (my->mass != NULL)
      tenm_mass_delete(my->mass);
    my->mass = NULL;
    my->count[1] = 0;
    my->count[2] = 1;
    my->count_d[0] = 0.0;
    my->count_d[1] = 0.5;
    tenm_table_apply(my->count[0],
                     (int (*)(tenm_object *, int))
                     disconnection_core_signal, 0);
    return 0;
  }
  
  return 0;
}

static int
disconnection_core_signal(tenm_object *my, int n)
{
  int i;

  /* sanity check */
  if (my == NULL)
    return 0;
  if (strcmp(my->name, "Disconnection manager") != 0)
    return 0;

  my->count[1] = 1;
  for (i = 0; i < 2; i++)
  {
    if (my->count[i + 2] >= 0)
    {
      tenm_table_apply(my->count[i + 2],
                       (int (*)(tenm_object *, int))
                       disconnection_signal_to_bit, 0);
    }
  }
  for (i = 0; i < 3; i++)
  {
    if (my->count[i + 4] >= 0)
    {
      tenm_table_apply(my->count[i + 4],
                       (int (*)(tenm_object *, int))
                       disconnection_signal_to_shroud, 0);
    }
  }


  return 0;
}

static int
disconnection_core_act(tenm_object *my, const tenm_object *player)
{
  double x_temp;
  double y_temp;
  double source_x;
  double source_y;
  int randomness;

  /* sanity check */
  if (my == NULL)
    return 0;
  if (player == NULL)
    return 0;

  (my->count[1])++;
  if (my->count[2] == -1)
  {
    if (my->count[1] == 50)
    {
      my->count_d[0] = 0.0;
      my->count_d[1] = 0.0;
    }
    else if (my->count[1] >= 150)
    {
      my->count[1] = 0;
      my->count[2] = 0;
    }    
  }
  else if (my->count[2] == 0)
  {
    if (my->count[1] >= 6)
    {
      randomness = 100;
      x_temp = (double) ((rand() % (randomness * 2 + 1)) - randomness);
      y_temp = (double) ((rand() % (randomness * 2 + 1)) - randomness);
      tenm_table_add(normal_shot_point_new(my->x, my->y, 9.0,
                                           player->x + x_temp,
                                           player->y + y_temp,
                                           COLOR_PURPLE, 1000));
      my->count[1] = 0;
    }
  }
  else if (my->count[2] == 1)
  {
    if ((my->count[1] <= 105) && (my->count[1] % 15 == 0))
    {
      source_x = my->x + (double) (rand() % 49 - 24);
      source_y = my->y + (double) (rand() % 49 - 24);
      tenm_table_add(explosion_new(source_x, source_y, 0.0, 0.0,
                                   1, 2000, COLOR_PURPLE, 6.0, 12));
      tenm_table_add(explosion_new(source_x, source_y, 0.0, 0.0,
                                   2, 500, COLOR_PURPLE, 4.0, 12));
    }
    else if (my->count[1] == 150)
    {
      tenm_table_add(explosion_new(my->x, my->y, 0.0, 0.0,
                                   1, 8000, COLOR_PURPLE, 10.0, 12));
      tenm_table_add(explosion_new(my->x, my->y, 0.0, 0.0,
                                   2, 2000, COLOR_PURPLE, 8.0, 12));
      tenm_table_add(explosion_new(my->x, my->y, 0.0, 0.0,
                                   3, 300, COLOR_PURPLE, 5.0, 12));
    }
    else if (my->count[1] >= 300)
    {
      tenm_table_add(stage_clear_new(3));
      return 1;
    }
  }
  else
  {
    fprintf(stderr, "disconnection_core_act: strange mode (%d)\n",
            my->count[2]);
  }

  return 0;
}

static int
disconnection_core_draw(tenm_object *my, int priority)
{
  int n = 0;

  /* sanity check */
  if (my == NULL)
    return 0;
  /* dead enemy has low priority */
  if (my->count[2] == 1)
  {
    if (priority != -1)
      return 0;
  }
  else
  {
    if (priority != 0)
      return 0;
  }

  if ((my->count[2] == 1) && (my->count[1] >= 150))
    return 0;

  if (my->count[3] == 0)
    n = 0;
  else
    n = 1;

  if (disconnection_core_blit((int) (my->x), (int) (my->y), n) != 0)
  {
    fprintf(stderr, "disconnection_core_draw: disconnection_core_blit "
            "failed\n");
    my->count[3] = 0;
    return 1;
  }
  my->count[3] = 0;
  return 0;
  /* the below may be useful for debugging */

  if (tenm_draw_mass(my->mass, tenm_map_color(78, 59, 156)) != 0)
  {
    fprintf(stderr, "disconnection_core_draw: tenm_draw_mass failed\n");
    return 1;
  }
  return 0;
}

static tenm_object *
disconnection_shroud_new(int parent_index, int n)
{
  tenm_primitive **p = NULL;
  tenm_object *new = NULL;
  int *count = NULL;
  double *count_d = NULL;
  const char *name = NULL;
  int hit_point = disconnection_shroud_hit_point(n);
  double x = DISCONNECTION_CENTER_X;
  double y = DISCONNECTION_CENTER_Y;

  /* sanity check */
  if ((n < 0) || (n > 2))
  {
    fprintf(stderr, "disconnection_shroud_new: strange n (%d)\n", n);
    return NULL;
  }

  if (n == 0)
  {
    x = DISCONNECTION_CENTER_X - 103.9230;
    y = -183.0;
  }
  else if (n == 1)
  {
    x = DISCONNECTION_CENTER_X;
    y = -3.0;
  }
  else
  {
    x = DISCONNECTION_CENTER_X + 103.9230;
    y = -183.0;
  }
  
  p = (tenm_primitive **) malloc(sizeof(tenm_primitive *) * 1);
  if (p == NULL)
  {
    fprintf(stderr, "disconnection_shroud_new: malloc(p) failed\n");
    return NULL;
  }

  if (n == 0)
    p[0] = (tenm_primitive *) tenm_polygon_new(4,
                                               x + 27.0, y - 54.0,
                                               x - 33.0, y + 50.0,
                                               x - 26.0, y + 54.0,
                                               x + 34.0, y - 50.0);
  else if (n == 1)
    p[0] = (tenm_primitive *) tenm_polygon_new(4,
                                               x + 60.0, y + 4.0,
                                               x - 60.0, y + 4.0,
                                               x - 60.0, y - 4.0,
                                               x + 60.0, y - 4.0);
  else
    p[0] = (tenm_primitive *) tenm_polygon_new(4,
                                               x - 27.0, y - 54.0,
                                               x + 33.0, y + 50.0,
                                               x + 26.0, y + 54.0,
                                               x - 34.0, y - 50.0);
  if (p[0] == NULL)
  {
    fprintf(stderr, "disconnection_shroud_new: cannot set p[0]\n");
    free(p);
    return NULL;
  }

  count = (int *) malloc(sizeof(int) * 6);
  if (count == NULL)
  {
    fprintf(stderr, "disconnection_shroud_new: malloc(count) failed\n");
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }

  count_d = (double *) malloc(sizeof(double) * 4);
  if (count_d == NULL)
  {
    fprintf(stderr, "disconnection_shroud_new: malloc(count_d) failed\n");
    (p[0])->delete(p[0]);
    free(p);
    if (count != NULL)
      free(count);
    return NULL;
  }

  if (n == 1)
    name = "Disconnection front shroud";
  else
    name = "Disconnection side shroud";

  /* list of count
   * [0] table index of parent (Disconnection manager)
   * [1] timer
   * [2] color
   * [3] n
   * [4] mode
   * [5] "damaged" flag
   */
  /* list of count_d
   * [0] speed x
   * [1] speed y
   * [2] center x
   * [3] center y
   */

  count[0] = parent_index;
  count[1] = 0;
  if (n == 0)
    count[2] = COLOR_BLUE;
  else if (n == 1)
    count[2] = COLOR_PURPLE;
  else
    count[2] = COLOR_RED;
  count[3] = n;
  count[4] = 0;
  count[5] = 0;

  count_d[0] = 0.0;
  if (n == 1)
    count_d[1] = ((DISCONNECTION_CENTER_Y + 120.0) - y) / 50.0;
  else
    count_d[1] = ((DISCONNECTION_CENTER_Y - 60.0) - y) / 50.0;
  count_d[2] = DISCONNECTION_CENTER_X;
  count_d[3] = DISCONNECTION_CENTER_Y;

  new = tenm_object_new(name,
                        ATTR_ENEMY | ATTR_OPAQUE, ATTR_PLAYER_SHOT,
                        hit_point, x, y,
                        6, count, 4, count_d, 1, p,
                        (int (*)(tenm_object *, double))
                        disconnection_shroud_move,
                        (int (*)(tenm_object *, tenm_object *))
                        disconnection_shroud_hit,
                        (int (*)(tenm_object *, const tenm_object *))
                        disconnection_shroud_act,
                        (int (*)(tenm_object *, int))
                        disconnection_shroud_draw);
  if (new == NULL)
  {
    fprintf(stderr, "disconnection_shroud_new: tenm_object_new failed\n");
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
disconnection_shroud_hit_point(int n)
{
  if (n == 1)
    return 750;
  return 500;
}

static int
disconnection_shroud_move(tenm_object *my, double turn_per_frame)
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

  if ((my->count[4] != 0) && (my->mass != NULL) && (!in_window_object(my)))
  {
    fprintf(stderr, "disconnection_shroud_move: the object is out of "
            "the window\n");
    return 1;
  }

  return 0;
}

static int
disconnection_shroud_hit(tenm_object *my, tenm_object *your)
{
  /* sanity check */
  if (my == NULL)
    return 0;
  if (your == NULL)
    return 0;

  if (your->attr != ATTR_PLAYER_SHOT)
    return 0;

  /* immutable time */
  if (my->count[4] == 0)
    return 0;

  my->hit_point -= add_damage_score(my->hit_point, your->hit_point);
  my->count[5] = 1;
  set_hit_status(my->name, disconnection_shroud_hit_point(my->count[3]),
                 my->hit_point);
  if (my->hit_point <= 0)
  {
    add_killed(my->count[2]);
    add_score(10000);
    disconnection_shroud_explosion(my);
    tenm_table_apply(my->count[0],
                     (int (*)(tenm_object *, int))
                     disconnection_shroud_signal, my->count[3]);
    return 1;
  }

  return 0;
}

static int
disconnection_shroud_explosion(const tenm_object *my)
{
  tenm_table_add(explosion_new(my->x, my->y, 0.0, 0.0,
                               1, 2000, my->count[2], 8.0, 12));
  tenm_table_add(explosion_new(my->x, my->y, 0.0, 0.0,
                               2, 500, my->count[2], 6.0, 12));
  return 0;
}

static int
disconnection_shroud_signal(tenm_object *my, int n)
{
  /* sanity check */
  if (my == NULL)
    return 0;
  if (strcmp(my->name, "Disconnection manager") != 0)
    return 0;

  my->count[n + 4] = -1;

  return 0;
}

static int
disconnection_shroud_act(tenm_object *my, const tenm_object *player)
{
  /* for rotate move */
  double r_x;
  double r_y;
  double r_length;
  double dr_x;
  double dr_y;
  double temp;
  double speed_r;
  double speed_theta = 0.45;

  /* sanity check */
  if (my == NULL)
    return 0;
  if (player == NULL)
    return 0;

  if (my->count[4] == 0)
  {
    (my->count[1])++;
    if (my->count[1] == 50)
    {
      my->count_d[0] = 0.0;
      my->count_d[1] = 0.0;
    }
    else if (my->count[1] >= 100)
    {
      my->count[4] = 1;
    }
    return 0;
  }
  else if (my->count[4] == 2)
  {
    speed_theta *= 0.1;
    (my->count[1])++;
    if (my->count[1] >= 10)
    {
      disconnection_shroud_explosion(my);
      return 1;
    }
  }

  /* rotate */
  r_x = my->x - my->count_d[2];
  r_y = my->y - my->count_d[3];
  r_length = tenm_sqrt((int) (r_x * r_x + r_y * r_y));
  /* ugly hack to prevent radius from increasing */
  speed_r = 120.0 - r_length;

  r_x /= r_length;
  r_y /= r_length;

  dr_x = -r_y;
  dr_y = r_x;

  temp = r_length / 5.0;
  my->count_d[0] = speed_r * r_x + speed_theta * dr_x * temp;
  my->count_d[1] = speed_r * r_y + speed_theta * dr_y * temp;

  return 0;
}

static int
disconnection_shroud_draw(tenm_object *my, int priority)
{
  tenm_color color;

  /* sanity check */
  if (my == NULL)
    return 0;
  /* dead enemy has low priority */
  if (my->count[4] == 2)
  {
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
    if (my->count[5] == 0)
      color = tenm_map_color(59, 112, 156);
    else
      color = tenm_map_color(119, 137, 157);
    break;
  case COLOR_PURPLE:
    if (my->count[5] == 0)
      color = tenm_map_color(78, 59, 156);
    else
      color = tenm_map_color(130, 100, 157);
    break;
  case COLOR_RED:
    if (my->count[5] == 0)
      color = tenm_map_color(156, 59, 72);
    else
      color = tenm_map_color(186, 100, 74);
    break;
  case COLOR_GREEN:
    color = tenm_map_color(59, 156, 71);
    break;
  default:
    color = tenm_map_color(0, 0, 0);
    break;
  }

  if ((my->mass != NULL) && (tenm_draw_mass(my->mass, color) != 0))
  {
    fprintf(stderr, "disconnection_shroud_draw: tenm_draw_mass failed\n");
    my->count[5] = 0;
    return 1;
  }
  my->count[5] = 0;
  return 0;
}

static tenm_object *
disconnection_bit_new(int parent_index, int n)
{
  tenm_primitive **p = NULL;
  tenm_object *new = NULL;
  int *count = NULL;
  double *count_d = NULL;
  double x = DISCONNECTION_CENTER_X;
  double y = DISCONNECTION_CENTER_Y;

  /* sanity check */
  if ((n < 0) || (n > 1))
  {
    fprintf(stderr, "disconnection_bit_new: strange n (%d)\n", n);
    return NULL;
  }

  if (n == 0)
    x = DISCONNECTION_CENTER_X - 190.0;
  else
    x = DISCONNECTION_CENTER_X + 190.0;
  y = -24.0;

  p = (tenm_primitive **) malloc(sizeof(tenm_primitive *) * 1);
  if (p == NULL)
  {
    fprintf(stderr, "disconnection_bit_new: malloc(p) failed\n");
    return NULL;
  }

  p[0] = (tenm_primitive *) tenm_circle_new(x, y, 25.0);

  if (p[0] == NULL)
  {
    fprintf(stderr, "disconnection_bit_new: cannot set p[0]\n");
    free(p);
    return NULL;
  }

  count = (int *) malloc(sizeof(int) * 5);
  if (count == NULL)
  {
    fprintf(stderr, "disconnection_bit_new: malloc(count) failed\n");
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }

  count_d = (double *) malloc(sizeof(double) * 2);
  if (count_d == NULL)
  {
    fprintf(stderr, "disconnection_bit_new: malloc(count_d) failed\n");
    (p[0])->delete(p[0]);
    free(p);
    if (count != NULL)
      free(count);
    return NULL;
  }

  /* list of count
   * [0] table index of parent (Disconnection manager)
   * [1] mode timer
   * [2] mode
   * [3] n
   * [4] normal shot timer
   */
  /* list of count_d
   * [0] speed x
   * [1] speed y
   */

  count[0] = parent_index;
  count[1] = 0;
  count[2] = -1;
  count[3] = n;
  if (n == 0)
    count[4] = 0;
  else
    count[4] = 20;

  count_d[0] = 0.0;
  count_d[1] = ((DISCONNECTION_CENTER_Y + 180.0) - y) / 60.0;

  new = tenm_object_new("Disconnection bit", ATTR_OBSTACLE, 0,
                        1, x, y,
                        5, count, 2, count_d, 1, p,
                        (int (*)(tenm_object *, double))
                        disconnection_bit_move,
                        (int (*)(tenm_object *, tenm_object *)) NULL,
                        (int (*)(tenm_object *, const tenm_object *))
                        disconnection_bit_act,
                        (int (*)(tenm_object *, int)) disconnection_bit_draw);
  if (new == NULL)
  {
    fprintf(stderr, "disconnection_bit_new: tenm_object_new failed\n");
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
disconnection_bit_move(tenm_object *my, double turn_per_frame)
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
    fprintf(stderr, "disconnection_bit_move: the object is out of "
            "the window\n");
    return 1;
  }

  return 0;
}

static int
disconnection_bit_act(tenm_object *my, const tenm_object *player)
{
  int i;
  double x_temp;
  double y_temp;
  double target_x;
  double target_y;
  int randomness;
  int theta;
  /* for rotate move */
  double r_x;
  double r_y;
  double r_length;
  double dr_x;
  double dr_y;
  double temp;
  double speed_r;
  double speed_theta;

  /* sanity check */
  if (my == NULL)
    return 0;
  if (player == NULL)
    return 0;

  (my->count[1])++;

  if (my->count[2] == 4)
  {
    if (((my->count[3] == 0) && (my->count[1] >= 30)) || (my->count[1] >= 70))
    {
      tenm_table_add(explosion_new(my->x, my->y, 0.0, 0.0,
                                   1, 2000, COLOR_GREEN, 8.0, 12));
      tenm_table_add(explosion_new(my->x, my->y, 0.0, 0.0,
                                   2, 500, COLOR_GREEN, 6.0, 12));
      return 1;
    }
    return 0;
  }
  else if (my->hit_point <= 0)
  {
    my->count[1] = 0;
    my->count[2] = 4;
    return 0;
  }
  else if (my->count[2] == -1)
  {
    if (my->count[1] == 60)
    {
      my->count_d[0] = 0.0;
      my->count_d[1] = 0.0;
    }
    else if (my->count[1] >= 320)
    {
      my->count[1] = 0;
      my->count[2] = 0;
    }
    return 0;
  }
  
  if (my->count[2] == 0)
  {    
    if (my->count[1] == 50)
    {
      if (my->count[3] == 0)
        my->count_d[0] = ((DISCONNECTION_CENTER_X - 100.0) - my->x) / 50.0;
      else
        my->count_d[0] = ((DISCONNECTION_CENTER_X + 100.0) - my->x) / 50.0;
      my->count_d[1] = ((DISCONNECTION_CENTER_Y + 210.0) - my->y) / 50.0;
    }
    else if (my->count[1] == 100)
    {
      my->count_d[0] = 0.0;
      my->count_d[1] = 0.0;
    }
    else if ((my->count[1] >= 120) && (my->count[1] <= 270)
             && (my->count[1] % 5 == 0))
    {
      for (i = 0; i < 2; i++)
      {
        if (i == 0)
        {  
          if (my->count[3] == 0)
            target_x = (double) (WINDOW_WIDTH / 2 - (my->count[1] - 100));
          else
            target_x = (double) (WINDOW_WIDTH / 2 + (my->count[1] - 100));
          target_y = (double) WINDOW_HEIGHT;
        }
        else
        {
          if (my->count[3] == 0)
            target_x = (double) WINDOW_WIDTH;
          else
            target_x = 0.0;
          target_y = my->y + (double) (my->count[1] - 100);
        }
        tenm_table_add(laser_point_new(my->x, my->y, 15.0,
                                       target_x, target_y,
                                       20.0, COLOR_GREEN, 1000));
      }
    }
    else if (my->count[1] == 290)
    {
      if (my->count[3] == 0)
        my->count_d[0] = ((DISCONNECTION_CENTER_X - 190.0) - my->x) / 50.0;
      else
        my->count_d[0] = ((DISCONNECTION_CENTER_X + 190.0) - my->x) / 50.0;
      my->count_d[1] = ((DISCONNECTION_CENTER_Y + 180.0) - my->y) / 50.0;
    }
    else if (my->count[1] >= 340)
    {
      my->count_d[0] = 0.0;
      my->count_d[1] = 0.0;
      my->count[1] = 0;
      my->count[2] = 1;
    }
  }
  else if (my->count[2] == 1)
  {
    if (my->count[1] == 50)
    {
      my->count_d[1] = 5.0;
    }
    else if (my->count[1] == 100)
    { 
      my->count_d[1] = 0.0;
    }
    else if (my->count[1] == 110)
    {
      my->count_d[1] = -5.0;
    }
    else if (my->count[1] == 160)
    {
      my->count_d[1] = 0.0;
      my->count[1] = 0;
      my->count[2] = 2;
    }
    
    if ((my->count[1] >= 55) && (my->count[1] <= 95)
        && (my->count[1] % 10 == 5))
    {
      if (my->count[3] == 0)
        theta = my->count[1] - 30;
      else
        theta = 180 - (my->count[1] - 30);
      tenm_table_add(laser_angle_new(my->x, my->y, 3.0, theta,
                                     20.0, COLOR_GREEN, 1000));
    }
    else if ((my->count[1] >= 115) && (my->count[1] <= 155)
        && (my->count[1] % 10 == 5))
    {
      if (my->count[3] == 0)
        theta = -(my->count[1] - 140);
      else
        theta = -(180 - (my->count[1] - 140));
      tenm_table_add(laser_angle_new(my->x, my->y, 8.5, theta,
                                     20.0, COLOR_GREEN, 1000));
    }
  }
  else if (my->count[2] == 2)
  {
    if (my->count[1] == 50)
    {
      if (my->count[3] == 0)
        my->count_d[0] = ((DISCONNECTION_CENTER_X - 150.0) - my->x) / 50.0;
      else
        my->count_d[0] = ((DISCONNECTION_CENTER_X + 150.0) - my->x) / 50.0;
      my->count_d[1] = ((DISCONNECTION_CENTER_Y + 370.0) - my->y) / 50.0;
    }
    else if (my->count[1] == 100)
    {
      my->count_d[0] = 0.0;
      my->count_d[1] = 0.0;
    }
    else if ((my->count[1] >= 120) && (my->count[1] < 400))
    {
      /* rotate */
      r_x = my->x - DISCONNECTION_CENTER_X;
      r_y = my->y - (DISCONNECTION_CENTER_Y + 370.0);
      r_length = tenm_sqrt((int) (r_x * r_x + r_y * r_y));
      /* ugly hack to prevent radius from increasing */
      speed_r = 150.0 - r_length;

      if (my->count[3] == 0)
        speed_theta = -0.15;
      else
        speed_theta = 0.15;

      r_x /= r_length;
      r_y /= r_length;

      dr_x = -r_y;
      dr_y = r_x;

      temp = r_length / 5.0;
      my->count_d[0] = speed_r * r_x + speed_theta * dr_x * temp;
      my->count_d[1] = speed_r * r_y + speed_theta * dr_y * temp;
    }
    else if (my->count[1] == 400)
    {
      my->count_d[0] = 0.0;
      my->count_d[1] = 0.0;
    }
    else if (my->count[1] == 420)
    {
      if (my->count[3] == 0)
        my->count_d[0] = ((DISCONNECTION_CENTER_X - 190.0) - my->x) / 50.0;
      else
        my->count_d[0] = ((DISCONNECTION_CENTER_X + 190.0) - my->x) / 50.0;
      my->count_d[1] = ((DISCONNECTION_CENTER_Y + 180.0) - my->y) / 50.0;
    }
    else if (my->count[1] >= 470)
    {
      my->count_d[0] = 0.0;
      my->count_d[1] = 0.0;
      my->count[1] = 0;
      my->count[2] = 0;
    }

    if ((my->count[1] >= 125) && (my->count[1] <= 385)
        && (my->count[1] % 15 == 5))
    {
      target_x = DISCONNECTION_CENTER_X;
      target_y = DISCONNECTION_CENTER_Y + 370.0;
      tenm_table_add(laser_point_new(my->x, my->y,
                                     ((double) (my->count[1])) * 0.03,
                                     target_x, target_y,
                                     20.0, COLOR_GREEN, 1000));
    }
    
  }

  (my->count[4])++;
  if (my->count[4] >= 40)
  {
    randomness = 50;
    x_temp = (double) ((rand() % (randomness * 2 + 1)) - randomness);
    y_temp = (double) ((rand() % (randomness * 2 + 1)) - randomness);
    tenm_table_add(normal_shot_point_new(my->x, my->y, 4.0,
                                         player->x + x_temp,
                                         player->y + y_temp,
                                         COLOR_GREEN, 1000));
    my->count[4] = 0;
  }

  return 0;
}

static int
disconnection_bit_draw(tenm_object *my, int priority)
{
  /* sanity check */
  if (my == NULL)
    return 0;

  /* dead enemy has low priority */
  if (my->count[2] == 4)
  {
    if (priority != -1)
      return 0;
  }
  else
  {    
    if (priority != 0)
      return 0;
  }

  if (disconnection_bit_blit((int) (my->x), (int) (my->y), 0) != 0)
  {
    fprintf(stderr, "disconnection_bit_draw: disconnection_bit_blit failed\n");
    return 1;
  }
  return 0;
  /* the below may be useful for debugging */

  if (tenm_draw_mass(my->mass, tenm_map_color(59, 156, 71)) != 0)
  {
    fprintf(stderr, "disconnection_bit_draw: tenm_draw_mass failed\n");
    return 1;
  }
  return 0;
}
