/* $Id: stage-5.c,v 1.119 2003/01/03 22:42:36 oohara Exp $ */

#include <stdio.h>
/* malloc, rand */
#include <stdlib.h>

#include "tenm_table.h"
#include "const.h"
#include "message.h"
#include "normal-enemy.h"
#include "tenm_object.h"
#include "solution.h"
#include "last-boss.h"

#include "stage-5.h"

static tenm_object *stage_5_more_1_new(int n);
static int stage_5_more_1_act(tenm_object *my, const tenm_object *player);
static tenm_object *stage_5_more_2_new(void);
static int stage_5_more_2_act(tenm_object *my, const tenm_object *player);

int
scheduler_5(int t)
{
  int status = SCHEDULER_SUCCESS;
  int i;
  int color;
  double temp;
  double x;
  double dx;
  double ddx;
  double y;
  double dy;

  /* sanity check */
  if (t < 0)
  {
    fprintf(stderr, "scheduler_5: strange t (%d)\n", t);
    return SCHEDULER_ERROR;
  }

  if (t == 30)
  {
    if (tenm_table_add(message_new(190.0, 240.0,
                                   0.0, 0.0, 100, "final stage", 11)) < 0)
    {
      fprintf(stderr, "scheduler_5: cannot create message \"final stage\""
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
    if (tenm_table_add(message_new(200.0, 270.0,
                                   0.0, 0.0, 100,
                                   "L is lost", 9)) < 0)
    {
      fprintf(stderr, "scheduler_5: cannot create message (stage 5 title) "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
    return status;
  }

  if (t == 180)
  {
    if (tenm_table_add(message_new(((double) WINDOW_WIDTH) - 1.0, 90.0,
                                   -5.0, 0.0, 1000,
                                   "It's a desire for an AI...", 27)) < 0)
    {
      fprintf(stderr, "scheduler_5: cannot create message (stage 5 demo "
              "line 1) "
              "(t = %d)\n", t);
      return SCHEDULER_ERROR;
    }
    return SCHEDULER_SUCCESS;
  }

  if (t == 230)
  {
    if (tenm_table_add(message_new(((double) WINDOW_WIDTH) - 1.0, 120.0,
                                   -5.0, 0.0, 1000,
                                   "it's the world as a library...", 30)) < 0)
    {
      fprintf(stderr, "scheduler_5: cannot create message (stage 5 demo "
              "line 2) "
              "(t = %d)\n", t);
      return SCHEDULER_ERROR;
    }
    return SCHEDULER_SUCCESS;
  }

  if (t == 280)
  {
    if (tenm_table_add(message_new(((double) WINDOW_WIDTH) - 1.0, 150.0,
                                   -5.0, 0.0, 1000,
                                   "it's a tangible piece of empathy...",
                                   35)) < 0)
    {
      fprintf(stderr, "scheduler_5: cannot create message (stage 5 demo "
              "line 3) "
              "(t = %d)\n", t);
      return SCHEDULER_ERROR;
    }
    return SCHEDULER_SUCCESS;
  }

  if (t == 330)
  {
    if (tenm_table_add(message_new(((double) WINDOW_WIDTH) - 1.0, 180.0,
                                   -5.0, 0.0, 1000,
                                   "and a vow to live, however hard it is.",
                                   38)) < 0)
    {
      fprintf(stderr, "scheduler_5: cannot create message (stage 5 demo "
              "line 4) "
              "(t = %d)\n", t);
      return SCHEDULER_ERROR;
    }
    return SCHEDULER_SUCCESS;
  }

  if ((t >= 490) && (t <= 840) && (t % 5 == 0))
  {
    for (i = 0; i < 2; i++)
    {
      if (i == 0)
        color = COLOR_BLUE;
      else
        color = COLOR_RED;
      if (t <= 530)
      {
        temp = ((double) (530 - t)) / 10.0;
        temp *= temp;
        if (i == 0)
          x = ((double) WINDOW_WIDTH) / 2.0 - (145.0 - 8.0 * temp);
        else
          x = ((double) WINDOW_WIDTH) / 2.0 + (145.0 - 8.0 * temp);
      }
      else if (t <= 540)
      {
        if (i == 0)
          x = ((double) WINDOW_WIDTH) / 2.0 - 145.0;
        else
          x = ((double) WINDOW_WIDTH) / 2.0 + 145.0;
      }
      else if (t <= 590)
      {
        temp = ((double) (t - 540)) / 10.0;
        temp *= temp;
        if (i == 0)
          x = ((double) WINDOW_WIDTH) / 2.0 - (145.0 - 8.0 * temp);
        else
          x = ((double) WINDOW_WIDTH) / 2.0 + (145.0 - 8.0 * temp);
      }
      else if (t <= 630)
      {
        temp = ((double) (630 - t)) / 10.0;
        temp *= temp;
        if (i == 0)
          x = ((double) WINDOW_WIDTH) / 2.0 + (167.0 - 8.0 * temp);
        else
          x = ((double) WINDOW_WIDTH) / 2.0 - (167.0 - 8.0 * temp);
      }
      else if (t <= 640)
      {
        if (i == 0)
          x = ((double) WINDOW_WIDTH) / 2.0 + 167.0;
        else
          x = ((double) WINDOW_WIDTH) / 2.0 - 167.0;
      }
      else if (t <= 690)
      {
        temp = ((double) (t - 640)) / 10.0;
        temp *= temp;
        if (i == 0)
          x = ((double) WINDOW_WIDTH) / 2.0 + (167.0 - 8.0 * temp);
        else
          x = ((double) WINDOW_WIDTH) / 2.0 - (167.0 - 8.0 * temp);
      }
      else if (t <= 730)
      {
        temp = ((double) (730 - t)) / 10.0;
        temp *= temp;
        if (i == 0)
          x = ((double) WINDOW_WIDTH) / 2.0 - (145.0 - 8.0 * temp);
        else
          x = ((double) WINDOW_WIDTH) / 2.0 + (145.0 - 8.0 * temp);
      }
      else if (t <= 740)
      {
        if (i == 0)
          x = ((double) WINDOW_WIDTH) / 2.0 - 145.0;
        else
          x = ((double) WINDOW_WIDTH) / 2.0 + 145.0;
      }
      else if (t <= 790)
      {
        temp = ((double) (t - 740)) / 10.0;
        temp *= temp;
        if (i == 0)
          x = ((double) WINDOW_WIDTH) / 2.0 - (145.0 - 8.0 * temp);
        else
          x = ((double) WINDOW_WIDTH) / 2.0 + (145.0 - 8.0 * temp);
      }
      else if (t <= 830)
      {
        temp = ((double) (830 - t)) / 10.0;
        temp *= temp;
        if (i == 0)
          x = ((double) WINDOW_WIDTH) / 2.0 + (167.0 - 8.0 * temp);
        else
          x = ((double) WINDOW_WIDTH) / 2.0 - (167.0 - 8.0 * temp);
      }
      else
      {
        if (i == 0)
          x = ((double) WINDOW_WIDTH) / 2.0 + 167.0;
        else
          x = ((double) WINDOW_WIDTH) / 2.0 - 167.0;
      }

      if (t <= 630)
        ddx = 0.0;
      else if (t <= 730)
        ddx = 0.3;
      else
        ddx = 0.8;
      
      if (tenm_table_add(normal_enemy_new(x, -14.0,
                                          NORMAL_BALL_SMALL, 0, color,
                                          0, -1, -1, 3, 1,
                                          /* move 0 */
                                          5, ddx * (-5.0), 15.0, ddx, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          10, 0.0, 15.0, -ddx, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* move 2 */
                                          10, 0.0, 15.0, ddx, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* shoot 0 */
                                          1000, 1000, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_5: cannot create normal_enemy (%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
    return status;
  }

  if ((t >= 890) && (t <= 940) && (t % 5 == 0))
  {
    if (t % 10 == 0)
    {
      x = ((double) WINDOW_WIDTH) / 2.0 - ((double) ((t - 850) * 3));
      dx = 3.0;
    }
    else
    {
      x = ((double) WINDOW_WIDTH) / 2.0 + ((double) ((t - 850) * 3));
      dx = -3.0;
    }

    dy = ((double) (t - 850)) * 0.15;
    if (tenm_table_add(normal_enemy_new(x, -23.0,
                                        NORMAL_STING, 0, COLOR_PURPLE,
                                        0, -1, -1, 1, 1,
                                        /* move 0 */
                                        1000, dx, dy, 0.0, -0.15,
                                        0.0, 0.0, 0.0, 0.0, 0,
                                        /* shoot 0 */
                                        1000, 12, 0, 100, 0)) < 0)
    {
      fprintf(stderr, "scheduler_5: cannot create normal_enemy "
              "(t = %d)\n", t);
      return SCHEDULER_ERROR;
    }
    return SCHEDULER_SUCCESS;
  }

  if (t == 1090)
  {
    for (i = 0; i < 24; i++)
    {
      if (i < 4)
        dy = (80.0 - (-23.0)) / 30.0;
      else if (i < 8)
        dy = (160.0 - (-23.0)) / 30.0;
      else if (i < 12)
        dy = (240.0 - (-23.0)) / 30.0;
      else if (i < 16)
        dy = (400.0 - (-23.0)) / 30.0;
      else if (i < 20)
        dy = (480.0 - (-23.0)) / 30.0;
      else
        dy = (560.0 - (-23.0)) / 30.0;

      switch (i % 4)
      {
      case 0:
        x = ((double) WINDOW_WIDTH) / 2.0 - 180.0;
        break;
      case 1:
        x = ((double) WINDOW_WIDTH) / 2.0 - 120.0;
        break;
      case 2:
        x = ((double) WINDOW_WIDTH) / 2.0 + 120.0;
        break;
      case 3:
        x = ((double) WINDOW_WIDTH) / 2.0 + 180.0;
        break;
      default:
        fprintf(stderr, "scheduler_5: strange remainder (%d) "
                "(t = %d)\n", i % 4, t);
        x = ((double) WINDOW_WIDTH) / 2.0;
        break;
      }

      if (i % 4 < 2)
      {
        dx = 15.0;
        ddx = -0.3;
      }
      else
      {
        dx = -15.0;
        ddx = 0.3;
      }
      
      if (tenm_table_add(normal_enemy_new(x, -23.0,
                                          NORMAL_STING, 0, COLOR_PURPLE,
                                          0, -1, -1, 3, 1,
                                          /* move 0 */
                                          30, 0.0, dy, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          30, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* move 1 */
                                          1000, dx, 4.0, ddx, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* shoot 0 */
                                          1000, 1000, 950, 100, 0)) < 0)
      {
        fprintf(stderr, "scheduler_5: cannot create normal_enemy "
                "(t = %d)\n", t);
        status = SCHEDULER_ERROR;
      }
    }
    return status;
  }

  if ((t >= 1300) && (t <= 1880) && (t % 5 == 0))
  {
    for (i = 0; i < 3; i++)
    {
      y = (double) ((t - 1300) * 2 + ((t * 13) % 197) - i * 200);

      if (t % 10 == 0)
      {
        x = -14.0;
        dx = 10.0;
      }
      else
      {
        x = ((double) WINDOW_WIDTH) + 14.0;
        dx = -10.0;
      }

      if (i == 0)
        color = COLOR_RED;
      else if (i == 1)
        color = COLOR_PURPLE;
      else
        color = COLOR_BLUE;

      if ((y > 0.0) && (y < (double) WINDOW_HEIGHT))
      {
        if (tenm_table_add(normal_enemy_new(x, y,
                                            NORMAL_BALL_SMALL, 0, color,
                                            0, -1, -1, 1, 1,
                                            /* move 0 */
                                            1000, dx, 0.0, 0.0, 0.0,
                                            0.0, 0.0, 0.0, 0.0, 0,
                                            /* shoot 0 */
                                            1000, 1000, 0, 0, 0)) < 0)
        {
          fprintf(stderr, "scheduler_5: cannot create normal_enemy (%d) "
                  "(t = %d)\n", i, t);
          status = SCHEDULER_ERROR;
        }
      }
    }
    return status;
  }

  if (t == 1890)
  {
    for (i = 0; i < 6; i++)
    {
      if (tenm_table_add(stage_5_more_1_new(i)) < 0)
      {
        fprintf(stderr, "scheduler_5: cannot create stage_5_more_1 (%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
    return status;
  }

  if ((t == 2240) || (t == 2340) || (t == 2440))
  {
    if (t == 2240)
      color = COLOR_PURPLE;
    else if (t == 2340)
      color = COLOR_BLUE;
    else
      color = COLOR_RED;

    if (tenm_table_add(normal_enemy_new(-35.0, 0.0,
                                        NORMAL_PLENTY, 0, color,
                                        0, -1, -1, 3, 1,
                                        /* move 0 */
                                        100, 8.0, 3.5, -0.08, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 1,
                                        /* move 1 */
                                        50, 0.0, 3.5, -0.16, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 2,
                                        /* move 2 */
                                        1000, -8.0, 3.5, 0.16, -0.1,
                                        0.0, 0.0, 0.0, 0.0, 2,
                                        /* shoot 0 */
                                        1000, 12, 0, 0, 0)) < 0)
    {
      fprintf(stderr, "scheduler_5: cannot create normal_enemy "
              "(t = %d)\n", t);
      return SCHEDULER_ERROR;
    }
    return SCHEDULER_SUCCESS;
  }

  if (t == 2750)
  {
    if (tenm_table_add(stage_5_more_2_new()) < 0)
    {
      fprintf(stderr, "scheduler_5: cannot create stage_5_more_2 "
              "(t = %d)\n", t);
      return SCHEDULER_ERROR;
    }
    return SCHEDULER_SUCCESS;
  }

  /* the boss is created by more 2 */

  return SCHEDULER_SUCCESS;
}

static tenm_object *
stage_5_more_1_new(int n)
{
  tenm_object *new = NULL;
  int *count = NULL;  count = (int *) malloc(sizeof(int) * 5);
  if (count == NULL)
  {
    fprintf(stderr, "stage_5_more_1_new: malloc(count) failed\n");
    return NULL;
  }

  /* list of count
   * [0] total timer
   * [1] mode timer
   * [2] mode
   * [3] killed
   * [4] n
   */
  count[0] = 0;
  count[1] = 0;
  count[2] = 0;
  count[3] = 0;
  count[4] = n;

  new = tenm_object_new("stage 5 more 1", 0, 0,
                        0, 0.0, 0.0,
                        4, count, 0, NULL, 0, NULL,
                        (int (*)(tenm_object *, double)) NULL,
                        (int (*)(tenm_object *, tenm_object *)) NULL,
                        (int (*)(tenm_object *, const tenm_object *))
                        stage_5_more_1_act,
                        (int (*)(tenm_object *, int)) NULL);
  if (new == NULL)
  {
    fprintf(stderr, "stage_5_more_1_new: tenm_object_new failed\n");
    free(count);
    return NULL;
  }

  return new;
}

static int
stage_5_more_1_act(tenm_object *my, const tenm_object *player)
{
  int i;
  int color;
  double x;
  double ddx;
  double dy;

  /* sanity check */
  if (my == NULL)
    return 0;

  if (my->count[2] == 0)
  {
    if (my->count[1] == 0)
    { 
      x = (double) (40 + my->count[4] * 80);

      switch (my->count[4] % 3)
      {
      case 0:
        color = COLOR_BLUE;
        break;
      case 1:
        color = COLOR_PURPLE;
        break;
      case 2:
        color = COLOR_RED;
        break;
      default:
        fprintf(stderr, "stage_5_more_1_act: strange n (%d) "
                "(%d, %d)\n", my->count[4], my->count[2], my->count[1]);
        color = COLOR_PURPLE;
        break;
      }

      if (my->count[4] % 2 == 0)
      {
        dy = (80.0 - (-23.0)) / 30.0;
        if (tenm_table_add(normal_enemy_new(x, -24.0,
                                            NORMAL_HORIZON, 0, color,
                                            0, my->table_index, 3, 6, 3,
                                            /* move 0 */
                                            30, 0.0, dy, 0.0, 0.0,
                                            0.0, 0.0, 0.0, 0.0, 1,
                                            /* move 1 */
                                            65, 0.0, 0.0, 0.0, 0.0,
                                            0.0, 0.0, 0.0, 0.0, 2,
                                            /* move 2 */
                                            40, 0.0, 11.0, 0.0, 0.0,
                                            0.0, 0.0, 0.0, 0.0, 3,
                                            /* move 3 */
                                            90, 0.0, 0.0, 0.0, 0.0,
                                            0.0, 0.0, 0.0, 0.0, 4,
                                            /* move 4 */
                                            25, 0.0, -11.0, 0.0, 0.0,
                                            0.0, 0.0, 0.0, 0.0, 5,
                                            /* move 5 */
                                            1000, 4.0, 0.0, 0.0, 0.0,
                                            0.0, 0.0, 0.0, 0.0, 5,
                                            /* shoot 0 */
                                            95, 1, -35, 0, 1,
                                            /* shoot 1 */
                                            115, 1, -45, 0, 2,
                                            /* shoot 2 */
                                            1000, 1, -45, 0, 2)) < 0)
        {
          fprintf(stderr, "stage_5_more_1_act: cannot create normal_enemy "
                  "(%d) "
                  "(%d, %d)\n", my->count[4], my->count[2], my->count[1]);
        }
      }
      else
      {
        dy = (560.0 - (-23.0)) / 30.0;
        if (tenm_table_add(normal_enemy_new(x, -24.0,
                                            NORMAL_HORIZON, 0, color,
                                            0, my->table_index, 3, 6, 4,
                                            /* move 0 */
                                            30, 0.0, dy, 0.0, 0.0,
                                            0.0, 0.0, 0.0, 0.0, 1,
                                            /* move 1 */
                                            65, 0.0, 0.0, 0.0, 0.0,
                                            0.0, 0.0, 0.0, 0.0, 2,
                                            /* move 2 */
                                            40, 0.0, -11.0, 0.0, 0.0,
                                            0.0, 0.0, 0.0, 0.0, 3,
                                            /* move 3 */
                                            90, 0.0, 0.0, 0.0, 0.0,
                                            0.0, 0.0, 0.0, 0.0, 4,
                                            /* move 4 */
                                            25, 0.0, 11.0, 0.0, 0.0,
                                            0.0, 0.0, 0.0, 0.0, 5,
                                            /* move 5 */
                                            1000, -4.0, 0.0, 0.0, 0.0,
                                            0.0, 0.0, 0.0, 0.0, 5,
                                            /* shoot 0 */
                                            2, 1, -1000, 0, 1,
                                            /* shoot 1 */
                                            93, 1, -33, 0, 2,
                                            /* shoot 2 */
                                            115, 1, -45, 0, 3,
                                            /* shoot 3 */
                                            1000, 1, -45, 0, 3)) < 0)
        {
          fprintf(stderr, "stage_5_more_1_act: cannot create normal_enemy "
                  "(%d) "
                  "(%d, %d)\n", my->count[4], my->count[2], my->count[1]);
        }
      }
    }
    if (my->count[1] > 0)
    {
      if ((my->count[3] >= 1) && (my->count[0] < 280))
      {
        my->count[1] = 0;
        my->count[2] = 1;
      }
    }
    else
    {
      (my->count[1])++;
    }
  }
  else if  (my->count[2] == 1)
  {
    if ((my->count[1] <= 9) && (my->count[1] % 3 == 0))
    {
      for (i = 0; i < 2; i++)
      {
        x = (double) (40 + my->count[4] * 80);
        if (i == 0)
        {
          x -= 5.0;
          ddx = -0.2;
        }
        else
        {
          x += 5.0;
          ddx = 0.2;
        }

        switch (my->count[4] % 3)
        {
        case 0:
          color = COLOR_BLUE;
          break;
        case 1:
          color = COLOR_PURPLE;
          break;
        case 2:
          color = COLOR_RED;
          break;
        default:
          fprintf(stderr, "stage_5_more_1_act: strange n (%d) "
                  "(%d, %d)\n", my->count[4], my->count[2], my->count[1]);
          color = COLOR_PURPLE;
          break;
        }

        if (tenm_table_add(normal_enemy_new(x, -19.0,
                                            NORMAL_BALL_MEDIUM, 0, color,
                                            0, -1, -1, 1, 1,
                                            /* move 0 */
                                            1000, 0.0, 8.0, ddx, 0.0,
                                            0.0, 0.0, 0.0, 0.0, 0,
                                            /* shoot 0 */
                                            1000, 1000, 0, 0, 0)) < 0)
        {
          fprintf(stderr, "stage_5_more_1_act: cannot create normal_enemy "
                  "(%d) "
                  "(%d, %d)\n", i, my->count[2], my->count[1]);
        }
      }
    }
    (my->count[1])++;
  }
  else
  {
    fprintf(stderr, "stage_5_more_1_act: strange mode (%d)\n", my->count[2]);
  }

  (my->count[0])++;
  if (my->count[0] >= 400)
    return 1;

  return 0;
}

static tenm_object *
stage_5_more_2_new(void)
{
  tenm_object *new = NULL;
  int *count = NULL;
  double *count_d = NULL;

  count = (int *) malloc(sizeof(int) * 5);
  if (count == NULL)
  {
    fprintf(stderr, "stage_5_more_2_new: malloc(count) failed\n");
    return NULL;
  }

  count_d = (double *) malloc(sizeof(double) * 1);
  if (count_d == NULL)
  {
    fprintf(stderr, "stage_5_more_2_new: malloc(count_d) failed\n");
    if (count != NULL)
      free(count);
    return NULL;
  }

  /* list of count
   * [0] total timer
   * [1] mode timer
   * [2] mode
   * [3] killed
   */
  /* list of count_d
   * [0] mode 1 x
   */
  count[0] = 0;
  count[1] = 0;
  count[2] = 0;
  count[3] = 0;

  count_d[0] = ((double) WINDOW_WIDTH) / 2.0;

  new = tenm_object_new("stage 5 more 2", 0, 0,
                        0, 0.0, 0.0,
                        4, count, 1, count_d, 0, NULL,
                        (int (*)(tenm_object *, double)) NULL,
                        (int (*)(tenm_object *, tenm_object *)) NULL,
                        (int (*)(tenm_object *, const tenm_object *))
                        stage_5_more_2_act,
                        (int (*)(tenm_object *, int)) NULL);
  if (new == NULL)
  {
    fprintf(stderr, "stage_5_more_2_new: tenm_object_new failed\n");
    free(count);
    return NULL;
  }

  return new;
}

static int
stage_5_more_2_act(tenm_object *my, const tenm_object *player)
{
  int color;

  /* sanity check */
  if (my == NULL)
    return 0;

  if (my->count[2] == 0)
  {
    if (my->count[1] == 0)
    {
      if (tenm_table_add(solution_new(my->table_index)) < 0)
      {
        fprintf(stderr, "stage_5_more_2: cannot create solution "
                "(%d, %d)\n", my->count[2], my->count[1]);
      }
    }
    (my->count[1])++;

    if (my->count[3] >= 1)
    {
      my->count[1] = 0;
      if (my->count[0] <= 480)
        my->count[2] = 1;
      else
        my->count[2] = 2;
    }
  }
  else if (my->count[2] == 1)
  {
    if ((my->count[1] >= 120) && (my->count[0] <= 700)
        && (my->count[1] % 5 == 0))
    {
      /* bounded random walk */
      if (my->count_d[0] < 70.0)
        my->count_d[0] += 30.0;
      else if (my->count_d[0] > ((double) WINDOW_WIDTH) - 70.0)
        my->count_d[0] -= 30.0;
      else if (rand() % 2 == 0)
        my->count_d[0] += 30.0;
      else
        my->count_d[0] -= 30.0;

      if (my->count[1] % 15 == 10)
        color = COLOR_PURPLE;
      else if (my->count[1] % 30 < 15)
        color = COLOR_BLUE;
      else
        color = COLOR_RED;

      if (tenm_table_add(normal_enemy_new(my->count_d[0], -14.0,
                                          NORMAL_BALL_SMALL, 0, color,
                                          0, -1, -1, 1, 1,
                                          /* move 0 */
                                          1000, 0.0, 15.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 0,
                                          /* shoot 0 */
                                          1000, 1000, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "stage_5_more_2: cannot create normal_enemy "
                "(%d, %d)\n", my->count[2], my->count[1]);
      }
    }
    (my->count[1])++;
    if (my->count[0] >= 700)
    {
      my->count[1] = 0;
      my->count[2] = 2;
    }
  }
  else if (my->count[2] == 2)
  {
    if (my->count[1] == 120)
    {
      if (tenm_table_add(message_new(((double) WINDOW_WIDTH) - 1.0, 270.0,
                                     -5.0, 0.0, 1000, "boss: L", 9)) < 0)
      {
        fprintf(stderr, "stage_5_more_2: cannot create message "
                "(%d, %d)\n", my->count[2], my->count[1]);
      }
    }
    else if (my->count[1] == 250)
    {
      if (tenm_table_add(last_boss_new()) < 0)
      {
        fprintf(stderr, "stage_5_more_2: cannot create L "
                "(%d, %d)\n", my->count[2], my->count[1]);
      }
    }
    (my->count[1])++;
  }
  else
  {
    fprintf(stderr, "stage_5_more_2: strange mode (%d)\n" , my->count[2]);
  }

  (my->count[0])++;
  if (my->count[0] >= 1000)
    return 1;

  return 0;
}
