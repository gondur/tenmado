/* $Id: last-boss.c,v 1.311 2003/08/21 20:41:33 oohara Exp $ */

#include <stdio.h>
/* malloc, rand */
#include <stdlib.h>

#include "tenm_primitive.h"
#include "tenm_object.h"
#include "tenm_graphic.h"
#include "const.h"
#include "score.h"
#include "tenm_table.h"
#include "explosion.h"
#include "chain.h"
#include "hit-status.h"
#include "laser.h"
#include "normal-shot.h"
#include "util.h"
#include "stage-clear.h"
#include "tenm_math.h"
#include "rotate-laser.h"
#include "image.h"

#include "last-boss.h"

#define MODE_0_HIT_POINT 1000
#define MODE_1_HIT_POINT 500
#define MODE_2_HIT_POINT 1000
#define MODE_3_HIT_POINT 750

#define TIME_DEMO 150

#define NEAR_ZERO 0.0001

static int last_boss_move(tenm_object *my, double turn_per_frame);
static int last_boss_hit(tenm_object *my, tenm_object *your);
static int last_boss_act(tenm_object *my, const tenm_object *player);
static int last_boss_act_0(tenm_object *my, const tenm_object *player);
static int last_boss_act_1(tenm_object *my, const tenm_object *player);
static int last_boss_act_2(tenm_object *my, const tenm_object *player);
static int last_boss_act_3(tenm_object *my, const tenm_object *player);
static int last_boss_act_4(tenm_object *my, const tenm_object *player);
static int last_boss_draw(tenm_object *my, int priority);

static tenm_object *reminder_new(int x, int y, int shrink_speed,
                                 int color, int life);
static int reminder_act(tenm_object *my, const tenm_object *player);
static int reminder_draw(tenm_object *my, int priority);

static tenm_object *surprise_laser_new(int x, int y);
static int surprise_laser_act(tenm_object *my, const tenm_object *player);

static tenm_object *spread_shot_new(int x, int y, int life);
static int spread_shot_act(tenm_object *my, const tenm_object *player);

static tenm_object *rotate_shot_new(double x, double y,
                                    double speed_r, int theta,
                                    double speed_theta, int color, int life);
static int rotate_shot_move(tenm_object *my, double turn_per_frame);
static int rotate_shot_act(tenm_object *my, const tenm_object *player);
static int rotate_shot_draw(tenm_object *my, int priority);

static tenm_object *clock_shot_new(double x, double y, int theta,
                                   int clockwise);
static int clock_shot_move(tenm_object *my, double turn_per_frame);
static int clock_shot_act(tenm_object *my, const tenm_object *player);
static int clock_shot_draw(tenm_object *my, int priority);

tenm_object *last_boss_demo_new(double x, double y, int n);
static int last_boss_demo_act(tenm_object *my, const tenm_object *player);
static int last_boss_demo_draw(tenm_object *my, int priority);

