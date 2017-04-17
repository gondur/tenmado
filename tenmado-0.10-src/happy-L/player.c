/* $Id: player.c,v 1.155 2003/01/25 11:29:40 oohara Exp $ */

#include <stdio.h>
/* malloc */
#include <stdlib.h>

#include "const.h"
#include "tenm_graphic.h"
#include "tenm_input.h"
#include "tenm_object.h"
#include "tenm_primitive.h"
#include "player-shot.h"
#include "tenm_table.h"
#include "explosion.h"
#include "ship.h"
#include "chain.h"
#include "util.h"
#include "image.h"

#include "player.h"

#define TIME_RESTART 40
#define TIME_IMMUTABLE 50

static int player_draw(tenm_object *my, int priority);
static int player_move(tenm_object *my, double turn_per_frame);
static int player_hit(tenm_object *my, tenm_object *your);
static int player_act(tenm_object *my, const tenm_object *player);
static int player_key_status_tutorial(int t);

tenm_object *
player_new(int tutorial)
{
  double x;
  double y;
  tenm_primitive **p = NULL;
  int *count = NULL;
  double *count_d = NULL;
  tenm_object *player = NULL;

  /* sanity check */
  if ((tutorial < 0) || (tutorial > 1))
  {
    fprintf(stderr, "player_new: strange tutorial (%d)\n", tutorial);
    return NULL;
  }
  
  if (tutorial == 0)
  {
    x = ((double) WINDOW_WIDTH) / 2.0;
    y = ((double) WINDOW_HEIGHT) * 0.9;
  }
  else
  {
    x = 80.0;
    y = ((double) WINDOW_HEIGHT) * 0.9;
  }  
  
  p = (tenm_primitive **) malloc(sizeof(tenm_primitive *) * 1);
  if (p == NULL)
  {
    fprintf(stderr, "player_new: malloc(p) failed\n");
    return NULL;
  }

  p[0] = (tenm_primitive *) tenm_circle_new(x, y, 5.0);
  if (p[0] == NULL)
  {
    fprintf(stderr, "player_new: cannot set p[0]\n");
    free(p);
    return NULL;
  }

  count = (int *) malloc(sizeof(int) * 4);
  if (count == NULL)
  {
    fprintf(stderr, "player_new: malloc(count) failed\n");
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }

  count_d = (double *) malloc(sizeof(double) * 2);
  if (count_d == NULL)
  {
    fprintf(stderr, "player_new: malloc(count_d) failed\n");
    (p[0])->delete(p[0]);
    free(p);
    if (count != NULL)
      free(count);
    return NULL;
  }

  /* This is 6 ("second shot"), not -1 ("first shot") because
   * the space key is used to choose a menu in the title.
   * If this is -1, you shoot one shot as soon as the game begins,
   * which is a little ugly
   */
  count[0] = 6;
  count[1] = 50;
  count[2] = tutorial;
  count[3] = 0;

  count_d[0] = 0.0;
  count_d[1] = 0.0;

  /* list of count
   * [0] shoot wait
   *     < 0 means next is first shot (no need to wait)
   *     >= 0 means wait for next (second or later) shot
   * [1] immutable time
   * [2] real/fake (for tutorial)
   * [3] timer (for tutorial)
   */
  /*
   * list of cound_d
   * [0] speed x
   * [1] speed y
   */
  player = tenm_object_new("player", ATTR_PLAYER, 0,
                           1, x, y,
                           4, count, 2, count_d, 1, p, 
                           (int (*)(tenm_object *, double)) player_move,
                           (int (*)(tenm_object *, tenm_object *)) player_hit,
                           (int (*)(tenm_object *, const tenm_object *))
                           player_act,
                           (int (*)(tenm_object *, int)) player_draw);
  if (player == NULL)
  {
    fprintf(stderr, "player_new: tenm_object_new failed\n");
    (p[0])->delete(p[0]);
    free(p);
    if (count != NULL)
      free(count);
    if (count_d != NULL)
      free(count_d);
    return NULL;
  }

  return player;
}

