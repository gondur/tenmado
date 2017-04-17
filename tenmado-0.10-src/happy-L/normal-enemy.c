/* $Id: normal-enemy.c,v 1.99 2004/02/15 14:39:46 oohara Exp $ */

#include <stdio.h>
/* malloc, rand */
#include <stdlib.h>
/* for normal_enemy_new */
#include <stdarg.h>

#include "tenm_object.h"
#include "tenm_primitive.h"
#include "const.h"
#include "tenm_graphic.h"
#include "tenm_table.h"
#include "explosion.h"
#include "chain.h"
#include "hit-status.h"
#include "score.h"
#include "normal-shot.h"
#include "util.h"
#include "tenm_math.h"
#include "laser.h"
#include "image.h"

#include "normal-enemy.h"

#define COUNT_MODE_BEGIN 16
#define COUNT_D_MODE_BEGIN 10
#define NEAR_ZERO 0.0001

static int normal_enemy_hit_point(int what);
static int normal_enemy_move(tenm_object *my, double turn_per_frame);
static int normal_enemy_hit(tenm_object *my, tenm_object *your);
static int normal_enemy_explode(const tenm_object *my);
static void normal_enemy_do_signal(const tenm_object *my);
static int normal_enemy_signal(tenm_object *my, int n);
static int normal_enemy_act(tenm_object *my, const tenm_object *player);
static int normal_enemy_draw(tenm_object *my, int priority);

/* ... is one or more of
 * (int t_move, double dx, double dy, double ddx, double ddy,
 * double center_x, double center_y, double speed_r, double speed_theta,
 * int next_mode_move)
 * followed by one or more of
 * (int t_shoot, int time_shoot, int time_shoot_initial,
 * int randomness, int next_mode_shoot)
 * if speed_theta is very large, you may want to use smaller speed_r
 * to cope with the dr error
 */
