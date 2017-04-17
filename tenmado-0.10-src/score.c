/* $Id: score.c,v 1.21 2003/01/12 01:52:50 oohara Exp $ */

#include <stdio.h>
/* strlen */
#include <string.h>

#include "util.h"
#include "tenm_object.h"
#include "const.h"
#include "stage.h"
#include "ship.h"

#include "score.h"

#define EXTEND_FIRST 200000
#define EXTEND_LATER_EVERY 200000

static int score = 0;
static int stage_score[6];
static int extend_next = EXTEND_FIRST;

void
clear_score(void)
{
  int i;
  score = 0;
  for (i = 0; i < 6; i++)
    stage_score[i] = 0;
  extend_next = EXTEND_FIRST;
}

int
get_score(void)
{
  return score;
}

int
get_stage_score(int stage)
{
  /* sanity check */
  if ((stage <= 0) || (stage > 6))
  {
    fprintf(stderr, "get_stage_score: strange stage (%d)\n", stage);
    return 0;
  }

  return stage_score[stage - 1];
}


/* return the actual score added to the total */
int
add_score(int delta)
{
  if (score + delta < 0)
    delta = -score;

  score += delta;
  if ((get_stage() >= 1) && (get_stage() <= 6))
    stage_score[get_stage() - 1] += delta;

  while (score >= extend_next)
  {
    extend_next += EXTEND_LATER_EVERY;
    /* you don't get an extra ship if you have already
     * lost the game, if you have cleared the game or
     * if you are viewing the tutorial demo
     */
    if ((get_ship() >= 0) && (get_stage() >= 1) && (get_stage() <= 5))
      add_ship(1);
  }

  return delta;
}

/* return 0 on success, 1 on error */
int
show_score(void)
{
  char temp[32];
  sprintf(temp, "score %8d", score);
  if (draw_string(10, WINDOW_HEIGHT - 10, temp, (int) strlen(temp)) != 0)
  {
    fprintf(stderr, "show_score: draw_string failed\n");
    return 1;
  }
  return 0;
}

/* return the damege that should be subtracted from
 * the hit point of the enemy */
int
add_damage_score(int hit_point, int damage)
{
  /* sanity check */
  if (hit_point <= 0)
    return 0;
  if (damage <= 0)
    return 0;

  if (hit_point > damage)
  {
    add_score(damage);
  }
  else
  {
    damage = hit_point;
    add_score(damage - 1);
  }

  return damage;
}