static int
player_draw(tenm_object *my, int priority)
{
  int status = 0;
  int n;
  int key_status;

  /* sanity check */
  if ((priority < 0) || (priority >= 2))
    return 0;
  /* we don't check my == NULL here because this can be called
   * even if the game is over */

  /* no need to draw the player if the game is over */
  if (get_ship() < 0)
  {
    if (priority == 1)
      draw_string(200, 300, "game over", 9);
    return 0;
  }

  /* sanity check again */
  if (my == NULL)
    return 0;
  if (priority != 0)
    return 0;

  switch (get_color_expected())
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
    n = -1;
    break;
  }

  /* show key status if it is a tutorial */
  if (my->count[2] != 0)
  {
    /* my->count[3] is already incremented in act */
    key_status = player_key_status_tutorial(my->count[3] - 1);

    if (key_status & 1)
    {
      if (draw_string(WINDOW_WIDTH - 130, WINDOW_HEIGHT - 95, "UP", 2) != 0)
        status = 1;
    }
    else
    {
      if (draw_string(WINDOW_WIDTH - 130, WINDOW_HEIGHT - 95, "up", 2) != 0)
        status = 1;
    }

    if (key_status & 2)
    {
      if (draw_string(WINDOW_WIDTH - 140, WINDOW_HEIGHT - 65, "DOWN", 4) != 0)
        status = 1;
    }
    else
    {
      if (draw_string(WINDOW_WIDTH - 140, WINDOW_HEIGHT - 65, "down", 4) != 0)
        status = 1;
    }

    if (key_status & 4)
    {
      if (draw_string(WINDOW_WIDTH - 110, WINDOW_HEIGHT - 80, "RIGHT", 5) != 0)
        status = 1;
    }
    else
    {
      if (draw_string(WINDOW_WIDTH - 110, WINDOW_HEIGHT - 80, "right", 5) != 0)
        status = 1;
    }

    if (key_status & 8)
    {
      if (draw_string(WINDOW_WIDTH - 170, WINDOW_HEIGHT - 80, "LEFT", 4) != 0)
        status = 1;
    }
    else
    {
      if (draw_string(WINDOW_WIDTH - 170, WINDOW_HEIGHT - 80, "left", 4) != 0)
        status = 1;
    }

    if (key_status & 16)
    {
      if (draw_string(WINDOW_WIDTH - 50, WINDOW_HEIGHT - 80, "SPACE", 5) != 0)
        status = 1;
    }
    else
    {
      if (draw_string(WINDOW_WIDTH - 50, WINDOW_HEIGHT - 80, "space", 5) != 0)
        status = 1;
    }
  }
  
  if ((my->count[1] <= TIME_IMMUTABLE) && (n >= 0))
    if (player_blit((int) my->x, (int) my->y, n) != 0)
      status = 1;

  if (my->count[1] <= 0)
  {
    if (tenm_draw_circle((int) (my->x), (int) (my->y), 5, 2,
                         tenm_map_color(23, 39, 39)) != 0)
      status = 1;

    /* may be useful for debugging */
    /*
    if (tenm_draw_primitive(my->mass->p[0], tenm_map_color(0, 0, 0)) != 0)
      status = 1;
    */
  }
  
  return status;
}

static int
player_move(tenm_object *my, double turn_per_frame)
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

  /* boundary check */
  if (my->x + dx_temp < 0.0)
    dx_temp = -(my->x);
  else if (my->x + dx_temp > (double) WINDOW_WIDTH)
    dx_temp = ((double) WINDOW_WIDTH) - my->x;
  if (my->y + dy_temp < 0.0)
    dy_temp = -(my->y);
  else if (my->y + dy_temp > (double) WINDOW_HEIGHT)
    dy_temp = ((double) WINDOW_HEIGHT) - my->y;

  my->x += dx_temp;
  my->y += dy_temp;
  tenm_move_mass(my->mass, dx_temp, dy_temp);
  
  return 0;
}