tenm_object *
normal_enemy_new(double x, double y, int what, int type, int color,
                 int time_no_escape, int signal_index, int signal_suffix,
                 int number_mode_move, int number_mode_shoot, ...)
{
  va_list ap;
  tenm_primitive **p = NULL;
  tenm_object *new = NULL;
  int *count = NULL;
  double *count_d = NULL;
  const char *name = NULL;
  int n_p = 1;
  int i;
  int j;
  int n;
  int n_d;
  int attr = ATTR_ENEMY;
  int hit_mask = ATTR_PLAYER_SHOT;
  int suffix;
  int suffix_d;
  int hit_point = -1;
  /* for rotate move */
  double r_x;
  double r_y;
  double r_length;
  double dr_x;
  double dr_y;
  double temp;
  double r_x_temp;
  double r_y_temp;
  double r_length_temp;

  /* sanity check */
  if (time_no_escape < 0)
    return NULL;
  if (number_mode_move <= 0)
    return NULL;
  if (number_mode_shoot <= 0)
    return NULL;

  switch (what)
  {
  case NORMAL_BALL_SMALL:
    name = "small ball";
    break;
  case NORMAL_BALL_MEDIUM:
    name = "medium ball";
    break;
  case NORMAL_BALL_LARGE:
    name = "large ball";
    break;
  case NORMAL_STING:
    name = "sting";
    break;
  case NORMAL_HORIZON:
    name = "horizon";
    break;
  case NORMAL_PLENTY:
    name = "plenty";
    break;
  case NORMAL_BURST:
    name = "burst";
    break;
  default:
    fprintf(stderr, "normal_enemy_new: strange what when setting name "
            "(%d)\n", what);
    return NULL;
    break;
  }

  hit_point = normal_enemy_hit_point(what);
  if (hit_point < 0)
  {
    fprintf(stderr, "normal_enemy_new: normal_enemy_hit_point failed\n");
    return NULL;
  }

  switch (what)
  {
  case NORMAL_BALL_SMALL:
    n_p = 1;
    break;
  case NORMAL_BALL_MEDIUM:
    n_p = 1;
    break;
  case NORMAL_BALL_LARGE:
    n_p = 1;
    break;
  case NORMAL_STING:
    n_p = 1;
    break;
  case NORMAL_HORIZON:
    n_p = 1;
    break;
  case NORMAL_PLENTY:
    n_p = 1;
    break;
  case NORMAL_BURST:
    n_p = 1;
    break;
  default:
    fprintf(stderr, "normal_enemy_new: strange what when setting n_p "
            "(%d)\n", what);
    return NULL;
    break;
  }

  p = (tenm_primitive **) malloc(sizeof(tenm_primitive *) * n_p);
  if (p == NULL)
  {
    fprintf(stderr, "normal_enemy_new: malloc(p) failed\n");
    return NULL;
  }

  switch (what)
  {
  case NORMAL_BALL_SMALL:
    p[0] = (tenm_primitive *) tenm_circle_new(x, y, 15.0);
    break;
  case NORMAL_BALL_MEDIUM:
    p[0] = (tenm_primitive *) tenm_circle_new(x, y, 20.0);
    break;
  case NORMAL_BALL_LARGE:
    p[0] = (tenm_primitive *) tenm_circle_new(x, y, 25.0);
    break;
  case NORMAL_STING:
    p[0] = (tenm_primitive *) tenm_polygon_new(4,
                                               x - 32.0, y - 24.0,
                                               x - 32.0, y + 24.0,
                                               x + 32.0, y + 24.0,
                                               x + 32.0, y - 24.0);
    break;
  case NORMAL_HORIZON:
    p[0] = (tenm_primitive *) tenm_polygon_new(4,
                                               x - 25.0, y - 25.0,
                                               x - 25.0, y + 25.0,
                                               x + 25.0, y + 25.0,
                                               x + 25.0, y - 25.0);
    break;
  case NORMAL_PLENTY:
    p[0] = (tenm_primitive *) tenm_polygon_new(4,
                                               x - 36.0, y - 48.0,
                                               x - 36.0, y + 48.0,
                                               x + 36.0, y + 48.0,
                                               x + 36.0, y - 48.0);
    break;
  case NORMAL_BURST:
    p[0] = (tenm_primitive *) tenm_polygon_new(4,
                                               x - 24.0, y - 32.0,
                                               x - 24.0, y + 32.0,
                                               x + 24.0, y + 32.0,
                                               x + 24.0, y - 32.0);
    break;
  default:
    fprintf(stderr, "normal_enemy_new: strange what when setting p "
            "(%d)\n", what);
    return NULL;
    break;
  }

  for (i = 0; i < n_p; i++)
    if (p[i] == NULL)
    {
      fprintf(stderr, "normal_enemy_new: cannot set p[%d]\n", i);
      /* we need to check from 0 to n_p to delete everything */
      for (j = 0; j < n_p; j++)
        if (p[j] != NULL)
          (p[j])->delete(p[j]);
      free(p);
      return NULL;
    }

  n = COUNT_MODE_BEGIN + 2 * number_mode_move + 5 * number_mode_shoot;
  count = (int *) malloc(sizeof(int) * n);
  if (count == NULL)
  {
    fprintf(stderr, "normal_enemy_new: malloc(count) failed\n");
      for (j = 0; j < n_p; j++)
        if (p[j] != NULL)
          (p[j])->delete(p[j]);
    free(p);
    return NULL;
  }

  n_d = COUNT_D_MODE_BEGIN + 8 * number_mode_move + 0 * number_mode_shoot;
  count_d = (double *) malloc(sizeof(double) * n_d);
  if (count_d == NULL)
  {
    fprintf(stderr, "normal_enemy_new: malloc(count_d) failed\n");
    free(count);
      for (j = 0; j < n_p; j++)
        if (p[j] != NULL)
          (p[j])->delete(p[j]);
    free(p);
    return NULL;
  }

  /* list of count
   * [0] color
   * [1] shoot timer (shoot if [1] >= [2])
   * [2] shoot interval (fixed in each mode)
   * [3] no escape time
   * [4] move mode timer
   * [5] current mode_move
   * [6] total number of mode_move
   * [7] shoot mode timer
   * [8] current mode_shoot
   * [9] total number of mode_shoot
   * [10] shoot randomness
   * [11] what
   * [12] type
   * [13] "deleted" signal index (-1 if disabled)
   * [14] "deleted" signal suffix
   * [15] "damaged" flag
   * [16 --] mode data  ((t_move, next_mode_move), (t_shoot, time_shoot,
   *   time_shoot_initial, randomness, next_mode_shoot))
   */
  /* list of count_d
   * [0] dx
   * [1] dy
   * [2] ddx
   * [3] ddy
   * [4] center x
   * [5] center y
   * [6] center dx
   * [7] center dy
   * [8] speed_r
   * [9] speed_theta (0.0 means no rotate move)
   * [10 --] mode data ((dx, dy, ddx, ddy,
   *   center_x, center_y, speed_r, speed_theta), ())
   */

  va_start(ap, number_mode_shoot);
  for (i = 0; i < number_mode_move; i++)
  {
    suffix = COUNT_MODE_BEGIN + 2 * i;
    suffix_d = COUNT_D_MODE_BEGIN + 8 * i;
    /* t_move */
    count[suffix + 0] = va_arg(ap, int);
    if (count[suffix + 0] <= 0)
    {
      fprintf(stderr, "normal_enemy_new: strange t_move (%d) in mode %d\n",
              count[suffix + 0], i);
      free(count_d);
      free(count);
      for (j = 0; j < n_p; j++)
        if (p[j] != NULL)
          (p[j])->delete(p[j]);
      free(p);
      va_end(ap);
      return NULL;
    }
    /* dx */
    count_d[suffix_d + 0] = va_arg(ap, double);
    /* dy */
    count_d[suffix_d + 1] = va_arg(ap, double);
    /* ddx */
    count_d[suffix_d + 2] = va_arg(ap, double);
    /* ddy */
    count_d[suffix_d + 3] = va_arg(ap, double);
    /* center_x */
    count_d[suffix_d + 4] = va_arg(ap, double);
    /* center_y */
    count_d[suffix_d + 5] = va_arg(ap, double);
    /* speed_r */
    count_d[suffix_d + 6] = va_arg(ap, double);
    /* speed_theta
     * negative speed_theta is OK --- it means counterclockwise
     */
    count_d[suffix_d + 7] = va_arg(ap, double);
    /* next_mode_move */
    count[suffix + 1] = va_arg(ap, int);
  }

  for (i = 0; i < number_mode_shoot; i++)
  {
    suffix = COUNT_MODE_BEGIN + 2 * number_mode_move + 5 * i;
    /* there is no double data for shoot mode */
    /* t_shoot */
    count[suffix + 0] = va_arg(ap, int);
    if (count[suffix + 0] <= 0)
    {
      fprintf(stderr, "normal_enemy_new: strange t_shoot (%d) in mode %d\n",
              count[suffix + 0], i);
      free(count_d);
      free(count);
      for (j = 0; j < n_p; j++)
        if (p[j] != NULL)
          (p[j])->delete(p[j]);
      free(p);
      va_end(ap);
    return NULL;
    }
    /* time_shoot */
    count[suffix + 1] = va_arg(ap, int);
    if (count[suffix + 1] <= 0)
    {
      fprintf(stderr, "normal_enemy_new: strange time_shoot (%d) in mode %d\n",
              count[suffix + 1], i);
      free(count_d);
      free(count);
      for (j = 0; j < n_p; j++)
        if (p[j] != NULL)
          (p[j])->delete(p[j]);
      free(p);
      va_end(ap);
    return NULL;
    }
    /* time_shoot_initial
     * negative time_shoot_initial is OK --- it means additional wait
     * before the first shoot
     */
    count[suffix + 2] = va_arg(ap, int);
    /* randomness */
    count[suffix + 3] = va_arg(ap, int);
    if (count[suffix + 3] < 0)
    {
      fprintf(stderr, "normal_enemy_new: strange randomness (%d) in mode %d\n",
              count[suffix + 3], i);
      free(count_d);
      free(count);
      for (j = 0; j < n_p; j++)
        if (p[j] != NULL)
          (p[j])->delete(p[j]);
      free(p);
      va_end(ap);
    return NULL;
    }
    /* next_mode_shoot */
    count[suffix + 4] = va_arg(ap, int);
  }

  va_end(ap);

  count[0] = color;
  count[1] = count[COUNT_MODE_BEGIN + 2 * number_mode_move + 2];
  count[2] = count[COUNT_MODE_BEGIN + 2 * number_mode_move + 1];
  count[3] = time_no_escape;
  count[4] = count[COUNT_MODE_BEGIN + 0];
  count[5] = 0;
  count[6] = number_mode_move;
  count[7] = count[COUNT_MODE_BEGIN + 2 * number_mode_move + 0];
  count[8] = 0;
  count[9] = number_mode_shoot;
  count[10] = count[COUNT_MODE_BEGIN + 2 * number_mode_move + 3];
  count[11] = what;
  count[12] = type;
  count[13] = signal_index;
  count[14] = signal_suffix;
  count[15] = 0;

  /* count_d[0] and count_d[1] are set below */
  count_d[2] = count_d[COUNT_D_MODE_BEGIN + 2];
  count_d[3] = count_d[COUNT_D_MODE_BEGIN + 3];
  count_d[4] = count_d[COUNT_D_MODE_BEGIN + 4];
  count_d[5] = count_d[COUNT_D_MODE_BEGIN + 5];
  count_d[6] = count_d[COUNT_D_MODE_BEGIN + 0];
  count_d[7] = count_d[COUNT_D_MODE_BEGIN + 1];
  count_d[8] = count_d[COUNT_D_MODE_BEGIN + 6];
  count_d[9] = count_d[COUNT_D_MODE_BEGIN + 7];
  if ((count_d[9] >= -0.01) && (count_d[9] <= 0.01))  
  {
    /* no rotate */
    count_d[0] = count_d[6];
    count_d[1] = count_d[7];
  }
  else
  {
    r_x = x - count_d[4];
    r_y = y - count_d[5];
    r_length = tenm_sqrt((int) (r_x * r_x + r_y * r_y));
    if (r_length < NEAR_ZERO)
      r_length = 1.0;
    r_x /= r_length;
    r_y /= r_length;
    
    dr_x = -r_y;
    dr_y = r_x;
    
    temp = r_length / 5.0;

    /* dr should not modify the radius */
    r_x_temp = x + count_d[9] * dr_x * temp - count_d[4];
    r_y_temp = y + count_d[9] * dr_y * temp - count_d[5];
    r_length_temp = tenm_sqrt((int) (r_x_temp * r_x_temp
                                     + r_y_temp * r_y_temp));
    if (r_length_temp < NEAR_ZERO)
      r_length_temp = 1.0;
    count_d[8] -= r_length_temp - r_length;

    count_d[0] = count_d[6]
      + count_d[8] * r_x + count_d[9] * dr_x * temp;
    count_d[1] = count_d[7]
      + count_d[8] * r_y + count_d[9] * dr_y * temp;
  } 

  attr = ATTR_ENEMY;
  if (type & ENEMY_TYPE_OBSTACLE)
    attr |= ATTR_OBSTACLE;

  hit_mask = ATTR_PLAYER_SHOT;
  if (type & ENEMY_TYPE_WEAK)
    hit_mask |= ATTR_OBSTACLE;

  new = tenm_object_new(name, attr, hit_mask,
                        hit_point, x, y,
                        n, count, n_d, count_d, n_p, p, 
                        (int (*)(tenm_object *, double)) normal_enemy_move,
                        (int (*)(tenm_object *, tenm_object *))
                        normal_enemy_hit,
                        (int (*)(tenm_object *, const tenm_object *))
                        normal_enemy_act,
                        (int (*)(tenm_object *, int)) normal_enemy_draw);
  if (new == NULL)
  {
    fprintf(stderr, "normal_enemy_new: tenm_object_new failed\n");
    free(count_d);
    free(count);
    for (j = 0; j < n_p; j++)
      if (p[j] != NULL)
        (p[j])->delete(p[j]);
    free(p);
    return NULL;
  }

  return new;
}

