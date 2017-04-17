/* $Id: player-shot.c,v 1.82 2003/04/11 08:48:44 oohara Exp $ */

#include <stdio.h>
/* malloc */
#include <stdlib.h>

#include "const.h"
#include "tenm_graphic.h"
#include "tenm_object.h"
#include "tenm_primitive.h"
#include "chain.h"
#include "util.h"

#include "player-shot.h"

static int player_shot_move(tenm_object *my, double turn_per_frame);
static int player_shot_hit(tenm_object *my, tenm_object *your);
static int player_shot_draw(tenm_object *my, int priority);
static int player_shot_act(tenm_object *my, const tenm_object *player);

tenm_object *
player_shot_new(double x, double y)
{
  tenm_primitive **p;
  tenm_object *new;
  int *count = NULL;

  p = (tenm_primitive **) malloc(sizeof(tenm_primitive *) * 1);
  if (p == NULL)
  {
    fprintf(stderr, "player_shot_new: malloc(p) failed\n");
    return NULL;
  }
  /*
  p[0] = (tenm_primitive *) tenm_circle_new(x, y, 6.0);
  */
  p[0] = (tenm_primitive *) tenm_polygon_new(3,
                                             x, y - 46.0,
                                             x - 6.0, y + 7.0,
                                             x + 6.0, y + 7.0);


  if (p[0] == NULL)
  {
    fprintf(stderr, "player_shot_new: cannot set p[0]\n");
    free(p);
    return NULL;
  }

  count = (int *) malloc(sizeof(int) * 2);
  if (count == NULL)
  {
    fprintf(stderr, "player_shot_new: malloc(count) failed\n");
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }

  /* list of count
   * [0] delete flag
   *     0: not yet hit anything
   *     1: already hit, should be drawn only once more
   *     2: should be deleted in the first move of the next frame
   * [1] damage decrease timer
   */
  count[0] = 0;
  count[1] = 4;

  new =tenm_object_new("player_shot", ATTR_PLAYER_SHOT,
                       ATTR_ENEMY | ATTR_OBSTACLE | ATTR_OPAQUE,
                       2, x, y,
                       2, count, 0, NULL, 1, p, 
                       (int (*)(tenm_object *, double)) player_shot_move,
                       (int (*)(tenm_object *, tenm_object *)) player_shot_hit,
                       (int (*)(tenm_object *, const tenm_object *))
                       player_shot_act,
                       (int (*)(tenm_object *, int)) player_shot_draw);
  if (new == NULL)
  {
    fprintf(stderr, "player_shot_new: tenm_object_new failed\n");
    free(count);
    (p[0])->delete(p[0]);
    free(p);
    return NULL;
  }

  return new;
}

static int
player_shot_move(tenm_object *my, double turn_per_frame)
{
  double dy_temp;

  /* sanity check */
  if (my == NULL)
    return 0;
  if (turn_per_frame <= 0.5)
    return 0;

  /* evil hack for a nicer look
   * this is an abuse of a move function, but I have no better idea
   */
  if (my->count[0] == 2)
  {
    return 1;
  }
  else if (my->count[0] == 1)
  {
    my->attr = 0;
    my->hit_mask = 0;
    return 0;
  }
  
  dy_temp = -52.0 / turn_per_frame;
  my->y += dy_temp;
  tenm_move_mass(my->mass, 0.0, dy_temp);
  if (!(in_window_object(my)))
    return 1;

  /* another evil hack */
  if ((my->hit_point != 1) && (my->count[1] <= 0))
    my->hit_point = 1;

  return 0;
}

static int
player_shot_hit(tenm_object *my, tenm_object *your)
{
  /* sanity check */
  if (my == NULL)
    return 0;
  if (your == NULL)
    return 0;

  my->count[0] = 1;
  return 0;
}

static int
player_shot_act(tenm_object *my, const tenm_object *player)
{
  /* sanity check */
  if (my == NULL)
    return 0;
  if (player == NULL)
    return 0;

  if (my->count[0] == 2)
  {
    /* should not reach here */ 
    return 1;
  }
  if (my->y + 6.0 < 0.0)
    return 1;

  if (my->count[1] > 0)
    (my->count[1])--;

  if (my->count[0] == 1)
    my->count[0] = 2;

  return 0;
}

static int
player_shot_draw(tenm_object *my, int priority)
{
  tenm_color color;

  /* sanity check */
  if (my == NULL)
    return 0;
  if (priority != 0)
    return 0;

  /* (1 - 0.6) * 255 + 0.6 * normal_shot_color */
  switch (get_color_expected())
  {
  case COLOR_BLUE:
    color = tenm_map_color(102, 140, 255);
    break;
  case COLOR_PURPLE:
    color = tenm_map_color(178, 102, 255);
    break;
  case COLOR_RED:
    color = tenm_map_color(255, 102, 132);
    break;
  case COLOR_GREEN:
    color = tenm_map_color(102, 255, 140);
    break;
  default:
    color = tenm_map_color(102, 102, 102);
    break;
  }

  if (my->hit_point < 2)
    color = tenm_map_color(102, 102, 102);

  if (tenm_draw_primitive(my->mass->p[0], color) != 0)
    return 1;
  return 0;
}