static int
player_hit(tenm_object *my, tenm_object *your)
{
  /* sanity check */
  if (my == NULL)
    return 0;
  if (your == NULL)
    return 0;

  tenm_table_add(explosion_new(my->x, my->y,
                               my->count_d[0] / 2.0, my->count_d[1] / 2.0,
                               1, 500, get_color_expected(), 5.0, 10));
  tenm_table_add(explosion_new(my->x, my->y,
                               my->count_d[0] / 2.0, my->count_d[1] / 2.0,
                               2, 150, get_color_expected(), 3.0, 10));
  tenm_table_add(explosion_new(my->x, my->y,
                               my->count_d[0] / 2.0, my->count_d[1] / 2.0,
                               3, 40, get_color_expected(), 1.0, 10));
  my->hit_mask = 0;
  my->hit_point = 0;
  my->count[0] = -1;
  my->count[1] = TIME_RESTART + TIME_IMMUTABLE;
  my->count_d[0] = 0.0;
  my->count_d[1] = 0.0;
  return 0;
}


/* list of keys
 * 1 TENM_KEY_UP
 * 2 TENM_KEY_DOWN
 * 4 TENM_KEY_RIGHT
 * 8 TENM_KEY_LEFT
 * 16 TENM_KEY_SPACE (shoot)
 * 32 TENM_KEY_ESCAPE (quit)
 * 64 TENM_KEY_p (pause)
 */
/* note that "player" (arg 2) is NULL */
static int
player_act(tenm_object *my, const tenm_object *player)
{
  double dx_temp;
  double dy_temp;
  int key_status;
  double speed;
  double speed_diagonal;

  /* sanity check */
  if (my == NULL)
    return 0;

  if (my->count[2] == 0)
  {
    key_status = tenm_get_key_status();
  } 
  else
  {
    key_status = player_key_status_tutorial(my->count[3]);
    (my->count[3])++;
  }
  
  if ((get_ship() < 0) || (my->count[1] > 0))
    (my->count[1])--;

  if (get_ship() < 0)
  {
    if (my->count[1] <= TIME_IMMUTABLE * (-2))
      return 1;
    return 0;
  }

  if (my->count[1] == TIME_IMMUTABLE)
  {
    /* player restarts */
    add_ship(-1);
    if (get_ship() < 0)
      return 0;

    my->hit_point = 1;
    if (my->count[2] == 0)
    {
      dx_temp = ((double) WINDOW_WIDTH) / 2.0 - my->x;
      dy_temp = ((double) WINDOW_HEIGHT) * 0.9 - my->y;
    }
    else
    {
      dx_temp = 80.0 - my->x;
      dy_temp = ((double) WINDOW_HEIGHT) * 0.9 - my->y;
    }
    my->x += dx_temp;
    my->y += dy_temp;
    tenm_move_mass(my->mass, dx_temp, dy_temp);
    clear_chain();
    tenm_table_apply_all((int (*)(tenm_object *, int)) delete_enemy_shot, 0);
  }
  if (my->count[1] <= 0)
    my->hit_mask = ATTR_ENEMY | ATTR_ENEMY_SHOT | ATTR_OBSTACLE | ATTR_OPAQUE;

  if (my->hit_point == 0)
    return 0;

  /* note that 8.0 * sqrt(2.0) / 2.0 = 5.656... */
  speed = 8.0;
  speed_diagonal = 5.656;
  /* first, refuse to move for a stupid player */
  if ((key_status & 1) && (key_status & 2))
  {
    my->count_d[1] = 0.0;
    my->count_d[0] = 0.0;
  }
  else if ((key_status & 4) && (key_status & 8))
  {
    my->count_d[1] = 0.0;
    my->count_d[0] = 0.0;
  }
  else if (key_status & 1)
  {
    if (key_status & 4)
    {
      my->count_d[1] = -speed_diagonal;
      my->count_d[0] = speed_diagonal;
    }
    else if (key_status & 8)
    {
      my->count_d[1] = -speed_diagonal;
      my->count_d[0] = -speed_diagonal;
    }
    else
    {
      my->count_d[1] = -speed;
      my->count_d[0] = 0.0;
    }
  }
  else if (key_status & 2)
  {
    if (key_status & 4)
    {
      my->count_d[1] = speed_diagonal;
      my->count_d[0] = speed_diagonal;
    }
    else if (key_status & 8)
    {
      my->count_d[1] = speed_diagonal;
      my->count_d[0] = -speed_diagonal;
    }
    else
    {
      my->count_d[1] = speed;
      my->count_d[0] = 0.0;
    }
  }
  else
  {
    if (key_status & 4)
    {
      my->count_d[1] = 0.0;
      my->count_d[0] = speed;
    }
    else if (key_status & 8)
    {
      my->count_d[1] = 0.0;
      my->count_d[0] = -speed;
    }
    else
    {
      my->count_d[1] = 0.0;
      my->count_d[0] = 0.0;
    }
  }

  if (key_status & 16)
  {
    /* the second shot is intentionally delayed so that
     * you can shoot only one shot easily
     */
    if (my->count[0] <= 0)
      tenm_table_add(player_shot_new(my->x, my->y));
    if (my->count[0] < 0)
      my->count[0] = 6;
    else if (my->count[0] > 0)
      (my->count[0])--;
  }
  else
    my->count[0] = -1;
  
  return 0;
}

