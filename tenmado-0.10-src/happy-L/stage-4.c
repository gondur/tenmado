/* $Id: stage-4.c,v 1.113 2002/07/07 15:53:57 oohara Exp $ */

#include <stdio.h>
/* malloc */
#include <stdlib.h>

#include "tenm_table.h"
#include "const.h"
#include "message.h"
#include "normal-enemy.h"
#include "tenm_object.h"
#include "close-system.h"
#include "fud.h"

#include "stage-4.h"

int
scheduler_4(int t)
{
  int i;
  int n;
  int status = SCHEDULER_SUCCESS;
  int t_start;
  int time_shoot;
  int time_shoot_initial;
  int color;
  double x;
  double y;
  double dx;
  double mirror;
  int direction;

  /* sanity check */
  if (t < 0)
  {
    fprintf(stderr, "scheduler_4: strange t (%d)\n", t);
    return SCHEDULER_ERROR;
  }

  if (t == 30)
  {
    if (tenm_table_add(message_new(210.0, 240.0,
                                   0.0, 0.0, 100, "stage 4", 7)) < 0)
    {
      fprintf(stderr, "scheduler_4: cannot create message \"stage 4\""
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
    if (tenm_table_add(message_new(200.0, 270.0,
                                   0.0, 0.0, 100,
                                   "another L", 9)) < 0)
    {
      fprintf(stderr, "scheduler_4: cannot create message (stage 4 title) "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
    return status;
  }

  /* x or X: move
   * . or |: not move
   * 11 frame per character
   *
   * x.xX..xXx.xXx.xX
   * x.xX.xxXx.xX.xx|
   */

  if (t < 1588)
  {    
    if (t < 532)
    {  
      t_start = 180;
      mirror = 1.0;
    }
    else if (t < 884)
    {
      t_start = 532;
      mirror = -1.0;
    }
    else if (t < 1236)
    {  
      t_start = 884;
      mirror = 1.0;
    }
    else
    {
      t_start = 1236;
      mirror = -1.0;
    }

    time_shoot = 33;
    time_shoot_initial = 0;

    /* B1 */
    if (t == t_start + 0)
    {
      x = ((double) WINDOW_WIDTH) / 2.0 - 86.0 * mirror;
      y = -18.0;
      if (mirror > 0.0)
        color = COLOR_BLUE;
      else
        color = COLOR_RED;
      if (tenm_table_add(normal_enemy_new(x, y,
                                          NORMAL_BALL_MEDIUM, 0, color,
                                          0, -1, -1, 15, 1,
                                          /* move 0 */
                                          8, 0.0, 10.75, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          58, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* move 2 */
                                          8, 21.5 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 3,
                                          /* move 3 */
                                          36, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 4,
                                          /* move 4 */
                                          8, 0.0, 10.75, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 5,
                                          /* move 5 */
                                          14, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 6,
                                          /* move 6 */
                                          8, 0.0, 10.75, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 7,
                                          /* move 7 */
                                          58, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 8,
                                          /* move 8 */
                                          8, -21.5 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 9,
                                          /* move 9 */
                                          25, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 10,
                                          /* move 10 */
                                          8, 0.0, -21.5, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 11,
                                          /* move 11 */
                                          3, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 12,
                                          /* move 12 */
                                          8, 21.5 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 13,
                                          /* move 13 */
                                          36, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 14,
                                          /* move 14 */
                                          1000, 10.75 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 14,
                                          /* shoot 0 */
                                          1000, time_shoot, time_shoot_initial,
                                          0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_4: cannot create normal_enemy "
                "(t = %d)\n", t);
        status = SCHEDULER_ERROR;
      }
      return status;
    }

    /* R2 */
    if (t == t_start + 22)
    {
      x = ((double) WINDOW_WIDTH) / 2.0 - 86.0 * mirror;
      y = -18.0;
      if (mirror > 0.0)
        color = COLOR_RED;
      else
        color = COLOR_BLUE;
      if (tenm_table_add(normal_enemy_new(x, y,
                                          NORMAL_BALL_MEDIUM, 0, color,
                                          0, -1, -1, 11, 1,
                                          /* move 0 */
                                          8, 0.0, 10.75, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          47, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* move 2 */
                                          8, 0.0, 21.5, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 3,
                                          /* move 3 */
                                          36, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 4,
                                          /* move 4 */
                                          8, 10.75 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 5,
                                          /* move 5 */
                                          25, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 6,
                                          /* move 6 */
                                          8, 10.75 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 7,
                                          /* move 7 */
                                          69, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 8,
                                          /* move 8 */
                                          8, 0.0, -10.75, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 9,
                                          /* move 9 */
                                          47, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 10,
                                          /* move 10 */
                                          1000, 10.75 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 10,
                                          /* shoot 0 */
                                          1000, time_shoot, time_shoot_initial,
                                          0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_4: cannot create normal_enemy "
                "(t = %d)\n", t);
        status = SCHEDULER_ERROR;
      }
      return status;
    }

    /* B3 */
    if (t == t_start + 33)
    {
      x = ((double) WINDOW_WIDTH) / 2.0 - 86.0 * mirror;
      y = -18.0;
      if (mirror > 0.0)
        color = COLOR_BLUE;
      else
        color = COLOR_RED;
      if (tenm_table_add(normal_enemy_new(x, y,
                                          NORMAL_BALL_MEDIUM, 0, color,
                                          0, -1, -1, 9, 1,
                                          /* move 0 */
                                          8, 0.0, 10.75, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          47, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* move 2 */
                                          8, 10.75 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 3,
                                          /* move 3 */
                                          36, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 4,
                                          /* move 4 */
                                          8, 10.75 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 5,
                                          /* move 5 */
                                          25, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 6,
                                          /* move 6 */
                                          8, 0.0, 21.5, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 7,
                                          /* move 7 */
                                          113, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 8,
                                          /* move 8 */
                                          1000, 10.75 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 8,
                                          /* shoot 0 */
                                          1000, time_shoot, time_shoot_initial,
                                          0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_4: cannot create normal_enemy "
                "(t = %d)\n", t);
        status = SCHEDULER_ERROR;
      }
      return status;
    }

    /* R4 */
    if (t == t_start + 66)
    {
      if (mirror > 0.0)
        x = ((double) WINDOW_WIDTH) + 18.0;
      else
        x = -18.0;
      y = 240.0;
      if (mirror > 0.0)
        color = COLOR_RED;
      else
        color = COLOR_BLUE;
      if (tenm_table_add(normal_enemy_new(x, y,
                                          NORMAL_BALL_MEDIUM, 0, color,
                                          0, -1, -1, 15, 1,
                                          /* move 0 */
                                          8, -21.5 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          36, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* move 2 */
                                          8, 0.0, -21.5, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 3,
                                          /* move 3 */
                                          14, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 4,
                                          /* move 4 */
                                          8, -21.5 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 5,
                                          /* move 5 */
                                          36, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 6,
                                          /* move 6 */
                                          8, 0.0, 21.5, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 7,
                                          /* move 7 */
                                          14, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 8,
                                          /* move 8 */
                                          8, 21.5 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 9,
                                          /* move 9 */
                                          36, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 10,
                                          /* move 10 */
                                          8, 0.0, -21.5, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 11,
                                          /* move 11 */
                                          3, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 12,
                                          /* move 12 */
                                          8, -21.5 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 13,
                                          /* move 13 */
                                          25, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 14,
                                          /* move 14 */
                                          1000, -10.75 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 14,
                                          /* shoot 0 */
                                          1000, time_shoot, time_shoot_initial,
                                          0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_4: cannot create normal_enemy "
                "(t = %d)\n", t);
        status = SCHEDULER_ERROR;
      }
      return status;
    }

    /* B5 */
    if (t == t_start + 77)
    {
      if (mirror > 0.0)
        x = ((double) WINDOW_WIDTH) + 18.0;
      else
        x = -18.0;
      y = 240.0;
      if (mirror > 0.0)
        color = COLOR_BLUE;
      else
        color = COLOR_RED;
      if (tenm_table_add(normal_enemy_new(x, y,
                                          NORMAL_BALL_MEDIUM, 0, color,
                                          0, -1, -1, 15, 1,
                                          /* move 0 */
                                          8, -21.5 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          25, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* move 2 */
                                          8, -21.5 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 3,
                                          /* move 3 */
                                          36, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 4,
                                          /* move 4 */
                                          8, 0.0, -21.5, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 5,
                                          /* move 5 */
                                          14, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 6,
                                          /* move 6 */
                                          8, 21.5 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 7,
                                          /* move 7 */
                                          47, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 8,
                                          /* move 8 */
                                          8, 0.0, 21.5, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 9,
                                          /* move 9 */
                                          3, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 10,
                                          /* move 10 */
                                          8, -21.5 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 11,
                                          /* move 11 */
                                          3, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 12,
                                          /* move 12 */
                                          8, 0.0, -10.75, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 13,
                                          /* move 13 */
                                          25, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 14,
                                          /* move 14 */
                                          1000, -10.75 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 14,
                                          /* shoot 0 */
                                          1000, time_shoot, time_shoot_initial,
                                          0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_4: cannot create normal_enemy "
                "(t = %d)\n", t);
        status = SCHEDULER_ERROR;
      }
      return status;
    }
  
    /* R6 */
    if (t == t_start + 88)
    {
      if (mirror > 0.0)
        x = ((double) WINDOW_WIDTH) + 18.0;
      else
        x = -18.0;
      y = 240.0;
      if (mirror > 0.0)
        color = COLOR_RED;
      else
        color = COLOR_BLUE;
      if (tenm_table_add(normal_enemy_new(x, y,
                                          NORMAL_BALL_MEDIUM, 0, color,
                                          0, -1, -1, 11, 1,
                                          /* move 0 */
                                          8, -21.5 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          36, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* move 2 */
                                          8, 0.0, -21.5, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 3,
                                          /* move 3 */
                                          25, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 4,
                                          /* move 4 */
                                          8, -21.5 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 5,
                                          /* move 5 */
                                          58, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 6,
                                          /* move 6 */
                                          8, 0.0, 10.75, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 7,
                                          /* move 7 */
                                          14, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 8,
                                          /* move 8 */
                                          8, 0.0, 10.75, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 9,
                                          /* move 9 */
                                          25, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 10,
                                          /* move 10 */
                                          1000, -10.75 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 10,
                                          /* shoot 0 */
                                          1000, time_shoot, time_shoot_initial,
                                          0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_4: cannot create normal_enemy "
                "(t = %d)\n", t);
        status = SCHEDULER_ERROR;
      }
      return status;
    }

    if (t == t_start + 176)
    {
      /* B7 */
      x = ((double) WINDOW_WIDTH) / 2.0;
      y = -18.0;
      if (mirror > 0.0)
        color = COLOR_BLUE;
      else
        color = COLOR_RED;
      if (tenm_table_add(normal_enemy_new(x, y,
                                          NORMAL_BALL_MEDIUM, 0, color,
                                          0, -1, -1, 9, 1,
                                          /* move 0 */
                                          8, 0.0, 10.75, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          14, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* move 2 */
                                          8, -10.75 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 3,
                                          /* move 3 */
                                          3, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 4,
                                          /* move 4 */
                                          8, 0.0, 21.5, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 5,
                                          /* move 5 */
                                          25, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 6,
                                          /* move 6 */
                                          8, 10.75 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 7,
                                          /* move 7 */
                                          47, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 8,
                                          /* move 8 */
                                          1000, 0.0, 10.75, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 8,
                                          /* shoot 0 */
                                          1000, time_shoot, time_shoot_initial,
                                          0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_4: cannot create normal_enemy (0) "
                "(t = %d)\n", t);
        status = SCHEDULER_ERROR;
      }
  
      /* B8 */
      if (mirror > 0.0)
        x = -18.0;
      else
        x = ((double) WINDOW_WIDTH) + 18.0;
      y = 154.0;
      if (mirror > 0.0)
        color = COLOR_BLUE;
      else
        color = COLOR_RED;
      if (tenm_table_add(normal_enemy_new(x, y,
                                          NORMAL_BALL_MEDIUM, 0, color,
                                          0, -1, -1, 11, 1,
                                          /* move 0 */
                                          8, 21.5 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          14, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* move 2 */
                                          8, 0.0, 10.75, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 3,
                                          /* move 3 */
                                          3, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 4,
                                          /* move 4 */
                                          8, 10.75 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 5,
                                          /* move 5 */
                                          25, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 6,
                                          /* move 6 */
                                          8, 10.75 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 7,
                                          /* move 7 */
                                          3, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 8,
                                          /* move 8 */
                                          8, 0.0, -10.75, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 9,
                                          /* move 9 */
                                          36, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 10,
                                          /* move 10 */
                                          1000, 0.0, 10.75, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 10,
                                          /* shoot 0 */
                                          1000, time_shoot, time_shoot_initial,
                                          0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_4: cannot create normal_enemy (1) "
                "(t = %d)\n", t);
        status = SCHEDULER_ERROR;
      }

      /* B9 */
      if (mirror > 0.0)
        x = ((double) WINDOW_WIDTH) + 18.0;
      else
        x = -18.0;
      y = 154.0;
      if (mirror > 0.0)
        color = COLOR_BLUE;
      else
        color = COLOR_RED;
      if (tenm_table_add(normal_enemy_new(x, y,
                                          NORMAL_BALL_MEDIUM, 0, color,
                                          0, -1, -1, 11, 1,
                                          /* move 0 */
                                          8, -21.5 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          14, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* move 2 */
                                          8, 0.0, -10.75, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 3,
                                          /* move 3 */
                                          3, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 4,
                                          /* move 4 */
                                          8, -10.75 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 5,
                                          /* move 5 */
                                          25, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 6,
                                          /* move 6 */
                                          8, -10.75 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 7,
                                          /* move 7 */
                                          3, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 8,
                                          /* move 8 */
                                          8, 0.0, 10.75, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 9,
                                          /* move 9 */
                                          36, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 10,
                                          /* move 10 */
                                          1000, 0.0, 10.75, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 10,
                                          /* shoot 0 */
                                          1000, time_shoot, time_shoot_initial,
                                          0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_4: cannot create normal_enemy (2) "
                "(t = %d)\n", t);
        status = SCHEDULER_ERROR;
      }

      return status;
    }

    if (t == t_start + 198)
    {
      /* R10 */
      x = ((double) WINDOW_WIDTH) / 2.0;
      y = -18.0;
      if (mirror > 0.0)
        color = COLOR_RED;
      else
        color = COLOR_BLUE;
      if (tenm_table_add(normal_enemy_new(x, y,
                                          NORMAL_BALL_MEDIUM, 0, color,
                                          0, -1, -1, 9, 1,
                                          /* move 0 */
                                          8, 0.0, 10.75, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          3, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* move 2 */
                                          8, 10.75 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 3,
                                          /* move 3 */
                                          25, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 4,
                                          /* move 4 */
                                          8, 0.0, 21.5, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 5,
                                          /* move 5 */
                                          3, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 6,
                                          /* move 6 */
                                          8, -10.75 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 7,
                                          /* move 7 */
                                          58, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 8,
                                          /* move 8 */
                                          1000, 0.0, 10.75, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 8,
                                          /* shoot 0 */
                                          1000, time_shoot, time_shoot_initial,
                                          0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_4: cannot create normal_enemy (0) "
                "(t = %d)\n", t);
        status = SCHEDULER_ERROR;
      }

      /* R11 */
      if (mirror > 0)
        x = -18.0;
      else
        x = ((double) WINDOW_WIDTH) + 18.0;
      y = 154.0;
      if (mirror > 0.0)
        color = COLOR_RED;
      else
        color = COLOR_BLUE;
      if (tenm_table_add(normal_enemy_new(x, y,
                                          NORMAL_BALL_MEDIUM, 0, color,
                                          0, -1, -1, 9, 1,
                                          /* move 0 */
                                          8, 21.5 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          3, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* move 2 */
                                          8, 0.0, -10.75, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 3,
                                          /* move 3 */
                                          36, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 4,
                                          /* move 4 */
                                          8, 21.5 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 5,
                                          /* move 5 */
                                          3, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 6,
                                          /* move 6 */
                                          8, 0.0, 10.75, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 7,
                                          /* move 7 */
                                          47, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 8,
                                          /* move 8 */
                                          1000, 0.0, 10.75, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 8,
                                          /* shoot 0 */
                                          1000, time_shoot, time_shoot_initial,
                                          0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_4: cannot create normal_enemy (1) "
                "(t = %d)\n", t);
        status = SCHEDULER_ERROR;
      }

      /* R12 */
      if (mirror > 0.0)
        x = ((double) WINDOW_WIDTH) + 18.0;
      else
        x = -18.0;
      y = 154.0;
      if (mirror > 0.0)
        color = COLOR_RED;
      else
        color = COLOR_BLUE;
      if (tenm_table_add(normal_enemy_new(x, y,
                                          NORMAL_BALL_MEDIUM, 0, color,
                                          0, -1, -1, 11, 1,
                                          /* move 0 */
                                          8, -21.5 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          3, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* move 2 */
                                          8, 0.0, 10.75, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 3,
                                          /* move 3 */
                                          14, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 4,
                                          /* move 4 */
                                          8, -10.75 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 5,
                                          /* move 5 */
                                          14, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 6,
                                          /* move 6 */
                                          8, -10.75 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 7,
                                          /* move 7 */
                                          3, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 8,
                                          /* move 8 */
                                          8, 0.0, -10.75, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 9,
                                          /* move 9 */
                                          47, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 10,
                                          /* move 10 */
                                          1000, 0.0, 10.75, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 10,
                                          /* shoot 0 */
                                          1000, time_shoot, time_shoot_initial,
                                          0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_4: cannot create normal_enemy (2) "
                "(t = %d)\n", t);
        status = SCHEDULER_ERROR;
      }

      return status;
    }

    if (t == t_start + 209)
    {
      /* P13 */
      x = ((double) WINDOW_WIDTH) / 2.0;
      y = -18.0;
      color = COLOR_PURPLE;
      if (tenm_table_add(normal_enemy_new(x, y,
                                          NORMAL_BALL_MEDIUM, 0, color,
                                          0, -1, -1, 9, 1,
                                          /* move 0 */
                                          8, 0.0, 10.75, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          14, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* move 2 */
                                          8, -10.75 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 3,
                                          /* move 3 */
                                          25, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 4,
                                          /* move 4 */
                                          8, 0.0, 21.5, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 5,
                                          /* move 5 */
                                          14, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 6,
                                          /* move 6 */
                                          8, 10.75 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 7,
                                          /* move 7 */
                                          36, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 8,
                                          /* move 8 */
                                          1000, 0.0, 10.75, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 8,
                                          /* shoot 0 */
                                          1000, time_shoot, time_shoot_initial,
                                          0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_4: cannot create normal_enemy (0) "
                "(t = %d)\n", t);
        status = SCHEDULER_ERROR;
      }

      /* P14 */
      if (mirror > 0.0)
        x = -18.0;
      else
        x = ((double) WINDOW_WIDTH) + 18.0;
      y = 154.0;
      color = COLOR_PURPLE;
      if (tenm_table_add(normal_enemy_new(x, y,
                                          NORMAL_BALL_MEDIUM, 0, color,
                                          0, -1, -1, 9, 1,
                                          /* move 0 */
                                          8, 21.5 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          14, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* move 2 */
                                          8, 0.0, 10.75, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 3,
                                          /* move 3 */
                                          25, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 4,
                                          /* move 4 */
                                          8, 21.5 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 5,
                                          /* move 5 */
                                          14, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 6,
                                          /* move 6 */
                                          8, 0.0, -10.75, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 7,
                                          /* move 7 */
                                          36, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 8,
                                          /* move 8 */
                                          1000, 0.0, 10.75, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 8,
                                          /* shoot 0 */
                                          1000, time_shoot, time_shoot_initial,
                                          0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_4: cannot create normal_enemy (1) "
                "(t = %d)\n", t);
        status = SCHEDULER_ERROR;
      }

      /* P15 */
      if (mirror > 0.0)
        x = ((double) WINDOW_WIDTH) + 18.0;
      else
        x = -18.0;
      y = 154.0;
      color = COLOR_PURPLE;
      if (tenm_table_add(normal_enemy_new(x, y,
                                          NORMAL_BALL_MEDIUM, 0, color,
                                          0, -1, -1, 9, 1,
                                          /* move 0 */
                                          8, -21.5 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          25, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* move 2 */
                                          8, 0.0, -10.75, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 3,
                                          /* move 3 */
                                          36, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 4,
                                          /* move 4 */
                                          8, -21.5 * mirror, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 5,
                                          /* move 5 */
                                          3, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 6,
                                          /* move 6 */
                                          8, 0.0, 10.75, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 7,
                                          /* move 7 */
                                          25, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 8,
                                          /* move 8 */
                                          1000, 0.0, 10.75, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 8,
                                          /* shoot 0 */
                                          1000, time_shoot, time_shoot_initial,
                                          0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_4: cannot create normal_enemy (2) "
                "(t = %d)\n", t);
        status = SCHEDULER_ERROR;
      }

      return status;
    }
  }

  if (t == 1600)
  {
    if (tenm_table_add(message_new(((double) WINDOW_WIDTH) - 1.0, 240.0,
                                   -5.0, 0.0, 1000,
                                   "It's a system of closing.", 25)) < 0)
    {
      fprintf(stderr, "scheduler_4: cannot create message (stage 4 demo "
              "line 1) "
              "(t = %d)\n", t);
      return SCHEDULER_ERROR;
    }
    return SCHEDULER_SUCCESS;
  }

  t_start = 1750;

  if (t == t_start)
  {
    for (i = 0; i < 4; i++)
    {
      if (tenm_table_add(close_system_wall_new(i)) < 0)
      {
        fprintf(stderr, "scheduler_4: cannot create close_system_wall (%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
    return status;
  }

  if ((t >= t_start + 100) && (t <= t_start + 450) && (t % 50 == 0))
  {
    for (i = 0; i < 2; i ++)
    {
      if (i == 0)
      {
        direction = CLOSE_SYSTEM_HORIZONTAL_FORWARD;
        time_shoot = 1000;
        if (t % 200 == 0)
          color = COLOR_PURPLE;
        else
          color = COLOR_BLUE;
      }
      else
      {
        direction = CLOSE_SYSTEM_HORIZONTAL_BACKWARD;
        time_shoot = 1000;
        if (t % 200 == 0)
          color = COLOR_PURPLE;
        else
          color = COLOR_RED;
      }

      if (tenm_table_add(close_system_ball_new(t - t_start, 2, direction,
                                               time_shoot, color))< 0)
      {
        fprintf(stderr, "scheduler_4: cannot create close_system_ball "
                "(t = %d)\n", t);
        status = SCHEDULER_ERROR;
      }
    }
    return status;
  }
  
  if ((t >= t_start + 460) && (t <= t_start + 1010) && (t % 50 == 10))
  {
    if (t <= t_start + 710)
      color = COLOR_BLUE;
    else if (t <= t_start + 760)
      color = COLOR_PURPLE;
    else
      color = COLOR_RED;

    time_shoot = 40;

    for (i = 0; i < 4; i ++)
    {
      if (i < 2)
      {
        direction = CLOSE_SYSTEM_HORIZONTAL_FORWARD;
      }
      else
      {
        direction = CLOSE_SYSTEM_HORIZONTAL_BACKWARD;
      }

      if (i % 2 == 0)
        n = -1;
      else
        n = 1;

      if (tenm_table_add(close_system_ball_new(t - t_start, n, direction,
                                               time_shoot, color))< 0)
      {
        fprintf(stderr, "scheduler_4: cannot create close_system_ball "
                "(t = %d)\n", t);
        status = SCHEDULER_ERROR;
      }
    }
    return status;
  }

  if ((t >= t_start + 1020) && (t <= t_start + 1570) && (t % 50 == 20))
  {
    direction = CLOSE_SYSTEM_VERTICAL_FORWARD;
    for (i = 0; i < 4; i ++)
    {
      if (i == 0)
      {
        n = -2;
      }
      else if (i == 1)
      {
        n = -1; 
      }
      else if (i == 2)
      {
        n = 1;
      }
      else
      {
        n = 2;
      }

      if ((i == 1) || (i == 2))
      {
        time_shoot = 1000;
        if (t <= t_start + 1270)
          color = COLOR_RED;
        else if (t <= t_start + 1320)
          color = COLOR_PURPLE;
        else
          color = COLOR_BLUE;
      }
      else
      {
        time_shoot = 40;
        if (t % 100 == 70)
          color = COLOR_BLUE;
        else
          color = COLOR_RED;
      }
      
      if (tenm_table_add(close_system_ball_new(t - t_start, n, direction,
                                               time_shoot, color))< 0)
      {
        fprintf(stderr, "scheduler_4: cannot create close_system_ball "
                "(t = %d)\n", t);
        status = SCHEDULER_ERROR;
      }
    }
    return status;
  }

  if ((t >= t_start + 1620) && (t <= t_start + 2470) && (t % 50 == 20))
  {
    time_shoot = 40;
    direction = CLOSE_SYSTEM_VERTICAL_BACKWARD;
    for (i = 0; i < 2; i ++)
    {
      if (i == 0)
      {
        n = -2;
      }
      else
      {
        n = 2;
      }

      if (t % 100 == 20)
        color = COLOR_BLUE;
      else
        color = COLOR_RED;

      if (tenm_table_add(close_system_ball_new(t - t_start, n, direction,
                                               time_shoot, color))< 0)
      {
        fprintf(stderr, "scheduler_4: cannot create close_system_ball "
                "(t = %d)\n", t);
        status = SCHEDULER_ERROR;
      }
    }
  }

  if ((t >= t_start + 1620) && (t <= t_start + 2470) && (t % 25 == 20))
  {
    time_shoot = 1000;
    direction = CLOSE_SYSTEM_VERTICAL_FORWARD;
    n = 0;

    if (t % 100 == 70)
      color = COLOR_BLUE;
    else if (t % 100 == 20)
      color = COLOR_RED;
    else
      color = COLOR_PURPLE;
      
    if (tenm_table_add(close_system_ball_new(t - t_start, n, direction,
                                             time_shoot, color))< 0)
    {
      fprintf(stderr, "scheduler_4: cannot create close_system_ball "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
  }

  if ((((t >= t_start + 1720) && (t <= t_start + 1770))
       || ((t >= t_start + 1870) && (t <= t_start + 1920))
       || ((t >= t_start + 2020) && (t <= t_start + 2070))
       || ((t >= t_start + 2170) && (t <= t_start + 2220)))
      && (t % 25 == 20))
  {
    time_shoot = 1000;
    if ((t >= t_start + 1720) && (t <= t_start + 1920))
      direction = CLOSE_SYSTEM_VERTICAL_FORWARD;
    else
      direction = CLOSE_SYSTEM_VERTICAL_BACKWARD;
    if (((t >= t_start + 1720) && (t <= t_start + 1770))
        || ((t >= t_start + 2170) && (t <= t_start + 2220)))
      n = 1;
    else
      n = -1;

    if (((t >= t_start + 1720) && (t <= t_start + 1770))
        || ((t >= t_start + 2020) && (t <= t_start + 2070)))
      color = COLOR_BLUE;
    else
      color = COLOR_RED;

    if (tenm_table_add(close_system_ball_new(t - t_start, n, direction,
                                             time_shoot, color))< 0)
    {
      fprintf(stderr, "scheduler_4: cannot create close_system_ball "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
    return status;
  }

  if ((t >= t_start + 3458) && (t < t_start + 3758) && (t % 6 == 0))
  {
    for (i = 0; i < 2; i++)
    {
      if (i == 0)
      {
        x = -19.0;
        dx = 12.0;
      }
      else
      {
        x = ((double) WINDOW_WIDTH) + 19.0;
        dx = -12.0;
      }
      if (tenm_table_add(normal_enemy_new(x, 261.5,
                                          NORMAL_BALL_MEDIUM,
                                          ENEMY_TYPE_WEAK |ENEMY_TYPE_OBSTACLE,
                                          COLOR_PURPLE,
                                          0, -1, -1, 2, 1,
                                          /* move 0 */
                                          22, dx, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          1000, 0.0, 12.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* shoot 0 */
                                          1000, 20, 0, 50, 0)) < 0)
      {
        fprintf(stderr, "scheduler_4: cannot create normal_enemy (%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
    return status;
  }

  if ((t >= t_start + 3800) && (t <= t_start + 4775) && (t % 25 == 0))
  {
    for (i = 0; i < 3; i++)
    {
      if (i == 0)
      {
        time_shoot = 1000;
        direction = CLOSE_SYSTEM_VERTICAL_FORWARD;
        n = 0;

        if (t % 100 == 50)
          color = COLOR_RED;
        else if (t % 100 == 0)
          color = COLOR_BLUE;
        else
          color = COLOR_PURPLE;
      }
      else
      {
        if (t % 75 == 50)
          time_shoot = 1000;
        else
          time_shoot = 40;

        if (t <= t_start + 4275)
          direction = CLOSE_SYSTEM_VERTICAL_BACKWARD;
        else if (i == 1)
          direction = CLOSE_SYSTEM_HORIZONTAL_FORWARD;
        else
          direction = CLOSE_SYSTEM_HORIZONTAL_BACKWARD;

        if (i == 1)
          n = -2;
        else
          n = 2;

        if (t % 50 == 0)
          color = COLOR_RED;
        else
          color = COLOR_BLUE;
      }
      
      if (tenm_table_add(close_system_ball_new(t - t_start, n, direction,
                                               time_shoot, color))< 0)
      {
        fprintf(stderr, "scheduler_4: cannot create close_system_ball "
                "(t = %d)\n", t);
        status = SCHEDULER_ERROR;
      }
    }
  }

  if ((((t >= t_start + 3900) && (t <= t_start + 3950))
       || ((t >= t_start + 4050) && (t <= t_start + 4100))
       || ((t >= t_start + 4200) && (t <= t_start + 4250))
       || ((t >= t_start + 4350) && (t <= t_start + 4400)))
      && (t % 25 == 0))
  {
    time_shoot = 1000;
    if ((t >= t_start + 3900) && (t <= t_start + 4100))
      direction = CLOSE_SYSTEM_VERTICAL_FORWARD;
    else
      direction = CLOSE_SYSTEM_VERTICAL_BACKWARD;
    if (((t >= t_start + 3900) && (t <= t_start + 3950))
        || ((t >= t_start + 4350) && (t <= t_start + 4400)))
      n = 1;
    else
      n = -1;

    if (((t >= t_start + 3900) && (t <= t_start + 3950))
        || ((t >= t_start + 4200) && (t <= t_start + 4250)))
      color = COLOR_BLUE;
    else
      color = COLOR_RED;

    if (tenm_table_add(close_system_ball_new(t - t_start, n, direction,
                                             time_shoot, color))< 0)
    {
      fprintf(stderr, "scheduler_4: cannot create close_system_ball "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
    return status;
  }

  if (t == 7000)
  {
    if (tenm_table_add(message_new(((double) WINDOW_WIDTH) - 1.0, 270.0,
                               -5.0, 0.0, 1000, "boss: FUD", 9)) < 0)
    {
      fprintf(stderr, "scheduler_4: cannot create message "
              "(t = %d)\n", t);
      return SCHEDULER_ERROR;
    }
    return SCHEDULER_SUCCESS;
  }

  if (t == 7130)
  {
    if (tenm_table_add(fud_new()) < 0)
    {
      fprintf(stderr, "scheduler_4: cannot create fud "
              "(t = %d)\n", t);
      return SCHEDULER_ERROR;
    }
    return SCHEDULER_SUCCESS;
  }

  return status;
}