/* return positive value on success, negative value on error */
static int
normal_enemy_hit_point(int what)
{
  switch (what)
  {
  case NORMAL_BALL_SMALL:
    return 1;
    break;
  case NORMAL_BALL_MEDIUM:
    return 1;
    break;
  case NORMAL_BALL_LARGE:
    return 1;
    break;
  case NORMAL_STING:
    return 10;
    break;
  case NORMAL_HORIZON:
    return 30;
    break;
  case NORMAL_PLENTY:
    return 100;
    break;
  case NORMAL_BURST:
    return 10;
    break;
  default:
    fprintf(stderr, "normal_enemy_hit_point: strange what (%d)\n", what);
    return -1;
    break;
  }
  /* should not reach here */
  fprintf(stderr, "normal_enemy_hit_point: switch(what) fell off\n");
  return -1;
}

static int
normal_enemy_move(tenm_object *my, double turn_per_frame)
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

  if ((my->count[3] <= 0) && (!in_window_object(my)))
  {
    normal_enemy_do_signal(my);
    return 1;
  }
  return 0;
}

static int
normal_enemy_hit(tenm_object *my, tenm_object *your)
{
  int score = 0;
  int hit_point;

  /* sanity check */
  if (my == NULL)
    return 0;
  if (your == NULL)
    return 0;

  if (your->attr & ATTR_OBSTACLE)
  {
    normal_enemy_explode(my);
    normal_enemy_do_signal(my);
    return 1;
  }

  if (my->count[0] == COLOR_GREEN)
    return 0;
  /* if the enemy has no time_no_escape, we don't need to check
   * in_window_object() again because it is done in the last move
   */
  if ((my->count[3] > 0) && (!in_window_object(my)))
    return 0;
  if (!(your->attr & ATTR_PLAYER_SHOT))
    return 0;

  hit_point = normal_enemy_hit_point(my->count[11]);
  if (hit_point < 0)
  {
    fprintf(stderr, "normal_enemy_hit: normal_enemy_hit_point failed\n");
    return 0;
  }

  switch (my->count[11])
  {
  case NORMAL_BALL_SMALL:
    score = 2;
    break;
  case NORMAL_BALL_MEDIUM:
    score = 2;
    break;
  case NORMAL_BALL_LARGE:
    score = 3;
    break;
  case NORMAL_STING:
    score = 5;
    break;
  case NORMAL_HORIZON:
    score = 7;
    break;
  case NORMAL_PLENTY:
    score = 15;
    break;
  case NORMAL_BURST:
    score = 5;
    break;
  default:
    fprintf(stderr, "normal_enemy_explode: strange what when setting score "
            "(%d)\n", my->count[11]);
    score = 0;
    break;
  }

  my->hit_point -= add_damage_score(my->hit_point, your->hit_point);
  my->count[15] = 1;
  set_hit_status(my->name, hit_point, my->hit_point);
  if (my->hit_point <= 0)
  {
    normal_enemy_explode(my);
    add_killed(my->count[0]);
    add_score(score);
    normal_enemy_do_signal(my);
    return 1;
  }
  
  return 0;
}