/* virtual key input for the tutorial */
static int
player_key_status_tutorial(int t)
{
  if ((t >= 170) && (t < 190))
    return 1;
  else if ((t >= 190) && (t < 210))
    return 4;
  else if ((t >= 210) && (t < 230))
    return 2;
  else if ((t >= 230) && (t < 240))
    return 8;
  else if ((t >= 240) && (t < 250))
    return 1;
  else if ((t >= 260) && (t < 270))
    return 1 + 8;
  else if ((t >= 270) && (t < 280))
    return 2 + 4;
  else if ((t >= 290) && (t < 300))
    return 1 + 4;
  else if ((t >= 300) && (t < 320))
    return 2 + 8;
  else if ((t >= 360) && (t < 365))
    return 16;
  else if ((t >= 370) && (t < 375))
    return 16;
  else if ((t >= 380) && (t < 385))
    return 16;
  else if ((t >= 420) && (t < 690))
    return 16;
  else if ((t >= 840) && (t < 845))
    return 16;
  else if ((t >= 860) && (t < 865))
    return 16;
  else if ((t >= 880) && (t < 885))
    return 16;
  else if ((t >= 1020) && (t < 1040))
    return 1;
  else if ((t >= 1140) && (t < 1145))
    return 16;
  else if ((t >= 1160) && (t < 1165))
    return 16;
  else if ((t >= 1180) && (t < 1185))
    return 16;
  else if ((t >= 2050) && (t < 2055))
    return 16;
  else if ((t >= 2060) && (t < 2065))
    return 16;
  else if ((t >= 2070) && (t < 2075))
    return 16;
  else if ((t >= 2080) && (t < 2085))
    return 16;
  else if ((t >= 2330) && (t < 2335))
    return 8;
  else if ((t >= 2340) && (t < 2345))
    return 16;
  else if ((t >= 2380) && (t < 2390))
    return 4;
  else if ((t >= 2395) && (t < 2400))
    return 16;
  else if ((t >= 2650) && (t < 2655))
    return 16;
  else if ((t >= 2660) && (t < 2670))
    return 8;
  else if ((t >= 2675) && (t < 2680))
    return 16;
  else if ((t >= 2685) && (t < 2695))
    return 4;
  else if ((t >= 2700) && (t < 2705))
    return 16;
  else if ((t >= 2710) && (t < 2720))
    return 8;
  else if ((t >= 2725) && (t < 2730))
    return 16;
  else if ((t >= 2735) && (t < 2745))
    return 4;
  else if ((t >= 2750) && (t < 2755))
    return 16;
  else if ((t >= 2760) && (t < 2770))
    return 8;
  else if ((t >= 2775) && (t < 2780))
    return 16;
  else if ((t >= 2785) && (t < 2795))
    return 4;
  else if ((t >= 2800) && (t < 2805))
    return 16;
  else if ((t >= 2810) && (t < 2820))
    return 8;
  else if ((t >= 2825) && (t < 2830))
    return 16;
  else if ((t >= 2835) && (t < 2840))
    return 4;
  else if ((t >= 2880) && (t < 2970))
    return 16;
  else if ((t >= 2970) && (t < 2975))
    return 8 + 16;
  else if ((t >= 2975) && (t < 3005))
    return 16;

  return 0;
}
