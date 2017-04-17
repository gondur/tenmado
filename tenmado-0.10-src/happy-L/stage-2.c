/* $Id: stage-2.c,v 1.71 2003/01/03 22:37:14 oohara Exp $ */

#include <stdio.h>
/* malloc */
#include <stdlib.h>

#include "tenm_table.h"
#include "const.h"
#include "message.h"
#include "normal-enemy.h"
#include "tenm_object.h"
#include "wall.h"
#include "intuition.h"
#include "overrun.h"

#include "stage-2.h"

#define SCROLL_SPEED_SLOW 3.0
#define SCROLL_SPEED_FAST 7.5

static tenm_object *stage_2_more_1_new(void);
static int stage_2_more_1_act(tenm_object *my, const tenm_object *player);

int
scheduler_2(int t)
{
  int i;
  int status = SCHEDULER_SUCCESS;
  int color;
  double x;
  double y;
  double dx;
  int t_start;
  double speed_theta;
  double center_x;

  /* sanity check */
  if (t < 0)
  {
    fprintf(stderr, "scheduler_2: strange t (%d)\n", t);
    return SCHEDULER_ERROR;
  }

  if (t == 30)
  {
    if (tenm_table_add(message_new(210.0, 240.0,
                                   0.0, 0.0, 100, "stage 2", 7)) < 0)
    {
      fprintf(stderr, "scheduler_2: cannot create message \"stage 2\""
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
    if (tenm_table_add(message_new(180.0, 270.0,
                                   0.0, 0.0, 100,
                                   "if only I had L", 15)) < 0)
    {
      fprintf(stderr, "scheduler_2: cannot create message (stage 2 title) "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
    return status;
  }

  if ((t >= 180) && (t <= 1820) && (t % 40 == 20))
  {
    for (i = 0; i < 2; i++)
    {
      if (i == 0)
        x = 0.0;
      else
        x = ((double) WINDOW_WIDTH);

      if (tenm_table_add(wall_new(x, 60.0, 60.0, SCROLL_SPEED_SLOW)) < 0)
      {
        fprintf(stderr, "scheduler_2: cannot create wall (%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
  }

  if (t == 200)
  {
    for (i = 1; i <= 6; i++)
    {
      if (i == 3)
        color = COLOR_PURPLE;
      else
        color = COLOR_BLUE;

      if (tenm_table_add(normal_enemy_new(60.0 + (double) (60 * i), -24.0,
                                          NORMAL_BALL_LARGE, 0, color,
                                          0, -1, -1, 6, 1,
                                          /* move 0 */
                                          35, 0.0, SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          12 * i, -5.0, SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* move 2 */
                                          12, 0.0, -5.0 + SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 3,
                                          /* move 3 */
                                          72, 5.0, SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 4,
                                          /* move 4 */
                                          12, 0.0, 5.0 + SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 5,
                                          /* move 5 */
                                          72, -5.0, SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* shoot 0 */
                                          1000, 50, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_2: cannot create normal_enemy (%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }

    if (tenm_table_add(normal_enemy_new(60.0, -24.0,
                                        NORMAL_BALL_LARGE, 0, COLOR_BLUE,
                                        0, -1, -1, 5, 1,
                                        /* move 0 */
                                        35, 0.0, SCROLL_SPEED_SLOW,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 1,
                                        /* move 1 */
                                        12, 0.0, -5.0 + SCROLL_SPEED_SLOW,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 2,
                                        /* move 2 */
                                        72, 5.0, SCROLL_SPEED_SLOW,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 3,
                                        /* move 3 */
                                        12, 0.0, 5.0 + SCROLL_SPEED_SLOW,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 4,
                                        /* move 4 */
                                        72, -5.0, SCROLL_SPEED_SLOW,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 1,
                                        /* shoot 0 */
                                        1000, 50, 0, 0, 0)) < 0)
    {
      fprintf(stderr, "scheduler_2: cannot create normal_enemy (0) "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
    return status;
  }

  if (t == 220)
  {
    for (i = 0; i <= 5; i++)
    {
      if (i == 3)
        color = COLOR_PURPLE;
      else
        color = COLOR_RED;

      if (tenm_table_add(normal_enemy_new(60.0 + (double) (60 * i), -24.0,
                                          NORMAL_BALL_LARGE, 0, color,
                                          0, -1, -1, 6, 1,
                                          /* move 0 */
                                          15, 0.0, SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          12 * (6 - i), 5.0, SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* move 2 */
                                          12, 0.0, 5.0 + SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 3,
                                          /* move 3 */
                                          72, -5.0, SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 4,
                                          /* move 4 */
                                          12, 0.0, -5.0 + SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 5,
                                          /* move 5 */
                                          72, 5.0, SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* shoot 0 */
                                          1000, 50, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_2: cannot create normal_enemy (%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }

    if (tenm_table_add(normal_enemy_new(420.0, -24.0,
                                        NORMAL_BALL_LARGE, 0, COLOR_RED,
                                        0, -1, -1, 5, 1,
                                        /* move 0 */
                                        15, 0.0, SCROLL_SPEED_SLOW,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 1,
                                        /* move 1 */
                                        12, 0.0, 5.0 + SCROLL_SPEED_SLOW,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 2,
                                        /* move 2 */
                                        72, -5.0, SCROLL_SPEED_SLOW,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 3,
                                        /* move 3 */
                                        12, 0.0, -5.0 + SCROLL_SPEED_SLOW,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 4,
                                        /* move 4 */
                                        72, 5.0, SCROLL_SPEED_SLOW,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 1,
                                        /* shoot 0 */
                                        1000, 50, 0, 0, 0)) < 0)
    {
      fprintf(stderr, "scheduler_2: cannot create normal_enemy (6) "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
    return status;
  }

  if ((t >= 340) && (t <= 440) && (t % 10 == 0))
  {
    y = 31.0 + ((double) (t - 340)) * SCROLL_SPEED_SLOW;
    if (tenm_table_add(normal_enemy_new(-14.0, y,
                                        NORMAL_BALL_SMALL, 0, COLOR_BLUE,
                                        0, -1, -1, 1, 1,
                                        /* move 0 */
                                        1000, 5.0, SCROLL_SPEED_SLOW,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 1,
                                        /* shoot 0 */
                                        1000, 1000, 0, 0, 0)) < 0)
    {
      fprintf(stderr, "scheduler_2: cannot create normal_enemy "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
    return status;
  }
  
  if (t == 520)
  {
    for (i = 0; i <= 5; i++)
    {
      if (i == 3)
        color = COLOR_PURPLE;
      else
        color = COLOR_RED;

      if (tenm_table_add(normal_enemy_new(60.0 + (double) (60 * i), -24.0,
                                          NORMAL_BALL_LARGE, 0, color,
                                          0, -1, -1, 6, 1,
                                          /* move 0 */
                                          35, 0.0, SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          12 * (6 - i), 5.0, SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* move 2 */
                                          12, 0.0, -5.0 + SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 3,
                                          /* move 3 */
                                          72, -5.0, SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 4,
                                          /* move 4 */
                                          12, 0.0, 5.0 + SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 5,
                                          /* move 5 */
                                          72, 5.0, SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* shoot 0 */
                                          1000, 50, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_2: cannot create normal_enemy (%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }

    if (tenm_table_add(normal_enemy_new(420.0, -24.0,
                                        NORMAL_BALL_LARGE, 0, COLOR_RED,
                                        0, -1, -1, 5, 1,
                                        /* move 0 */
                                        35, 0.0, SCROLL_SPEED_SLOW,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 1,
                                        /* move 1 */
                                        12, 0.0, -5.0 + SCROLL_SPEED_SLOW,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 2,
                                        /* move 2 */
                                        72, -5.0, SCROLL_SPEED_SLOW,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 3,
                                        /* move 3 */
                                        12, 0.0, 5.0 + SCROLL_SPEED_SLOW,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 4,
                                        /* move 4 */
                                        72, 5.0, SCROLL_SPEED_SLOW,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 1,
                                        /* shoot 0 */
                                        1000, 50, 0, 0, 0)) < 0)
    {
      fprintf(stderr, "scheduler_2: cannot create normal_enemy (6) "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
    return status;
  }

  if (t == 540)
  {
    for (i = 1; i <= 6; i++)
    {
      if (i == 3)
        color = COLOR_PURPLE;
      else
        color = COLOR_BLUE;

      if (tenm_table_add(normal_enemy_new(60.0 + (double) (60 * i), -24.0,
                                          NORMAL_BALL_LARGE, 0, color,
                                          0, -1, -1, 6, 1,
                                          /* move 0 */
                                          15, 0.0, SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          12 * i, -5.0, SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* move 2 */
                                          12, 0.0, 5.0 + SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 3,
                                          /* move 3 */
                                          72, 5.0, SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 4,
                                          /* move 4 */
                                          12, 0.0, -5.0 + SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 5,
                                          /* move 5 */
                                          72, -5.0, SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* shoot 0 */
                                          1000, 50, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_2: cannot create normal_enemy (%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }

    if (tenm_table_add(normal_enemy_new(60.0, -24.0,
                                        NORMAL_BALL_LARGE, 0, COLOR_BLUE,
                                        0, -1, -1, 5, 1,
                                        /* move 0 */
                                        15, 0.0, SCROLL_SPEED_SLOW,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 1,
                                        /* move 1 */
                                        12, 0.0, 5.0 + SCROLL_SPEED_SLOW,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 2,
                                        /* move 2 */
                                        72, 5.0, SCROLL_SPEED_SLOW,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 3,
                                        /* move 3 */
                                        12, 0.0, -5.0 + SCROLL_SPEED_SLOW,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 4,
                                        /* move 4 */
                                        72, -5.0, SCROLL_SPEED_SLOW,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 1,
                                        /* shoot 0 */
                                        1000, 50, 0, 0, 0)) < 0)
    {
      fprintf(stderr, "scheduler_2: cannot create normal_enemy (0) "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
    return status;
  }

  if ((t >= 660) && (t <= 760) && (t % 10 == 0))
  {
    x = ((double) WINDOW_WIDTH) + 14.0;
    y = 31.0 + ((double) (t - 660)) * SCROLL_SPEED_SLOW;
    if (tenm_table_add(normal_enemy_new(x, y,
                                        NORMAL_BALL_SMALL, 0, COLOR_RED,
                                        0, -1, -1, 1, 1,
                                        /* move 0 */
                                        1000, -5.0, SCROLL_SPEED_SLOW,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 1,
                                        /* shoot 0 */
                                        1000, 1000, 0, 0, 0)) < 0)
    {
      fprintf(stderr, "scheduler_2: cannot create normal_enemy "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
    return status;
  }

  if (t == 840)
  {
    for (i = 1; i <= 6; i++)
    {
      if (i == 3)
        color = COLOR_PURPLE;
      else if (i % 2 == 0)
        color = COLOR_BLUE;
      else
        color = COLOR_RED;

      if (tenm_table_add(normal_enemy_new(60.0 + (double) (60 * i), -24.0,
                                          NORMAL_BALL_LARGE, 0, color,
                                          0, -1, -1, 6, 1,
                                          /* move 0 */
                                          35, 0.0, SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          12 * i, -5.0, SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* move 2 */
                                          12, 0.0, -5.0 + SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 3,
                                          /* move 3 */
                                          72, 5.0, SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 4,
                                          /* move 4 */
                                          12, 0.0, 5.0 + SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 5,
                                          /* move 5 */
                                          72, -5.0, SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* shoot 0 */
                                          1000, 50, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_2: cannot create normal_enemy (%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }

    if (tenm_table_add(normal_enemy_new(60.0, -24.0,
                                        NORMAL_BALL_LARGE, 0, COLOR_BLUE,
                                        0, -1, -1, 5, 1,
                                        /* move 0 */
                                        35, 0.0, SCROLL_SPEED_SLOW,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 1,
                                        /* move 1 */
                                        12, 0.0, -5.0 + SCROLL_SPEED_SLOW,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 2,
                                        /* move 2 */
                                        72, 5.0, SCROLL_SPEED_SLOW,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 3,
                                        /* move 3 */
                                        12, 0.0, 5.0 + SCROLL_SPEED_SLOW,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 4,
                                        /* move 4 */
                                        72, -5.0, SCROLL_SPEED_SLOW,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 1,
                                        /* shoot 0 */
                                        1000, 50, 0, 0, 0)) < 0)
    {
      fprintf(stderr, "scheduler_2: cannot create normal_enemy (0) "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
    return status;
  }

  if (t == 860)
  {
    for (i = 0; i <= 5; i++)
    {
      if (i == 3)
        color = COLOR_PURPLE;
      else if (i % 2 == 0)
        color = COLOR_RED;
      else
        color = COLOR_BLUE;

      if (tenm_table_add(normal_enemy_new(60.0 + (double) (60 * i), -24.0,
                                          NORMAL_BALL_LARGE, 0, color,
                                          0, -1, -1, 6, 1,
                                          /* move 0 */
                                          15, 0.0, SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          12 * (6 - i), 5.0, SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* move 2 */
                                          12, 0.0, 5.0 + SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 3,
                                          /* move 3 */
                                          72, -5.0, SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 4,
                                          /* move 4 */
                                          12, 0.0, -5.0 + SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 5,
                                          /* move 5 */
                                          72, 5.0, SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* shoot 0 */
                                          1000, 50, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_2: cannot create normal_enemy (%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }

    if (tenm_table_add(normal_enemy_new(420.0, -24.0,
                                        NORMAL_BALL_LARGE, 0, COLOR_RED,
                                        0, -1, -1, 5, 1,
                                        /* move 0 */
                                        15, 0.0, SCROLL_SPEED_SLOW,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 1,
                                        /* move 1 */
                                        12, 0.0, 5.0 + SCROLL_SPEED_SLOW,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 2,
                                        /* move 2 */
                                        72, -5.0, SCROLL_SPEED_SLOW,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 3,
                                        /* move 3 */
                                        12, 0.0, -5.0 + SCROLL_SPEED_SLOW,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 4,
                                        /* move 4 */
                                        72, 5.0, SCROLL_SPEED_SLOW,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 1,
                                        /* shoot 0 */
                                        1000, 50, 0, 0, 0)) < 0)
    {
      fprintf(stderr, "scheduler_2: cannot create normal_enemy (6) "
              "(t = %d)\n", t);
      /* this line breaks the show-paren-mode of emacs21 if it has
       * 18 or more non-white characters */
      status = SCHEDULER_ERROR;
    }
    return status;
  }

  if ((t >= 1060) && (t <= 1160) && (t % 10 == 0))
  {
    for (i = 0; i < 2; i++)
    {
      y = 31.0 + ((double) (t - 1060)) * SCROLL_SPEED_SLOW;
      if (i == 0)
      {
        x = -14.0;
        dx = 5.0;
      }
      else
      {
        x = ((double) WINDOW_WIDTH) + 14.0;
        dx = -5.0;
      }
      
      if (t % 30 == 10)
      {
        color = COLOR_BLUE;  
      }
      else if (t % 30 == 20)
      {
        if (i == 0)
          color = COLOR_PURPLE;
        else
          color = COLOR_RED;
      }
      else
      {
        if (i == 0)
          color = COLOR_RED;
        else
          color = COLOR_PURPLE;
      }

      if (tenm_table_add(normal_enemy_new(x, y,
                                          NORMAL_BALL_SMALL, 0, color,
                                          0, -1, -1, 1, 1,
                                          /* move 0 */
                                          1000, dx, SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* shoot 0 */
                                          1000, 1000, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_2: cannot create normal_enemy "
                "(t = %d)\n", t);
        status = SCHEDULER_ERROR;
      }
    }
    
    return status;
  }

  if ((t == 1300) || (t == 1580))
  {
    for (i = 0; i < 4; i++)
    {
      x = 60.0 + (double) (90 * i);

      if (tenm_table_add(wall_new(x, 60.0, 60.0, SCROLL_SPEED_SLOW)) < 0)
      {
        fprintf(stderr, "scheduler_2: cannot create wall (%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
  }

  if ((t == 1330) || (t == 1350) || (t == 1610) || (t == 1630))
  {
    if (t <= 1350)
      t_start = 1330;
    else
      t_start = 1610;
    for (i = 0; i < 4; i++)
    {
      if (i == 0)
        color = COLOR_PURPLE;
      else
        color = COLOR_BLUE;

      if (tenm_table_add(normal_enemy_new(60.0 + (double) (90 * i), -24.0,
                                          NORMAL_BALL_LARGE, 0, color,
                                          0, -1, -1, 5, 1,
                                          /* move 0 */
                                          35 - (t - t_start),
                                          0.0, SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          9, 5.0, SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* move 2 */
                                          11, 0.0, SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 3,
                                          /* move 3 */
                                          9, -5.0, SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 4,
                                          /* move 4 */
                                          11, 0.0, SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* shoot 0 */
                                          1000, 40, -10, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_2: cannot create normal_enemy (%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
    return status;
  }
  
  if ((((t >= 1360) && (t <= 1400)) || ((t >= 1640) && (t <= 1680)))
      && (t % 10 == 0))
  {
    if ((t == 1400) || (t == 1680))
      color = COLOR_PURPLE;
    else
      color = COLOR_RED;

    x = (330.0 + ((double) WINDOW_WIDTH)) / 2.0;
    if (tenm_table_add(normal_enemy_new(x, -23.0,
                                        NORMAL_STING, 0, color,
                                        0, -1, -1, 1, 1,
                                        /* move 0 */
                                        1000,
                                        0.0, 8.0,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 0,
                                        /* shoot 0 */
                                        1000, 25, 0, 0, 0)) < 0)
    {
      fprintf(stderr, "scheduler_2: cannot create normal_enemy "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
    return status;
  }

  if ((t == 1460) || (t == 1700))
  {
    for (i = 0; i < 4; i++)
    {
      x = ((double) WINDOW_WIDTH) - 60.0 - (double) (90 * i);

      if (tenm_table_add(wall_new(x, 60.0, 60.0, SCROLL_SPEED_SLOW)) < 0)
      {
        fprintf(stderr, "scheduler_2: cannot create wall (%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
  }

  if ((t == 1490) || (t == 1510) || (t == 1730) || (t == 1750))
  {
    if (t <= 1510)
      t_start = 1490;
    else
      t_start = 1730;

    for (i = 0; i < 4; i++)
    {
      if (i == 0)
        color = COLOR_PURPLE;
      else
        color = COLOR_RED;

      x = ((double) WINDOW_WIDTH) - 60.0 - (double) (90 * i);
      if (tenm_table_add(normal_enemy_new(x, -24.0,
                                          NORMAL_BALL_LARGE, 0, color,
                                          0, -1, -1, 5, 1,
                                          /* move 0 */
                                          35 - (t - t_start),
                                          0.0, SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          9, -5.0, SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* move 2 */
                                          11, 0.0, SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 3,
                                          /* move 3 */
                                          9, 5.0, SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 4,
                                          /* move 4 */
                                          11, 0.0, SCROLL_SPEED_SLOW,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* shoot 0 */
                                          1000, 40, -10, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_2: cannot create normal_enemy (%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
    return status;
  }

  if ((((t >= 1520) && (t <= 1560)) || ((t >= 1760) && (t <= 1800)))
      && (t % 10 == 0))
  {
    if ((t == 1560) || (t == 1800))
      color = COLOR_PURPLE;
    else
      color = COLOR_BLUE;

    x = (((double) WINDOW_WIDTH) - 330.0 + 0.0) / 2.0;
    if (tenm_table_add(normal_enemy_new(x, -23.0,
                                        NORMAL_STING, 0, color,
                                        0, -1, -1, 1, 1,
                                        /* move 0 */
                                        1000,
                                        0.0, 8.0,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 0,
                                        /* shoot 0 */
                                        1000, 25, 0, 0, 0)) < 0)
    {
      fprintf(stderr, "scheduler_2: cannot create normal_enemy "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
    return status;
  }

  if (t == 2000)
  {
    if (tenm_table_add(message_new(((double) WINDOW_WIDTH) - 1.0, 240.0,
                                   -5.0, 0.0, 1000,
                                   "It's a guide to eternity,", 25)) < 0)
    {
      fprintf(stderr, "scheduler_2: cannot create message (stage 2 demo "
              "line 1) "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
    return status;
  }

  if (t == 2030)
  {
    if (tenm_table_add(message_new(((double) WINDOW_WIDTH) - 1.0, 270.0,
                                   -5.0, 0.0, 1000,
                                   "an escape from the chain of logic.",
                                   34)) < 0)
    {
      fprintf(stderr, "scheduler_2: cannot create message (stage 2 demo "
              "line 2) "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
    return status;
  }

  if (t == 2219)
  {
    if (tenm_table_add(stage_2_more_1_new()) < 0)
    {
      fprintf(stderr, "scheduler_2: cannot create stage_2_more_1 "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
    return status;
  }
  
  if ((t >= 2304) && (t <= 4320) && (t % 16 == 0))
  {
    for (i = 0; i < 2; i++)
    {
      if (i == 0)
        x = 0.0;
      else
        x = ((double) WINDOW_WIDTH);

      if (tenm_table_add(wall_new(x, 60.0, 60.0, SCROLL_SPEED_FAST)) < 0)
      {
        fprintf(stderr, "scheduler_2: cannot create wall (%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
  }

  if ((t >= 2400) && (t <= 2544) && (t % 24 == 0))
  {
    x = ((double) WINDOW_WIDTH) - 188.0;

    if (tenm_table_add(wall_new(x, 60.0, 60.0, SCROLL_SPEED_FAST)) < 0)
    {
      fprintf(stderr, "scheduler_2: cannot create wall "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
  }

  if ((t >= 2400) && (t <= 2538) && (t % 6 == 0))
  {
    if (t == 2538)
      color = COLOR_PURPLE;
    else
      color = COLOR_BLUE;

    if (t % 24 == 0)
      x = ((double) WINDOW_WIDTH) - 141.0;
    else if ((t % 24 == 6) || (t % 24 == 18))
      x = ((double) WINDOW_WIDTH) - 94.0;
    else
      x = ((double) WINDOW_WIDTH) - 47.0;

    if (tenm_table_add(normal_enemy_new(x, -14.0,
                                        NORMAL_BALL_SMALL, 0, color,
                                        0, -1, -1, 1, 1,
                                        /* move 0 */
                                        1000, 0.0, SCROLL_SPEED_FAST, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 0,
                                        /* shoot 0 */
                                        1000, 1000, 0, 0, 0)) < 0)
    {
      fprintf(stderr, "scheduler_2: cannot create normal_enemy "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
  }

  if ((t >= 2640) && (t <= 2784) && (t % 24 == 0))
  {
    x = 188.0;

    if (tenm_table_add(wall_new(x, 60.0, 60.0, SCROLL_SPEED_FAST)) < 0)
    {
      fprintf(stderr, "scheduler_2: cannot create wall "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
  }

  if ((t >= 2640) && (t <= 2778) && (t % 6 == 0))
  {
    if (t == 2778)
      color = COLOR_PURPLE;
    else
      color = COLOR_RED;

    if (t % 24 == 0)
      x = 141.0;
    else if ((t % 24 == 6) || (t % 24 == 18))
      x = 94.0;
    else
      x = 47.0;

    if (tenm_table_add(normal_enemy_new(x, -14.0,
                                        NORMAL_BALL_SMALL, 0, color,
                                        0, -1, -1, 1, 1,
                                        /* move 0 */
                                        1000, 0.0, SCROLL_SPEED_FAST, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 0,
                                        /* shoot 0 */
                                        1000, 1000, 0, 0, 0)) < 0)
    {
      fprintf(stderr, "scheduler_2: cannot create normal_enemy "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
  }

  if ((t >= 2880) && (t <= 3024) && (t % 24 == 0))
  {
    x = ((double) WINDOW_WIDTH) - 188.0;

    if (tenm_table_add(wall_new(x, 60.0, 60.0, SCROLL_SPEED_FAST)) < 0)
    {
      fprintf(stderr, "scheduler_2: cannot create wall "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
  }

  if ((t >= 2880) && (t <= 3024) && (t % 12 == 0))
  {
    if (t == 2880)
      color = COLOR_PURPLE;
    else if (t == 2892)
      color = COLOR_RED;
    else
      color = COLOR_BLUE;

    for (i = 0; i < 2; i++)
    {
      if (i == 0)
        x = ((double) WINDOW_WIDTH) - 141.0;
      else
        x = ((double) WINDOW_WIDTH) - 47.0;

      if (tenm_table_add(normal_enemy_new(x, -14.0,
                                          NORMAL_BALL_SMALL, 0, color,
                                          0, -1, -1, 1, 1,
                                          /* move 0 */
                                          1000, 0.0, SCROLL_SPEED_FAST,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 0,
                                          /* shoot 0 */
                                          1000, 1000, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_2: cannot create normal_enemy "
                "(t = %d)\n", t);
        status = SCHEDULER_ERROR;
      }
    }
  }

  if ((t >= 3120) && (t <= 3264) && (t % 24 == 0))
  {
    x = 188.0;

    if (tenm_table_add(wall_new(x, 60.0, 60.0, SCROLL_SPEED_FAST)) < 0)
    {
      fprintf(stderr, "scheduler_2: cannot create wall "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
  }

  if ((t >= 3120) && (t <= 3264) && (t % 12 == 0))
  {
    if (t == 3120)
      color = COLOR_PURPLE;
    else if (t == 3132)
      color = COLOR_BLUE;
    else
      color = COLOR_RED;

    for (i = 0; i < 2; i++)
    {
      if (i == 0)
        x = 141.0;
      else
        x = 47.0;

      if (tenm_table_add(normal_enemy_new(x, -14.0,
                                          NORMAL_BALL_SMALL, 0, color,
                                          0, -1, -1, 1, 1,
                                          /* move 0 */
                                          1000, 0.0, SCROLL_SPEED_FAST,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 0,
                                          /* shoot 0 */
                                          1000, 1000, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_2: cannot create normal_enemy "
                "(t = %d)\n", t);
        status = SCHEDULER_ERROR;
      }
    }
  }

  if ((t >= 3360) && (t <= 3392) && (t % 8 == 0))
  {
    for (i = 0; i < 2; i++)
    {
      if (t == 3392)
        color = COLOR_PURPLE;
      else if (i == 0)
        color = COLOR_BLUE;
      else
        color = COLOR_RED;

      if (i == 0)
        x = 87.0;
      else
        x = ((double) WINDOW_WIDTH) - 87.0;

      if (tenm_table_add(normal_enemy_new(x, -14.0,
                                          NORMAL_STING, 0, color,
                                          0, -1, -1, 3, 1,
                                          /* move 0 */
                                          40 - (t - 3360),
                                          0.0, SCROLL_SPEED_FAST,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          50, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* move 2 */
                                          1000, 0.0, SCROLL_SPEED_FAST,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* shoot 0 */
                                          1000, 30, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_2: cannot create normal_enemy "
                "(t = %d)\n", t);
        status = SCHEDULER_ERROR;
      }
    }
  }

  if ((((t >= 3552) && (t <= 3648)) || ((t >= 3792) && (t <= 3888)))
      && (t % 8 == 0))
  {
    if (t <= 3648)
      x = 240.0;
    else
      x = ((double) WINDOW_WIDTH) - 240.0;

    if (tenm_table_add(wall_new(x, 60.0, 60.0, SCROLL_SPEED_FAST)) < 0)
    {
      fprintf(stderr, "scheduler_2: cannot create wall "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
  }

  if ((t == 3552) || (t == 3648) || (t == 3792) || (t == 3888))
  {
    for (i = 0; i < 2; i++)
    {
      if (t <= 3648)
      {
        if (i == 0)
          x = 90.0;
        else
          x = 150.0;
      }
      else
      {
        if (i == 0)
          x = ((double) WINDOW_WIDTH) - 90.0;
        else
          x = ((double) WINDOW_WIDTH) - 150.0;
      }

      if (tenm_table_add(wall_new(x, 60.0, 60.0, SCROLL_SPEED_FAST)) < 0)
      {
        fprintf(stderr, "scheduler_2: cannot create wall (%d)"
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
    
  }

  if ((t == 3561) || (t == 3577) || (t == 3590) || (t == 3606)
      || (t == 3619) || (t == 3635)
      || (t == 3801) || (t == 3817) || (t == 3830) || (t == 3846)
      || (t == 3859) || (t == 3875))
  {
    if (t <= 3635)
    {
      color = COLOR_RED;
      x = 120.0;
      center_x = 120.0;
    }
    else
    {
      color = COLOR_BLUE;
      x = ((double) WINDOW_WIDTH) - 120.0;
      center_x = ((double) WINDOW_WIDTH) - 120.0;
    }
    
    if (t <= 3577)
      t_start = 3561;
    else if (t <= 3610)
      t_start = 3590;
    else if (t <= 3635)
      t_start = 3619;
    else if (t <= 3817)
      t_start = 3801;
    else if (t <= 3846)
      t_start = 3830;
    else
      t_start = 3859;
    

    if ((t == 3590) || (t == 3606) || (t == 3830) || (t == 3846))
      speed_theta = -0.5;
    else
      speed_theta = 0.5;

    if (tenm_table_add(normal_enemy_new(x, -24.0,
                                        NORMAL_BALL_LARGE, 0, color,
                                        0, -1, -1, 2, 1,
                                        /* move 0 */
                                        24 - (t - t_start),
                                        0.0, SCROLL_SPEED_FAST,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 1,
                                        /* move 1 */
                                        1000, 0.0, SCROLL_SPEED_FAST,
                                        0.0, 0.0,
                                        center_x, 96.0, 0.0, speed_theta, 1,
                                        /* shoot 0 */
                                        1000, 50, 0, 0, 0)) < 0)
    {
      fprintf(stderr, "scheduler_2: cannot create normal_enemy "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
  }

  if ((t == 3569) || (t == 3598) || (t == 3627)
      || (t == 3809) || (t == 3838) || (t == 3867))
  {
    color = COLOR_PURPLE;
    if (t <= 3627)
      x = 120.0;
    else
      x = ((double) WINDOW_WIDTH) - 120.0;

    if (tenm_table_add(normal_enemy_new(x, -24.0,
                                        NORMAL_BALL_LARGE, 0, color,
                                        0, -1, -1, 1, 1,
                                        /* move 0 */
                                        1000, 0.0, SCROLL_SPEED_FAST,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 0,
                                        /* shoot 0 */
                                        1000, 50, 0, 0, 0)) < 0)
    {
      fprintf(stderr, "scheduler_2: cannot create normal_enemy "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
  }

  if ((t == 3565) || (t == 3573) || (t == 3594) || (t == 3602)
      || (t == 3623) || (t == 3631)
      || (t == 3805) || (t == 3813) || (t == 3834) || (t == 3842)
      || (t == 3863) || (t == 3871))
  {
    color = COLOR_RED;

    if (t <= 3577)
      t_start = 3561;
    else if (t <= 3610)
      t_start = 3590;
    else if (t <= 3631)
      t_start = 3619;
    else if (t <= 3813)
      t_start = 3801;
    else if (t <= 3842)
      t_start = 3830;
    else
      t_start = 3859;

    if ((t == 3594) || (t == 3602) || (t == 3834) || (t == 3842))
      speed_theta = -0.5;
    else
      speed_theta = 0.5;

    for (i = 0; i < 2; i++)
    {
      if (t <= 3631)
      {
        color = COLOR_RED;
        center_x = 120.0;
        if (i == 0)
          x = 120.0 - 51.9615;
        else
          x = 120.0 + 51.9615;
      }
      else
      {
        color = COLOR_BLUE;
        center_x = ((double) WINDOW_WIDTH) - 120.0;
        if (i == 0)
          x = ((double) WINDOW_WIDTH) - (120.0 - 51.9615);
        else
          x = ((double) WINDOW_WIDTH) - (120.0 + 51.9615);
      }
      
      if (tenm_table_add(normal_enemy_new(x, -24.0,
                                          NORMAL_BALL_LARGE, 0, color,
                                          0, -1, -1, 2, 1,
                                          /* move 0 */
                                          24 - (t - t_start),
                                          0.0, SCROLL_SPEED_FAST,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          1000, 0.0, SCROLL_SPEED_FAST,
                                          0.0, 0.0,
                                          center_x, 96.0, 0.0, speed_theta, 1,
                                          /* shoot 0 */
                                          1000, 50, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_2: cannot create normal_enemy (%d)"
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
  }

  if ((t == 4048) || (t == 4112) || (t == 4176) || (t == 4240) || (t == 4304))
  {
    for (i = 1; i <= 3; i++)
    {
      if ((t == 4048) || (t == 4176) || (t == 4304))
        x = ((double) (i * 85));
      else
        x = ((double) (WINDOW_WIDTH - i * 85));

      if (tenm_table_add(wall_new(x, 60.0, 60.0, SCROLL_SPEED_FAST)) < 0)
      {
        fprintf(stderr, "scheduler_2: cannot create wall (%d)"
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
  }

  if ((t == 4060) || (t == 4068) || (t == 4076)
      || (t == 4124) || (t == 4132) || (t == 4140))
  {
    if ((t == 4060) || (t == 4124))
      i = 0;
    else if ((t == 4068) || (t == 4132))
      i = 1;
    else
      i = 2;
    for (; i < 3; i++)
    {
      if (t <= 4076)
      {
        color = COLOR_RED;
        x = ((double) (180 - i * 60));
      }
      else
      {
        color = COLOR_BLUE;
        x = ((double) (WINDOW_WIDTH - (180 - i * 60)));
      }
      
      if (tenm_table_add(normal_enemy_new(x, -24.0,
                                          NORMAL_BALL_LARGE, 0, color,
                                          0, -1, -1, 1, 1,
                                          /* move 0 */
                                          1000, 0.0, SCROLL_SPEED_FAST,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 0,
                                          /* shoot 0 */
                                          1000, 25, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_2: cannot create normal_enemy (%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
  }

  if (t == 4092)
  {
    color = COLOR_PURPLE;
    x = ((double) WINDOW_WIDTH) / 2.0;
      
    if (tenm_table_add(normal_enemy_new(x, -24.0,
                                        NORMAL_BALL_LARGE, 0, color,
                                        0, -1, -1, 1, 1,
                                        /* move 0 */
                                        1000, 0.0, SCROLL_SPEED_FAST,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 0,
                                        /* shoot 0 */
                                        1000, 25, 0, 0, 0)) < 0)
    {
      fprintf(stderr, "scheduler_2: cannot create normal_enemy "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
  }

  if (t == 4264)
  {
    if (tenm_table_add(normal_enemy_new(-24.0, -24.0,
                                        NORMAL_HORIZON, 0, COLOR_PURPLE,
                                        0, -1, -1, 1, 2,
                                        /* move 0 */
                                        1000, 3.0, SCROLL_SPEED_FAST,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 0,
                                        /* shoot 0 */
                                        100, 1, -30, 0, 1,
                                        /* shoot 1 */
                                        1000, 1, -1000, 0, 1)) < 0)
    {
      fprintf(stderr, "scheduler_2: cannot create normal_enemy (0)"
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }

    if (tenm_table_add(normal_enemy_new(((double) WINDOW_WIDTH) + 24.0, -24.0,
                                        NORMAL_HORIZON, 0, COLOR_PURPLE,
                                        0, -1, -1, 1, 2,
                                        /* move 0 */
                                        1000, -2.5, SCROLL_SPEED_FAST,
                                        0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 0,
                                        /* shoot 0 */
                                        10, 1, -1000, 0, 1,
                                        /* shoot 1 */
                                        1000, 1, -50, 0, 1)) < 0)
    {
      fprintf(stderr, "scheduler_2: cannot create normal_enemy (1)"
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }

  }



  return status;
}

static tenm_object *
stage_2_more_1_new(void)
{
  tenm_object *new = NULL;
  int *count = NULL;

  count = (int *) malloc(sizeof(int) * 4);
  if (count == NULL)
  {
    fprintf(stderr, "stage_2_more_1_new: malloc(count) failed\n");
    return NULL;
  }

  /* list of count
   * [0] total timer
   * [1] mode timer
   * [2] mode
   * [3] killed
   */
  count[0] = 0;
  count[1] = 0;
  count[2] = 0;
  count[3] = 0;

  new = tenm_object_new("stage 2 more 1", 0, 0,
                        0, 0.0, 0.0,
                        4, count, 0, NULL, 0, NULL, 
                        (int (*)(tenm_object *, double)) NULL,
                        (int (*)(tenm_object *, tenm_object *)) NULL,
                        (int (*)(tenm_object *, const tenm_object *))
                        stage_2_more_1_act,
                        (int (*)(tenm_object *, int)) NULL);
  if (new == NULL)
  {
    fprintf(stderr, "stage_2_more_1_new: tenm_object_new failed\n");
    free(count);
    return NULL;
  }

  return new;
}

static int
stage_2_more_1_act(tenm_object *my, const tenm_object *player)
{
  int i;
  double x;
  double dx;
  double dy;
  double ddx;

  /* sanity check */
  if (my == NULL)
    return 0;

  (my->count[1])++;
  /* max about 2313 frame */
  if (my->count[2] == 0)
  {
    if (my->count[1] == 1)
    {
      if (tenm_table_add(intuition_new(my->table_index)) < 0)
        fprintf(stderr, "stage_2_more_1_act: cannot create intuition "
                "(%d, %d)\n", my->count[2], my->count[1]);
    }
    else if (my->count[3] == 1)
    {
      if (my->count[1] <= 2250)
      {
        my->count[1]= 0;
        my->count[2] = 1;
      }
      else
      {
        my->count[1]= 70;
        my->count[2] = 2;
      }
    }
  }
  /* max about 262 frame */
  else if (my->count[2] == 1)
  {
    if ((my->count[1] <= 126) && (my->count[1] % 5 == 1))
    {
      dy = 5.0 + ((double) (my->count[1] % 20)) / 10.0;

      for (i = 0; i < 2; i++)
      {
        if (i == 0)
        {
          x = 80.0;
          dx = -4.0;
          ddx = 0.14;
        }
        else
        {
          x = ((double) WINDOW_WIDTH) - 80.0;
          dx = 4.0;
          ddx = -0.14;
        }
        
        if (tenm_table_add(normal_enemy_new(x, -19.0,
                                            NORMAL_BALL_MEDIUM,
                                            ENEMY_TYPE_WEAK,
                                            COLOR_PURPLE,
                                            0, -1, -1, 1, 1,
                                            /* move 0 */
                                            1000, dx, dy,
                                            ddx, 0.0,
                                            0.0, 0.0, 0.0, 0.0, 0,
                                            /* shoot 0 */
                                            1000, 25, 0, 50, 0)) < 0)
          fprintf(stderr, "stage_2_more_1_act: cannot create normal_enemy "
                  "(%d) "
                  "(%d, %d)\n", i, my->count[2], my->count[1]);
      }
    }
    if ((my->count[0] >= 2100) && (my->count[1] >= 126))
    {
      my->count[1] = 0;
      my->count[2] = 2;
    }
  }
  else if (my->count[2] == 2)
  {
    if (my->count[1] == 150)
    {
      if (tenm_table_add(message_new(((double) WINDOW_WIDTH) - 1.0, 270.0,
                                     -5.0, 0.0, 1000,
                                     "boss: Overrun", 14)) < 0)
      {
        fprintf(stderr, "stage_1_more_5_act: cannot create message "
                "(%d, %d)\n", my->count[2], my->count[1]);
      }
    }
    else if (my->count[1] == 300)
    {
      if (tenm_table_add(overrun_new()) < 0)
      {
        fprintf(stderr, "stage_1_more_5_act: cannot create overrun "
                "(%d, %d)\n", my->count[2], my->count[1]);
      }
    }
  }
  else
  {
    fprintf(stderr, "stage_2_more_1_act: strange (%d) ", my->count[2]);
  }
  
  (my->count[0])++;
  if (my->count[0] >= 3000)
    return 1;

  return 0;
}