static int
normal_enemy_explode(const tenm_object *my)
{
  /* sanity check */
  if (my == NULL)
    return 0;

  switch (my->count[11])
  {
  case NORMAL_BALL_SMALL:
    tenm_table_add(explosion_new(my->x, my->y,
                                 0.0, 0.0,
                                 2, 15, my->count[0], 2.0, 8));
    break;
  case NORMAL_BALL_MEDIUM:
    tenm_table_add(explosion_new(my->x, my->y,
                                 my->count_d[0] / 2.0, my->count_d[1]/ 2.0,
                                 2, 20, my->count[0], 3.0, 8));
    break;
  case NORMAL_BALL_LARGE:
    tenm_table_add(explosion_new(my->x, my->y,
                                 my->count_d[0] / 2.0, my->count_d[1]/ 2.0,
                                 2, 30, my->count[0], 4.0, 8));
    break;
  case NORMAL_STING:
    tenm_table_add(explosion_new(my->x, my->y,
                                 my->count_d[0] / 2.0, my->count_d[1]/ 2.0,
                                 1, 200, my->count[0], 5.0, 8));
    tenm_table_add(explosion_new(my->x, my->y,
                                 my->count_d[0] / 2.0, my->count_d[1]/ 2.0,
                                 2, 50, my->count[0], 2.0, 8));
    break;
  case NORMAL_HORIZON:
    tenm_table_add(explosion_new(my->x, my->y,
                                 my->count_d[0] / 2.0, my->count_d[1]/ 2.0,
                                 1, 200, my->count[0], 5.0, 8));
    tenm_table_add(explosion_new(my->x, my->y,
                                 my->count_d[0] / 2.0, my->count_d[1]/ 2.0,
                                 2, 50, my->count[0], 2.0, 8));
    break;
  case NORMAL_PLENTY:
    tenm_table_add(explosion_new(my->x, my->y,
                                 my->count_d[0] / 2.0, my->count_d[1]/ 2.0,
                                 1, 500, my->count[0], 6.0, 10));
    tenm_table_add(explosion_new(my->x, my->y,
                                 my->count_d[0] / 2.0, my->count_d[1]/ 2.0,
                                 2, 150, my->count[0], 4.0, 10));
    break;
  case NORMAL_BURST:
    tenm_table_add(explosion_new(my->x, my->y,
                                 my->count_d[0] / 2.0, my->count_d[1]/ 2.0,
                                 1, 200, my->count[0], 5.0, 8));
    tenm_table_add(explosion_new(my->x, my->y,
                                 my->count_d[0] / 2.0, my->count_d[1]/ 2.0,
                                 2, 50, my->count[0], 2.0, 8));
    break;
  default:
    fprintf(stderr, "normal_enemy_explode: strange what when setting p "
            "(%d)\n", my->count[11]);
    return 1;
    break;
  }
  return 0; 
}