tenm_object *
last_boss_new(void)
{
  tenm_primitive **p = NULL;
  tenm_object *new = NULL;
  int *count = NULL;
  double *count_d = NULL;
  double x = (double) (WINDOW_WIDTH / 2);
  double y = -59.0;

  p = (tenm_primitive **) malloc(sizeof(tenm_primitive *) * 1);
  if (p == NULL)
  {
    fprintf(stderr, "last_boss_new: malloc(p) failed\n");
    return NULL;
  }

  p[0] = (tenm_primitive *) tenm_circle_new(x, y, 60.0);
  if (p[0] == NULL)
  {
    fprintf(stderr, "last_boss_new: cannot set p[0]\n");
    free(p);
    return NULL;
  }

  count = (int *) malloc(sizeof(int) * 6);
  if (count == NULL)
  {
    fprintf(stderr, "last_boss_new: malloc(count) failed\n");
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }

  count_d = (double *) malloc(sizeof(double) * 3);
  if (count_d == NULL)
  {
    fprintf(stderr, "last_boss_new: malloc(count_d) failed\n");
    (p[0])->delete(p[0]);
    free(p);
    if (count != NULL)
      free(count);
    return NULL;
  }

  /* list of count
   * [0] mode
   * [1] shoot timer
   * [2] immutable timer
   * [3] rotate laser timer (for mode 1)
   * [4] demo timer
   * [5] "damaged" flag
   */
  count[0] = 0;
  count[1] = 48;
  count[2] = 3;
  count[3] = 0;
  count[4] = TIME_DEMO;
  count[5] = 0;
  /* list of count_d
  * [0] speed x
  * [1] speed y
  * [2] speed r (for mode 1)
  */
  count_d[0] = 0.0;
  count_d[1] = (100.0 - y) / ((double) (TIME_DEMO - 50));
  count_d[2] = 0.0;

  new = tenm_object_new("L", ATTR_ENEMY, ATTR_PLAYER_SHOT,
                        MODE_0_HIT_POINT, x, y,
                        6, count, 3, count_d, 1, p, 
                        (int (*)(tenm_object *, double)) last_boss_move,
                        (int (*)(tenm_object *, tenm_object *)) last_boss_hit,
                        (int (*)(tenm_object *, const tenm_object *))
                        last_boss_act,
                        (int (*)(tenm_object *, int)) last_boss_draw);
  if (new == NULL)
  {
    fprintf(stderr, "last_boss_new: tenm_object_new failed\n");
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
last_boss_move(tenm_object *my, double turn_per_frame)
{
  double dx_temp;
  double dy_temp;

  /* sanity check */
  if (my == NULL)
    return 0;
  if (turn_per_frame <= 0.5)
    return 0;

  if ((my->count[4] <= 0) && (my->count[2] > 0))
    return 0;

  dx_temp = my->count_d[0] / turn_per_frame;
  dy_temp = my->count_d[1] / turn_per_frame;
  my->x += dx_temp;
  my->y += dy_temp;
  if (my->mass != NULL)
    tenm_move_mass(my->mass, dx_temp, dy_temp);

  if ((my->mass != NULL) && (!in_window_object(my)))
  {
    fprintf(stderr, "last_boss_move: the object is out of the window\n");
    return 1;
  }

  return 0;
}

static int
last_boss_hit(tenm_object *my, tenm_object *your)
{
  int score = 10000;
  int hit_point = MODE_0_HIT_POINT;
  double time_move;

  /* sanity check */
  if (my == NULL)
    return 0;
  if (your == NULL)
    return 0;

  if (!(your->attr & ATTR_PLAYER_SHOT))
    return 0;

  /* immutable time */
  if (my->count[2] > 0)
    return 0;

  my->hit_point -= add_damage_score(my->hit_point, your->hit_point);
  my->count[5] = 1;

  if (my->count[0] == 0)
  {
    hit_point = MODE_0_HIT_POINT;
    score = 10000;
  }
  else if (my->count[0] == 1)
  {
    hit_point = MODE_1_HIT_POINT;
    score = 15000;
  }
  else if (my->count[0] == 2)
  {
    hit_point = MODE_2_HIT_POINT;
    score = 25000;
  }
  else if (my->count[0] == 3)
  {
    hit_point = MODE_3_HIT_POINT;
    score = 50000;
  }

  set_hit_status(my->name, hit_point, my->hit_point);
  if (my->hit_point <= 0)
  {
    tenm_table_add(explosion_new(my->x, my->y, 0.0, 0.0,
                                 1, 2000, COLOR_PURPLE, 15.0, 16));
    add_killed(COLOR_PURPLE);
    add_score(score);
    tenm_table_apply_all((int (*)(tenm_object *, int)) delete_enemy_shot, 0);
    time_move = ((double) (TIME_DEMO - 50));
    if (my->count[0] == 0)
    {
      my->hit_point = MODE_1_HIT_POINT;
      my->count[0] = 1;
      my->count[1] = 48;
      my->count[2] = 3;
      my->count[4] = TIME_DEMO;
      my->count[5] = 0;
      my->count_d[0] = (((double) (WINDOW_WIDTH / 2)) - my->x) / time_move;
      my->count_d[1] = (((double) (WINDOW_HEIGHT / 2))- 50.0- my->y)/time_move;
      return 0;
    }
    else if (my->count[0] == 1)
    {
      my->hit_point = MODE_2_HIT_POINT;
      my->count[0] = 2;
      my->count[1] = 48;
      my->count[2] = 3;
      my->count[4] = TIME_DEMO;
      my->count[5] = 0;
      my->count_d[0] = (((double) (WINDOW_WIDTH / 2)) - my->x) / time_move;
      my->count_d[1] = (100.0 - my->y) / time_move;
      return 0;
    }
    else if (my->count[0] == 2)
    {
      my->hit_point = MODE_3_HIT_POINT;
      my->count[0] = 3;
      my->count[1] = 64;
      my->count[2] = 4;
      my->count[4] = TIME_DEMO;
      my->count[5] = 0;
      my->count_d[0] = (((double) (WINDOW_WIDTH / 2)) - my->x) / time_move;
      my->count_d[1] = (100.0 - my->y) / time_move;
      return 0;
    }
    /* the boss is dead */
    /* don't modify my->attr or my->hit_mask here, or the player shot
     * may fly through the enemy */
    if (my->mass != NULL)
      tenm_mass_delete(my->mass);
    my->mass = NULL;
    my->count[0] = 4;
    my->count[1] = 0;
    my->count[5] = 0;
    my->count_d[0] = 0.0;
    my->count_d[1] = 0.5;
    return 0;
  }
  return 0;
}

static int
last_boss_act(tenm_object *my, const tenm_object *player)
{
  /* sanity check */
  if (my == NULL)
    return 0;
  if (player == NULL)
    return 0;

  if (my->count[4] > 0)
  {
    (my->count[4])--;
    if ((my->count[4] == TIME_DEMO - 1) && (my->count[0] != 4))
      tenm_table_add(last_boss_demo_new(100.0, 400.0, my->count[0]));
    if (my->count[4] == 50)
    {
      my->count_d[0] = 0.0;
      my->count_d[1] = 0.0;
    }
    if (my->count[4] > 0)
      return 0;

    if (my->count[0] == 0)
    {
      my->count_d[0] = 3.0;
      my->count_d[1] = 0.0;
    }
    else if (my->count[0] == 1)
    {
      my->count_d[0] = 0.0;
      my->count_d[1] = 0.0;
    }
    else if (my->count[0] == 2)
    {
      my->count_d[0] = 3.0;
      my->count_d[1] = 0.0;
    }
    else if (my->count[0] == 3)
    {
      my->count_d[0] = (400.0 - my->x) / 24.0;
      my->count_d[1] = 0.0;
    }
    return 0;
  }
  
  if (my->count[0] == 0)
    return last_boss_act_0(my, player);
  else if (my->count[0] == 1)
    return last_boss_act_1(my, player);
  else if (my->count[0] == 2)
    return last_boss_act_2(my, player);
  else if (my->count[0] == 3)
    return last_boss_act_3(my, player);
  else if (my->count[0] == 4)
    return last_boss_act_4(my, player);
  else
    fprintf(stderr, "last_boss_act: strange mode (%d)\n", my->count[0]);
  return 0;
}

static int
last_boss_act_0(tenm_object *my, const tenm_object *player)
{
  if ((my->x <= 140.0) && (my->count_d[0] < 3.0))
    my->count_d[0] += 0.1;
  if ((my->x >= ((double) WINDOW_WIDTH) - 140.0) && (my->count_d[0] > -3.0))
    my->count_d[0] -= 0.1;


  (my->count[1])++;
  if ((my->count[2] <= 0) && (my->count[1] % 4 == 0))
    tenm_table_add(normal_shot_point_new(my->x + (double)(rand() % 61) - 30.0,
                                         my->y + (double)(rand() % 61) - 30.0,
                                         6.5 + (double)(rand() % 5),
                                         player->x+ (double)(rand()%201)-100.0,
                                         player->y+ (double)(rand()%201)-100.0,
                                         COLOR_PURPLE, 1000));
  if (my->count[1] >= 48)
  {
    tenm_table_add(surprise_laser_new(60, 60));
    tenm_table_add(surprise_laser_new(WINDOW_WIDTH - 60, 60));
    my->count[1] = 0;
    if (my->count[2] > 0)
      (my->count[2])--;
  }
  
  return 0;
}

static int
last_boss_act_1(tenm_object *my, const tenm_object *player)
{
  double r_x;
  double r_y;
  double r_length;
  double dr_x;
  double dr_y;
  double temp;
  int i;
  int clockwise;

  r_x = my->x - ((double) (WINDOW_WIDTH / 2));
  r_y = my->y - ((double) (WINDOW_HEIGHT / 2));
  r_length = tenm_sqrt((int) (r_x * r_x + r_y * r_y));
  r_x /= r_length;
  r_y /= r_length;

  if (my->count[2] <= 0)
  {
    /* make sure we have a positive radius */
    if (r_length <= 50.0)
      my->count_d[2] = 0.0;
    if ((r_length <= 70.0) && (my->count_d[2] < 3.0))
      my->count_d[2] += 0.02;
    if ((r_length >= 85.0) && (my->count_d[2] > -3.0))
      my->count_d[2] -= 0.02;
  }

  dr_x = -r_y;
  dr_y = r_x;

  temp = r_length / 5.0;
  my->count_d[0] = my->count_d[2] * r_x + 0.3 * dr_x * temp;
  my->count_d[1] = my->count_d[2] * r_y + 0.3 * dr_y * temp;

  tenm_table_add(rotate_laser_new(my->x, my->y, my->count_d[0], my->count_d[1],
                                  my->count[3] * 3 + 270, 3.0,
                                  325.0, COLOR_PURPLE, 1));

  if ((my->count[1] >= 0) && (my->count[1] < 15))
  {
      tenm_table_add(rotate_laser_new(my->x, my->y,
                                      my->count_d[0], my->count_d[1],
                                      my->count[3] * 4 + 270, 4.0,
                                      800.0, COLOR_PURPLE, 1));
  }

  if ((my->count[3] % 45 == 0) && (my->count[2] <= 0))
  {
    if (my->count[3] % 90 == 0)
      clockwise = 1;
    else
      clockwise = 0;
    for (i = 0; i < 6; i++)
      tenm_table_add(clock_shot_new(my->x, my->y, i * 60, clockwise));
  }
  
  (my->count[3])++;
  if (my->count[3] >= 360)
  {
    my->count[3] = 0;
  }

  (my->count[1])++;
  /* a prime number provides aperiodicity */
  if (my->count[1] >= 29)
  {
    my->count[1] = 0;
    if (my->count[2] > 0)
      (my->count[2])--;
  }
  
  return 0;
}

static int
last_boss_act_2(tenm_object *my, const tenm_object *player)
{
  if ((my->x <= 140.0) && (my->count_d[0] < 3.0))
    my->count_d[0] += 0.1;
  if ((my->x >= ((double) WINDOW_WIDTH) - 140.0) && (my->count_d[0] > -3.0))
    my->count_d[0] -= 0.1;

  (my->count[1])++;

  if ((my->count[2] <= 0) && (my->count[1] % 12 == 0))
    tenm_table_add(laser_point_new(my->x + (double)(rand() % 61) - 30.0,
                                   my->y + (double)(rand() % 61) - 30.0,
                                   12.0,
                                   player->x,
                                   player->y,
                                   50.0, COLOR_PURPLE, 1000));

  if ((my->count[1] == 5) || (my->count[1] == 10)
      || (my->count[1] == 15) || (my->count[1] == 20))
    tenm_table_add(spread_shot_new(rand() % WINDOW_WIDTH,
                                   rand() % WINDOW_HEIGHT,
                                   60 + my->count[1] * 2));
  if (my->count[1] >= 60)
  {
    my->count[1] = 0;
    if (my->count[2] > 0)
      (my->count[2])--;
  }
  
  return 0;
}

static int
last_boss_act_3(tenm_object *my, const tenm_object *player)
{
  int i;
  int j;
  double x_temp;
  double y_temp;
  double speed_theta;
  int theta;
  int clockwise;

  (my->count[1])++;
  if (my->count[2] <= 0)
  {
    if (my->count[1] % 2 == 0)
    {
      theta = rand() % 120 + 30;
      x_temp = ((double) (rand() % WINDOW_WIDTH)) - tenm_cos(theta) * 28.0;
      y_temp = 0.0 - tenm_sin(theta) * 28.0;
      tenm_table_add(laser_angle_new(x_temp, y_temp,
                                     25.0, theta,
                                     30.0, COLOR_GREEN, 1000));
    }
  }

  if (my->count[1] == 24)
    my->count_d[0] = 0.0;
  
  if (my->count[1] >= 32)
  {
    theta = rand() % 360;
    clockwise = 1 - (rand() % 2) * 2;
    speed_theta = 0.45 * (double) clockwise;
    for (i = 1; i <= 8; i++)
      for (j = 1; j <= 2; j++)
      {
        tenm_table_add(rotate_shot_new(my->x, my->y,
                                       5.0, theta * clockwise,
                                       speed_theta, COLOR_PURPLE, 150));
        theta -= i * 5;
      }
    if ((my->x <= 200.0) || (my->x >= 280.0))
      my->count_d[0] = (240.0 - my->x) / 24.0;
    else if (rand() % 2 == 0)
      my->count_d[0] = (400.0 - my->x) / 24.0;
    else
      my->count_d[0] = (80.0 - my->x) / 24.0;
    
    my->count[1] = 0;
    if (my->count[2] > 0)
      (my->count[2])--;
  }
  return 0;
}

/* the boss is dead */
static int
last_boss_act_4(tenm_object *my, const tenm_object *player)
{
  double x;
  double y;

  (my->count[1])++;
  if (my->count[1] < 160)
  {
    if(my->count[1] % 4 == 0)
    {
      x = my->x + (double) (rand() % 91 - 45);
      y = my->y + (double) (rand() % 91 - 45);
      tenm_table_add(explosion_new(x, y, 0.0, -0.5,
                                   1, 500, COLOR_PURPLE, 12.0, 12));
    }
    if(my->count[1] % 9 == 0)
    {
      x = my->x + (double) (rand() % 91 - 45);
      y = my->y + (double) (rand() % 91 - 45);
      tenm_table_add(explosion_new(x, y, 0.0, -0.5,
                                   2, 300, COLOR_PURPLE, 7.0, 12));
    }
  }
  else if (my->count[1] == 160)
  {
    tenm_table_add(explosion_new(my->x, my->y, 0.0, 0.0,
                                 1, 15000, COLOR_PURPLE, 16.0, 15));
    tenm_table_add(explosion_new(my->x, my->y, 0.0, 0.0,
                                 2, 3000, COLOR_PURPLE, 12.0, 15));
    tenm_table_add(explosion_new(my->x, my->y, 0.0, 0.0,
                                 3, 600, COLOR_PURPLE, 9.0, 15));
    return 0;
  }
  else if (my->count[1] == 300)
  {
    tenm_table_add(stage_clear_new(5));
    return 1;
  }
  return 0;
}

static int
last_boss_draw(tenm_object *my, int priority)
{
  int n = 0;
  int status = 0;

  /* sanity check */
  if (my == NULL)
    return 0;
  /* dead boss has lower priority */
  if (my->count[0] == 4)
  {
    if (priority != -1)
      return 0;
  }
  else
  {
    if (priority != 0)
      return 0;
  }

  if ((my->count[0] == 4) && (my->count[1] >= 160))
    return 0;

  if (my->count[5] == 0)
    n = 0;
  else
    n = 1;

  if (last_boss_blit((int) my->x, (int) my->y, n) != 0)
    status = 1;
  /*
  if (tenm_draw_primitive(my->mass->p[0], tenm_map_color(0, 0, 0)) != 0)
    status = 1;
  */

  my->count[5] = 0;

  return status;
}

static tenm_object *
reminder_new(int x, int y, int shrink_speed, int color, int life)
{
  tenm_object *new = NULL;
  int *count = NULL;

  /* sanity check */
  if (shrink_speed <= 0)
    return NULL;
  if (life <= 0)
    return NULL;
  
  count = (int *) malloc(sizeof(int) * 3);
  if (count == NULL)
  {
    fprintf(stderr, "reminder_new: malloc(count) failed\n");
    return NULL;
  }

  /* list of count
   * [0] color
   * [1] life
   * [2] shrink speed
   */
  count[0] = color;
  count[1] = life;
  count[2] = shrink_speed;

  /* ATTR_ENEMY_SHOT is only to clear it when the player is killed */
  new = tenm_object_new("reminder", ATTR_ENEMY_SHOT, 0,
                        0, (double) x, (double) y,
                        3, count, 0, NULL, 0, NULL, 
                        (int (*)(tenm_object *, double)) NULL,
                        (int (*)(tenm_object *, tenm_object *)) NULL,
                        (int (*)(tenm_object *, const tenm_object *))
                        reminder_act,
                        (int (*)(tenm_object *, int)) reminder_draw);
  if (new == NULL)
  {
    fprintf(stderr, "reminder_new: tenm_object_new failed\n");
    if (count != NULL)
      free(count);
    return NULL;
  }

  return new;
}

static int
reminder_act(tenm_object *my, const tenm_object *player)
{
  (my->count[1])--;
  if (my->count[1] <= 0)
    return 1;
  return 0;
}

static int
reminder_draw(tenm_object *my, int priority)
{
  int status = 0;
  tenm_color color;

  /* sanity check */
  if (my == NULL)
    return 0;
  if (priority != 0)
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

  if (tenm_draw_circle((int) (my->x), (int) (my->y),
                       my->count[1] * my->count[2], 1,
                       color) != 0)
    status = 1;
  return status;
}

static tenm_object *
surprise_laser_new(int x, int y)
{
  tenm_object *new = NULL;
  int *count = NULL;

  count = (int *) malloc(sizeof(int) * 2);
  if (count == NULL)
  {
    fprintf(stderr, "last_boss_new: malloc(count) failed\n");
    return NULL;
  }

  /* list of count
   * [0] color (just for delete_enemy_shot)
   * [1] life
   */
  count[0] = COLOR_GREEN;
  count[1] = 47;

  /* ATTR_ENEMY_SHOT is only to clear it when the player is killed */
  new = tenm_object_new("surprise laser manager", ATTR_ENEMY_SHOT, 0,
                        0, (double) x, (double) y,
                        2, count, 0, NULL, 0, NULL, 
                        (int (*)(tenm_object *, double)) NULL,
                        (int (*)(tenm_object *, tenm_object *)) NULL,
                        (int (*)(tenm_object *, const tenm_object *))
                        surprise_laser_act,
                        (int (*)(tenm_object *, int)) NULL);
  if (new == NULL)
  {
    fprintf(stderr, "surprise_laser_new: tenm_object_new failed\n");
    if (count != NULL)
      free(count);
    return NULL;
  }

  return new;
}

static int
surprise_laser_act(tenm_object *my, const tenm_object *player)
{
  double dx;
  double dy;
  double temp;
  double delta_x;

  (my->count[1])--;
  if (my->count[1] == 25)
  {
    tenm_table_add(reminder_new((int) (my->x), (int) (my->y), 3,
                                COLOR_GREEN, 24));
    return 0;
  }
  if (my->count[1] <= 0)
  {
    /* order does matter here */
    dx = player->x - my->x;
    dy = (player->y - 60.0) - my->y;
    temp = tenm_sqrt((int) (dx * dx + dy * dy));
    if (temp <= NEAR_ZERO)
    {
      dx = 0.0;
      dy = 1.0;
      temp = 1.0;
    }
    tenm_table_add(laser_new(my->x, my->y, 0.0, 0.0,
                             800.0 * dx / temp, 800.0 * dy / temp,
                             COLOR_GREEN, 10));

    if (my->x < ((double) WINDOW_WIDTH) / 2.0)
      delta_x = -60.0;
    else
      delta_x = 60.0;
    /* order does matter here */
    dx = (player->x + delta_x) - my->x;
    dy = player->y - my->y;
    temp = tenm_sqrt((int) (dx * dx + dy * dy));
    if (temp <= NEAR_ZERO)
    {

      if (my->x < ((double) WINDOW_WIDTH) / 2.0)
        dx = 1.0;
      else
        dx = -1.0;
      dy = 0.0;
      temp = 1.0;
    }
    tenm_table_add(laser_new(my->x, my->y, 0.0, 0.0,
                             800.0 * dx / temp, 800.0 * dy / temp,
                             COLOR_GREEN, 10));
    return 1;
  }
  return 0;
}

/* codename "Akai's star dust" */
static tenm_object *
spread_shot_new(int x, int y, int life)
{
  tenm_object *new = NULL;
  int *count = NULL;

  count = (int *) malloc(sizeof(int) * 3);
  if (count == NULL)
  {
    fprintf(stderr, "spread_shot_new: malloc(count) failed\n");
    return NULL;
  }

  /* list of count
   * [0] color (just for delete_enemy_shot)
   * [1] life
   * [2] reminder flag
   */
  count[0] = COLOR_PURPLE;
  count[1] = life;
  count[2] = 0;

  /* ATTR_ENEMY_SHOT is only to clear it when the player is killed */
  new = tenm_object_new("spread shot manager", ATTR_ENEMY_SHOT, 0,
                        0, (double) x, (double) y,
                        3, count, 0, NULL, 0, NULL, 
                        (int (*)(tenm_object *, double)) NULL,
                        (int (*)(tenm_object *, tenm_object *)) NULL,
                        (int (*)(tenm_object *, const tenm_object *))
                        spread_shot_act,
                        (int (*)(tenm_object *, int)) NULL);
  if (new == NULL)
  {
    fprintf(stderr, "spread_shot_new: tenm_object_new failed\n");
    if (count != NULL)
      free(count);
    return NULL;
  }

  return new;
}

static int
spread_shot_act(tenm_object *my, const tenm_object *player)
{
  int i;
  int temp;

  (my->count[1])--;
  if (my->count[2] == 0)
  {
    tenm_table_add(reminder_new((int) (my->x), (int) (my->y), 3,
                                COLOR_PURPLE, 24));
    /* this is equal to the initial life, so strictly positive */
    my->count[2] = my->count[1] + 1;
    return 0;
  }
  if (my->count[1] <= 0)
  {
    if (my->count[2] % 24 < 12)
      temp = 0;
    else
      temp = 30;
    for (i = 0; i < 6; i++)
      tenm_table_add(normal_shot_angle_new(my->x, my->y, 4.5, temp + i * 60,
                                           COLOR_PURPLE, 1000));
    for (i = 0; i < 6; i++)
      tenm_table_add(normal_shot_angle_new(my->x, my->y, 6.5, temp+30 + i * 60,
                                           COLOR_PURPLE, 1000));
    return 1;
  }
  return 0;
}

static tenm_object *
rotate_shot_new(double x, double y, double speed_r, int theta,
                double speed_theta, int color, int life)
{
  tenm_primitive **p = NULL;
  tenm_object *new = NULL;
  int *count = NULL;
  double *count_d = NULL;

  /* sanity check */
  if (life <= 0)
    return NULL;
  if (speed_r <= 0.0)
    return NULL;
  /* negative speed_theta is OK --- it means counterclockwise */
  if ((speed_theta > -0.01) && (speed_theta < 0.01))
    return NULL;

  p = (tenm_primitive **) malloc(sizeof(tenm_primitive *) * 1);
  if (p == NULL)
  {
    fprintf(stderr, "rotate_shot_new: malloc(p) failed\n");
    return NULL;
  }

  p[0] = (tenm_primitive *) tenm_circle_new(x, y, 5.0);
  if (p[0] == NULL)
  {
    fprintf(stderr, "rotate_shot_new: cannot set p[0]\n");
    free(p);
    return NULL;
  }

  count = (int *) malloc(sizeof(int) * 2);
  if (count == NULL)
  {
    fprintf(stderr, "rotate_shot_new: malloc(count) failed\n");
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }
  count_d = (double *) malloc(sizeof(double) * 6);
  if (count_d == NULL)
  {
    fprintf(stderr, "rotate_shot_new: malloc(count_d) failed\n");
    free(count);
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }

  /* list of count
   * [0] color
   * [1] life
   */
  /* list of count_d
   * [0] speed x
   * [1] speed y
   * [2] center x
   * [3] center y
   * [4] speed r
   * [5] speed theta
   */
  count[0] = color;
  count[1] = life;

  count_d[0] = speed_r * tenm_cos(theta) + speed_theta * tenm_cos(theta + 90);
  count_d[1] = speed_r * tenm_sin(theta) + speed_theta * tenm_sin(theta + 90);
  count_d[2] = x + 5.0 * tenm_cos(theta);
  count_d[3] = y + 5.0 * tenm_sin(theta);
  count_d[4] = speed_r;
  count_d[5] = speed_theta;

  new = tenm_object_new("rotate shot", ATTR_ENEMY_SHOT, 0,
                        1, count_d[2], count_d[3],
                        2, count, 6, count_d, 1, p, 
                        (int (*)(tenm_object *, double)) rotate_shot_move,
                        NULL,
                        (int (*)(tenm_object *, const tenm_object *))
                        rotate_shot_act,
                        (int (*)(tenm_object *, int)) rotate_shot_draw);

  if (new == NULL)
  {
    fprintf(stderr, "rotate_shot_new: tenm_object_new failed\n");
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
rotate_shot_move(tenm_object *my, double turn_per_frame)
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

  /* don't delete it even if it is out of the window
   * --- you need to set my->count[1] carefully
   */
  return 0;
}

static int
rotate_shot_act(tenm_object *my, const tenm_object *player)
{
  double r_x;
  double r_y;
  double r_length;
  double dr_x;
  double dr_y;
  double temp;

  (my->count[1])--;
  if (my->count[1] <= 0)
    return 1;

  r_x = my->x - my->count_d[2];
  r_y = my->y - my->count_d[3];
  r_length = tenm_sqrt((int) (r_x * r_x + r_y * r_y));
  r_x /= r_length;
  r_y /= r_length;

  dr_x = -r_y;
  dr_y = r_x;

  temp = r_length / 5.0;
  my->count_d[0] = my->count_d[4] * r_x + my->count_d[5] * dr_x * temp;
  my->count_d[1] = my->count_d[4] * r_y + my->count_d[5] * dr_y * temp;

  return 0;
}

static int
rotate_shot_draw(tenm_object *my, int priority)
{
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

  return tenm_draw_circle((int) (my->x), (int) (my->y), 5, 2, color);

  /* may be useful for debugging */
  /*
  return tenm_draw_primitive(my->mass->p[0], color);
  */
}

static tenm_object *
clock_shot_new(double x, double y, int theta, int clockwise)
{
  tenm_primitive **p = NULL;
  tenm_object *new = NULL;
  int *count = NULL;
  double *count_d = NULL;

  /* sanity check */

  p = (tenm_primitive **) malloc(sizeof(tenm_primitive *) * 1);
  if (p == NULL)
  {
    fprintf(stderr, "clock_shot_new: malloc(p) failed\n");
    return NULL;
  }

  p[0] = (tenm_primitive *) tenm_circle_new(x, y, 5.0);
  if (p[0] == NULL)
  {
    fprintf(stderr, "clock_shot_new: cannot set p[0]\n");
    free(p);
    return NULL;
  }

  count = (int *) malloc(sizeof(int) * 4);
  if (count == NULL)
  {
    fprintf(stderr, "clock_shot_new: malloc(count) failed\n");
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }
  count_d = (double *) malloc(sizeof(double) * 2);
  if (count_d == NULL)
  {
    fprintf(stderr, "clock_shot_new: malloc(count_d) failed\n");
    free(count);
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }

  /* list of count
   * [0] color (just for delete_enemy_shot)
   * [1] theta
   * [2] speed down/up timer
   * [3] clockwise flag
   */
  /* list of count_d
   * [0] speed x
   * [1] speed y
   */
  count[0] = COLOR_PURPLE;
  count[1] = theta;
  count[2] = 0;
  count[3] = clockwise;

  count_d[0] = 7.0 * tenm_cos(theta);
  count_d[1] = 7.0 * tenm_sin(theta);

  new = tenm_object_new("clock shot", ATTR_ENEMY_SHOT, 0,
                        1, x, y,
                        4, count, 2, count_d, 1, p, 
                        (int (*)(tenm_object *, double)) clock_shot_move,
                        NULL,
                        (int (*)(tenm_object *, const tenm_object *))
                        clock_shot_act,
                        (int (*)(tenm_object *, int)) clock_shot_draw);

  if (new == NULL)
  {
    fprintf(stderr, "clock_shot_new: clock_object_new failed\n");
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
clock_shot_move(tenm_object *my, double turn_per_frame)
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

  if ((!in_window_object(my)) && (my->count[2] >= 61))
    return 1;
  return 0;
}

static int
clock_shot_act(tenm_object *my, const tenm_object *player)
{
  (my->count[2])++;

  if ((my->count[2] < 50) && (my->count[2] % 5 == 0))
  {
    my->count_d[0] -= 0.7 * tenm_cos(my->count[1]);
    my->count_d[1] -= 0.7 * tenm_sin(my->count[1]);
  }
  else if (my->count[2] == 50)
  {
    my->count_d[0] = 0.0;
    my->count_d[1] = 0.0;
    if (my->count[3] != 0)
      my->count[1] += 150;
    else
      my->count[1] -= 150;
  }
  else if ((my->count[2] > 51) && (my->count[2] % 5 == 0))
  {
    my->count_d[0] += 0.7 * tenm_cos(my->count[1]);
    my->count_d[1] += 0.7 * tenm_sin(my->count[1]);
  }

  return 0;
}

static int
clock_shot_draw(tenm_object *my, int priority)
{
  /* sanity check */
  if (my == NULL)
    return 0;
  if (priority != 1)
    return 0;

  return tenm_draw_circle((int) (my->x), (int) (my->y), 5, 2,
                          tenm_map_color(127, 0, 255));
  /* may be useful for debugging */
  /*
  return tenm_draw_primitive(my->mass->p[0], tenm_map_color(127, 0, 255));
  */
}

tenm_object *
last_boss_demo_new(double x, double y, int n)
{
  tenm_object *new = NULL;
  int *count = NULL;

  count = (int *) malloc(sizeof(int) * 2);
  if (count == NULL)
  {
    fprintf(stderr, "last_boss_demo_new: malloc(count) failed\n");
    return NULL;
  }

  /* list of count
   * [0] timer
   * [1] mode
   */
  count[0] = TIME_DEMO - 10;
  count[1] = n;

  new = tenm_object_new("L demo", 0, 0,
                        0, x, y,
                        2, count, 0, NULL, 0, NULL, 
                        (int (*)(tenm_object *, double)) NULL,
                        (int (*)(tenm_object *, tenm_object *)) NULL,
                        (int (*)(tenm_object *, const tenm_object *))
                        last_boss_demo_act,
                        (int (*)(tenm_object *, int)) last_boss_demo_draw);
  if (new == NULL)
  {
    fprintf(stderr, "last_boss_demo_new: tenm_object_new failed\n");
    free(count);
    return NULL;
  }

  return new;
}

static int
last_boss_demo_act(tenm_object *my, const tenm_object *player)
{
  (my->count[0])--;
  if (my->count[0] <= 0)
    return 1;
  return 0;
}

static int
last_boss_demo_draw(tenm_object *my, int priority)
{
  /* sanity check */
  if (my == NULL)
    return 0;
  if (priority != 1)
    return 0;

  if (my->count[0] > TIME_DEMO - 20)
    return 0;

  return last_boss_demo_blit((int) my->x, (int) my->y, my->count[1]);
}