static void
normal_enemy_do_signal(const tenm_object *my)
{
  /* sanity check */
  if (my == NULL)
    return;
  if (my->count[13] < 0)
    return;
  if (my->count[14] < 0)
    return;

  tenm_table_apply(my->count[13],
                   (int (*)(tenm_object *, int)) normal_enemy_signal,
                   my->count[14]);
}

static int
normal_enemy_signal(tenm_object *my, int n)
{
  /* sanity check */
  if (my == NULL)
    return 0;
  if ((n < 0) || (n >= my->n))
    return 0;

  (my->count[n])++;
  return 0;
}

static int
normal_enemy_act(tenm_object *my, const tenm_object *player)
{
  int i;
  double x_temp;
  double y_temp;
  int suffix;
  int suffix_d;
  int next_mode;
  /* for rotate move */
  double r_x;
  double r_y;
  double r_length;
  double dr_x;
  double dr_y;
  double temp;
  double r_x_temp;
  double r_y_temp;
  double r_length_temp;
  /* for horizon */
  double length_y;
  /* for plenty */
  double dy;

  /* sanity check */
  if (my == NULL)
    return 0;
  if (player == NULL)
    return 0;

  /* no escape time count down */
  if (my->count[3] > 0)
    (my->count[3])--;

  /* speed change */
  my->count_d[6] += my->count_d[2];
  my->count_d[7] += my->count_d[3];

  /* rotate center move */
  my->count_d[4] += my->count_d[6];
  my->count_d[5] += my->count_d[7];

  /* rotate move */
  if ((my->count_d[9] >= -0.01) && (my->count_d[9] <= 0.01))  
  {
    /* no rotate */
    my->count_d[0] = my->count_d[6];
    my->count_d[1] = my->count_d[7];
  }
  else
  {
    r_x = my->x - my->count_d[4];
    r_y = my->y - my->count_d[5];
    r_length = tenm_sqrt((int) (r_x * r_x + r_y * r_y));
    if (r_length < NEAR_ZERO)
      r_length = 1.0;
    r_x /= r_length;
    r_y /= r_length;

    dr_x = -r_y;
    dr_y = r_x;

    /* note that my->count_d[8] is already adjusted so that
     * dr does not change the radius */
    temp = r_length / 5.0;
    my->count_d[0] = my->count_d[6]
      + my->count_d[8] * r_x + my->count_d[9] * dr_x * temp;
    my->count_d[1] = my->count_d[7]
      + my->count_d[8] * r_y + my->count_d[9] * dr_y * temp;
  }
  
  /* shoot */
  (my->count[1])++;
  if (my->count[1] >= my->count[2])
  {
    my->count[1] = 0;
    if (my->count[10] <= 0)
    {
      x_temp = 0.0;
      y_temp = 0.0;
    }
    else
    {
      x_temp = (double) ((rand() % (my->count[10] * 2 + 1)) - my->count[10]);
      y_temp = (double) ((rand() % (my->count[10] * 2 + 1)) - my->count[10]);
    }  
    switch (my->count[11])
    {
    case NORMAL_BALL_SMALL:
      /* no shoot */
      break;
    case NORMAL_BALL_MEDIUM:
      /* fall off */
    case NORMAL_BALL_LARGE:
      tenm_table_add(normal_shot_point_new(my->x, my->y, 4.0,
                                           player->x + x_temp,
                                           player->y + y_temp,
                                           my->count[0], 1000));
      break;
    case NORMAL_STING:
      tenm_table_add(normal_shot_point_new(my->x - 24.0, my->y, 5.5,
                                           player->x + x_temp,
                                           player->y + y_temp,
                                           my->count[0], 1000));
      tenm_table_add(normal_shot_point_new(my->x + 24.0, my->y, 5.5,
                                           player->x + x_temp,
                                           player->y + y_temp,
                                           my->count[0], 1000));
      break;
    case NORMAL_HORIZON:
      if (my->count[8] % 2 == 0)
        length_y = 700.0;
      else
        length_y = -700.0;
      tenm_table_add(laser_new(my->x, my->y, my->count_d[0], my->count_d[1],
                               0.0, length_y, my->count[0], 1));
      break;
    case NORMAL_PLENTY:
      tenm_table_add(laser_point_new(my->x, my->y, 7.0,
                                     player->x + x_temp,
                                     player->y + y_temp,
                                     30.0, my->count[0], 1000));
      for (i = 0; i < 2; i++)
        tenm_table_add(laser_angle_new(my->x - 31.0 + (double) (i * 62),
                                       my->y, 7.0, 90,
                                       30.0, my->count[0], 1000));
      if (player->y >= my->y)
        dy = 5.0;
      else
        dy = -5.0;
      for (i = 0; i < 2; i++)
        /* yes, a new enemy */
        tenm_table_add(normal_enemy_new(my->x - 22.0 + (double) (i * 44),
                                        my->y,
                                        NORMAL_BALL_SMALL, 0, my->count[0],
                                        0, -1, -1, 2, 1,
                                        /* move 0 */
                                        13, -5.0 + (double) (i * 10), 0.0,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 1,
                                        /* move 1 */
                                        1000, 0.0, dy, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 1,
                                        /* shoot 0 */
                                        1000, 1000, 0, 0, 0));
      break;
    case NORMAL_BURST:
      tenm_table_add(laser_angle_new(my->x - 15.0, my->y, 10.0, 90,
                                     30.0, my->count[0], 1000));
      tenm_table_add(laser_angle_new(my->x + 15.0, my->y, 10.0, 90,
                                     30.0, my->count[0], 1000));
      break;
    default:
      fprintf(stderr, "normal_enemy_act: strange what when shooting "
              "(%d)\n", my->count[11]);
      return 0;
      break;
    }
  }
  else if ((my->count[11] == NORMAL_HORIZON) && (my->count[1] > -25))
  {
    if (my->count[8] % 2 == 0)
      length_y = (double) (100 + 3 * my->count[1]);
    else
      length_y = (double) (-(100 + 3 * my->count[1]));
    tenm_table_add(laser_new(my->x, my->y, my->count_d[0], my->count_d[1],
                             0.0, length_y, my->count[0], 1));
  }
  
  /* move mode change */
  (my->count[4])--;
  if (my->count[4] <= 0)
  {
    next_mode = my->count[COUNT_MODE_BEGIN + 2 * my->count[5] + 1];
    if ((next_mode < 0) || (next_mode >= my->count[6]))
    {
      /* strange mode, die */
      normal_enemy_do_signal(my);
      return 1;
    }
    suffix = COUNT_MODE_BEGIN + 2 * next_mode;
    suffix_d = COUNT_D_MODE_BEGIN + 8 * next_mode;
    my->count[4] = my->count[suffix + 0];
    my->count[5] = next_mode;
    /* my->count_d[0] and my->count_d[0] are set below
     * to handle rotation correctly */
    my->count_d[2] = my->count_d[suffix_d + 2];
    my->count_d[3] = my->count_d[suffix_d + 3];
    my->count_d[4] = my->count_d[suffix_d + 4];
    my->count_d[5] = my->count_d[suffix_d + 5];
    my->count_d[6] = my->count_d[suffix_d + 0];
    my->count_d[7] = my->count_d[suffix_d + 1];
    my->count_d[8] = my->count_d[suffix_d + 6];
    my->count_d[9] = my->count_d[suffix_d + 7];
    if ((my->count_d[9] >= -0.01) && (my->count_d[9] <= 0.01))  
    {
      /* no rotate */
      my->count_d[0] = my->count_d[6];
      my->count_d[1] = my->count_d[7];
    }
    else
    {
      r_x = my->x - my->count_d[4];
      r_y = my->y - my->count_d[5];
      r_length = tenm_sqrt((int) (r_x * r_x + r_y * r_y));
      if (r_length < NEAR_ZERO)
        r_length = 1.0;
      r_x /= r_length;
      r_y /= r_length;

      dr_x = -r_y;
      dr_y = r_x;
 
      temp = r_length / 5.0;

      /* dr should not modify the radius */
      r_x_temp = my->x + my->count_d[9] * dr_x * temp - my->count_d[4];
      r_y_temp = my->y + my->count_d[9] * dr_y * temp - my->count_d[5];
      r_length_temp = tenm_sqrt((int) (r_x_temp * r_x_temp
                                     + r_y_temp * r_y_temp));
      if (r_length_temp < NEAR_ZERO)
        r_length_temp = 1.0;
      my->count_d[8] -= r_length_temp - r_length;

      my->count_d[0] = my->count_d[6]
        + my->count_d[8] * r_x + my->count_d[9] * dr_x * temp;
      my->count_d[1] = my->count_d[7]
        + my->count_d[8] * r_y + my->count_d[9] * dr_y * temp;
    } 
  }
  /* shoot mode change */  
  (my->count[7])--;
  if (my->count[7] <= 0)
  {
    next_mode = my->count[COUNT_MODE_BEGIN + 2 * my->count[6]
                          + 5 * my->count[8] + 4];
    if ((next_mode < 0) || (next_mode >= my->count[9]))
    {
      normal_enemy_do_signal(my);
      return 1;
    }
    suffix = COUNT_MODE_BEGIN + 2 * my->count[6] + 5 * next_mode;
    my->count[1] = my->count[suffix + 2];
    my->count[2] = my->count[suffix + 1];
    my->count[7] = my->count[suffix + 0];
    my->count[8] = next_mode;
    my->count[10] = my->count[suffix + 3];
  }

  return 0;
}

static int
normal_enemy_draw(tenm_object *my, int priority)
{
  int n = 0;
  int hit_point;
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
    n = 0;
    break;
  case COLOR_PURPLE:
    n = 1;
    break;
  case COLOR_RED:
    n = 2;
    break;
  default:
    fprintf(stderr, "normal_enemy_draw: strange color when doing blit "
            "(%d)\n", my->count[0]);
    status = 1;
    n = 0;
    break;
  }

  /* we have to check the hit point because we have no "damaged" image file
   * for an enemy whose hit point is 1
   */
  hit_point = normal_enemy_hit_point(my->count[11]);
  if (hit_point < 0)
  {
    fprintf(stderr, "normal_enemy_hit: normal_enemy_hit_point failed\n");
    status = 1;
  }
  if ((hit_point > 1) && (my->count[15] != 0))
    n += 3;

  switch (my->count[11])
  {
  case NORMAL_BALL_SMALL:
    status = small_ball_blit((int) (my->x), (int) (my->y), n);
    break;
  case NORMAL_BALL_MEDIUM:
    status = medium_ball_blit((int) (my->x), (int) (my->y), n);
    break;
  case NORMAL_BALL_LARGE:
    status = large_ball_blit((int) (my->x), (int) (my->y), n);
    break;
  case NORMAL_STING:
    status = sting_blit((int) (my->x), (int) (my->y), n);
    break;
  case NORMAL_HORIZON:
    status = horizon_blit((int) (my->x), (int) (my->y), n);
    break;
  case NORMAL_PLENTY:
    status = plenty_blit((int) (my->x), (int) (my->y), n);
    break;
  case NORMAL_BURST:
    status = burst_blit((int) (my->x), (int) (my->y), n);
    break;
  default:
    fprintf(stderr, "normal_enemy_draw: strange what when doing blit "
            "(%d)\n", my->count[11]);
    status = 1;
    break;
  }

  my->count[15] = 0;
  return status;

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

  if (tenm_draw_mass(my->mass, color) != 0)
  {
    fprintf(stderr, "normal_enemy_draw: tenm_draw_mass failed\n");
    return 1;
  }
  return 0;
}
