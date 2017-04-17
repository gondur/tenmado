/* $Id: stage-1.c,v 1.108 2003/01/03 22:36:10 oohara Exp $ */

#include <stdio.h>
/* malloc */
#include <stdlib.h>

#include "tenm_table.h"
#include "const.h"
#include "message.h"
#include "normal-enemy.h"
#include "tenm_object.h"
#include "midnight.h"

#include "stage-1.h"

static tenm_object *stage_1_more_1_new(void);
static int stage_1_more_1_act(tenm_object *my, const tenm_object *player);
static tenm_object *stage_1_more_2_new(void);
static int stage_1_more_2_act(tenm_object *my, const tenm_object *player);
static tenm_object *stage_1_more_3_new(int n);
static int stage_1_more_3_act(tenm_object *my, const tenm_object *player);
static tenm_object *stage_1_more_4_new(int n);
static int stage_1_more_4_act(tenm_object *my, const tenm_object *player);
static tenm_object *stage_1_more_5_new(void);
static int stage_1_more_5_act(tenm_object *my, const tenm_object *player);

int
scheduler_1(int t)
{
  int i = 0;
  int j;
  int color;
  int status = SCHEDULER_SUCCESS;
  double x;
  double dx;

  /* sanity check */
  if (t < 0)
  {
    fprintf(stderr, "scheduler_1: strange t (%d)\n", t);
    return SCHEDULER_ERROR;
  }

  if (t == 30)
  {
    if (tenm_table_add(message_new(210.0, 240.0,
                                   0.0, 0.0, 100, "stage 1", 7)) < 0)
    {
      fprintf(stderr, "scheduler_1: cannot create message \"stage 1\""
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
    if (tenm_table_add(message_new(70.0, 270.0,
                                   0.0, 0.0, 100,
                                   "the four-letter word that begins with L",
                                   39)) < 0)
    {
      fprintf(stderr, "scheduler_1: cannot create message (stage 1 title) "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
    return status;
  }
  
  if (t == 180)
  {
    if (tenm_table_add(stage_1_more_1_new()) < 0)
    {
      fprintf(stderr, "scheduler_1: cannot create stage_1_more_1 "
              "(t = %d)\n", t);
      return SCHEDULER_ERROR;
    }
    else
      return SCHEDULER_SUCCESS;
  }

  /* wait enough time to avoid getting a signal of more 1 */
  if (t == 750)
  {
    if (tenm_table_add(message_new(((double) WINDOW_WIDTH) - 1.0, 240.0,
                                   -5.0, 0.0, 1000,
                                   "It's life, love, libc or lisp,", 30)) < 0)
    {
      fprintf(stderr, "scheduler_1: cannot create message (stage 1 demo "
              "line 1) "
              "(t = %d)\n", t);
      return SCHEDULER_ERROR;
    }
    return SCHEDULER_SUCCESS;
  }

  if (t == 780)
  {
    if (tenm_table_add(message_new(((double) WINDOW_WIDTH) - 1.0, 270.0,
                                   -5.0, 0.0, 1000,
                                   "depending your point of view.", 29)) < 0)
    {
      fprintf(stderr, "scheduler_1: cannot create message (stage 1 demo "
              "line 2) "
              "(t = %d)\n", t);
      return SCHEDULER_ERROR;
    }
    return SCHEDULER_SUCCESS;
  }

  if (t == 970)
  {
    for (i = 0; i < 4; i++)
    {
      if (i == 0)
        color = COLOR_PURPLE;
      else
        color = COLOR_BLUE;
      if (tenm_table_add(normal_enemy_new(0.0 - 31.0, 20.0 + (double) (60 * i),
                                          NORMAL_STING, 0, color,
                                          0, -1, -1, 2, 1,
                                          /* move 0 */
                                          30, 8.0, 4.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          1000, -8.0, 4.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* shoot 0 */
                                          1000, 40, 30, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_1: cannot create normal_enemy (%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
    return status;
  }

  if (t == 1040)
  {
    for (i = 0; i < 4; i++)
    {
      if (i == 0)
        color = COLOR_PURPLE;
      else
        color = COLOR_RED;
      if (tenm_table_add(normal_enemy_new(((double) WINDOW_WIDTH) + 31.0,
                                          20.0 + (double) (60 * i),
                                          NORMAL_STING, 0, color,
                                          0, -1, -1, 2, 1,
                                          /* move 0 */
                                          30, -8.0, 4.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          1000, 8.0, 4.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* shoot 0 */
                                          1000, 40, 30, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_1: cannot create normal_enemy (%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
    return status;
  }

  if (t == 1110)
  {
    if (tenm_table_add(normal_enemy_new((double) (WINDOW_WIDTH / 2),
                                        0.0 - 23.0,
                                        NORMAL_STING, 0, COLOR_PURPLE,
                                        0, -1, -1, 3, 2,
                                        /* move 0 */
                                        20, 0.0, 5.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 1,
                                        /* move 1 */
                                        100, 0.0, 0.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 2,
                                        /* move 2 */
                                        1000, 0.0, -8.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 2,
                                        /* shoot 0 */
                                        110, 10, -30, 0, 1,
                                        /* shoot 1 */
                                        1000, 1000, 0, 0, 1)) < 0)
    {
      fprintf(stderr, "scheduler_1: cannot create normal_enemy "
              "(t = %d)\n", t);
      return SCHEDULER_ERROR;
    }
    return SCHEDULER_SUCCESS;
  }

  if ((t >= 1150) && (t <= 1190) && (t % 10 == 0))
  {
    for (i = 0; i < 2; i++)
    {
      if (i == 0)
      {
        color = COLOR_RED;
        x = 120.0;
      }
      else
      {
        color = COLOR_BLUE;
        x = ((double) WINDOW_WIDTH) - 120.0;
      }
      if (tenm_table_add(normal_enemy_new(x, 0.0 - 23.0,
                                          NORMAL_STING, 0, color,
                                          0, -1, -1, 1, 1,
                                          /* move 0 */
                                          1000, 0.0, 6.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 0,
                                          /* shoot 0 */
                                          1000, 25, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_1: cannot create normal_enemy (%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
    return status;
  }

  if (t == 1310)
  {
    if (tenm_table_add(stage_1_more_2_new()) < 0)
    {
      fprintf(stderr, "scheduler_1: cannot create stage_1_more_2 "
              "(t = %d)\n", t);
      return SCHEDULER_ERROR;
    }
    else
      return SCHEDULER_SUCCESS;
  }

  if (t == 1600)
  {
    for (i = 0; i < 2; i++)
      if (tenm_table_add(stage_1_more_3_new(i)) < 0)
      {
        fprintf(stderr, "scheduler_1: cannot create stage_1_more_3(%d)  "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    return status;
  }

  if ((t == 1995) || (t == 2043))
  {
    for (i = 0; i < 6; i++)
    {
      if (t == 2043)
      {
        color = COLOR_PURPLE;
      }
      else if (i % 2 == 0)
      {
        color = COLOR_BLUE;
      }
      else
      {
        color = COLOR_RED;
      }
      x = 90.0 + ((double) (i * 60));
      if (tenm_table_add(normal_enemy_new(x, -24.0,
                                          NORMAL_BALL_LARGE, 0, color,
                                          0, -1, -1, 3, 1,
                                          /* move 0 */
                                          60 - (t - 1995), 0.0, 5.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          210, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* move 2 */
                                          1000, 0.0, -5.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* shoot 0 */
                                          1000, 1000, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_1: cannot create normal_enemy (%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
    return status;
  }

  if ((t >= 2112) && (t <= 2160) && (t % 24 == 0))
  {
    for (i = 0; i < 2; i++)
    {
      if (t % 48 == i * 24)
        color = COLOR_BLUE;
      else
        color = COLOR_RED;

      if (i == 0)
      {
        x = -24.0;
        dx = 2.5;
      }
      else
      {
        x = ((double) WINDOW_WIDTH) + 24.0;
        dx = -2.5;
      }
      for (j = 0; j < 3; j++)
        if (tenm_table_add(normal_enemy_new(x,
                                            120.0 + ((double) (j * 60)) - 24.0,
                                            NORMAL_BALL_LARGE, 0, color,
                                            0, -1, -1, 1, 1,
                                            /* move 0 */
                                            1000, dx, 0.0, 0.0, 0.0,
                                            0.0, 0.0, 0.0, 0.0, 0,
                                            /* shoot 0 */
                                            1000, 1000, 0, 0, 0)) < 0)
        {
          fprintf(stderr, "scheduler_1: cannot create normal_enemy (%d) "
                  "(t = %d)\n", i, t);
          status = SCHEDULER_ERROR;
        }
    }
    return status;
  }

  if (t == 2380)
  {
    if (tenm_table_add(normal_enemy_new((double) (WINDOW_WIDTH / 2),
                                        0.0 - 24.0,
                                        NORMAL_HORIZON, 0, COLOR_PURPLE,
                                        0, -1, -1, 5, 2,
                                        /* move 0 */
                                        20, 0.0, 5.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 1,
                                        /* move 1 */
                                        100, 0.0, 0.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 2,
                                        /* move 2 */
                                        88, 0.0, 5.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 3,
                                        /* move 3 */
                                        100, 0.0, 0.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 4,
                                        /* move 4 */
                                        1000, 0.0, 5.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 4,
                                        /* shoot 0 */
                                        150, 1, -50, 0, 1,
                                        /* shoot 1 */
                                        1000, 1, -88, 0, 0)) < 0)
    {
      fprintf(stderr, "scheduler_1: cannot create normal_enemy "
              "(t = %d)\n", t);
      return SCHEDULER_ERROR;
    }
    return SCHEDULER_SUCCESS;
  }

  if (t == 2390)
  {
    for (i = 0; i < 2; i++)
      if (tenm_table_add(stage_1_more_4_new(i)) < 0)
      {
        fprintf(stderr, "scheduler_1: cannot create stage_1_more_4(%d)  "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    return status;
  }

  if (t == 2890)
  {
    for (i = 0; i < 6; i++)
    {
      if (i < 3)
      {
        x = -31.0;
        dx = 8.0;
      }
      else
      {
        x = ((double) WINDOW_WIDTH) + 31.0;
        dx = -8.0;
      }
      
      if (tenm_table_add(normal_enemy_new(x, 100.0 + 50.0 * ((double) (i % 3)),
                                          NORMAL_STING, 0, COLOR_PURPLE,
                                          0, -1, -1, 3, 1,
                                          /* move 0 */
                                          10, dx, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          20, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* move 2 */
                                          1000,
                                          dx / (2.0- 0.5 * ((double) (i % 3))),
                                          0.0,
                                          0.0, 0.4,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* shoot 0 */
                                          1000, 20, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_1: cannot create normal_enemy (%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
    return status;
  }

  if (t == 2980)
  {
    for (i = 0; i < 8; i++)
    {
      if (i < 4)
      {
        x = -31.0;
        dx = 8.0;
      }
      else
      {
        x = ((double) WINDOW_WIDTH) + 31.0;
        dx = -8.0;
      }
      
      if (tenm_table_add(normal_enemy_new(x, 100.0 + 50.0 * ((double) (i % 4)),
                                          NORMAL_STING, 0, COLOR_RED,
                                          0, -1, -1, 3, 1,
                                          /* move 0 */
                                          10, dx, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          20, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* move 2 */
                                          1000,
                                          dx / (2.0- 0.4 * ((double) (i % 4))),
                                          0.0,
                                          0.0, 0.4,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* shoot 0 */
                                          1000, 20, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_1: cannot create normal_enemy (%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
    return status;
  }

  if (t == 3070)
  {
    for (i = 0; i < 8; i++)
    {
      if (i < 4)
      {
        x = -31.0;
        dx = 8.0;
      }
      else
      {
        x = ((double) WINDOW_WIDTH) + 31.0;
        dx = -8.0;
      }
      
      if (tenm_table_add(normal_enemy_new(x, 150.0 + 50.0 * ((double) (i % 4)),
                                          NORMAL_STING, 0, COLOR_PURPLE,
                                          0, -1, -1, 3, 1,
                                          /* move 0 */
                                          10, dx, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          20, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* move 2 */
                                          1000,
                                          dx / (2.0- 0.4 * ((double) (i % 4))),
                                          0.0,
                                          0.0, 0.4,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* shoot 0 */
                                          1000, 20, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_1: cannot create normal_enemy (%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
    return status;
  }

  if (t == 3160)
  {
    for (i = 0; i < 12; i++)
    {
      if (i < 6)
      {
        x = -31.0;
        dx = 8.0;
      }
      else
      {
        x = ((double) WINDOW_WIDTH) + 31.0;
        dx = -8.0;
      }
      
      if (tenm_table_add(normal_enemy_new(x, 50.0 + 50.0 * ((double) (i % 6)),
                                          NORMAL_STING, 0, COLOR_BLUE,
                                          0, -1, -1, 3, 1,
                                          /* move 0 */
                                          10, dx, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          20, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* move 2 */
                                          1000,
                                          dx / (2.0-0.3 * ((double) (i % 6))),
                                          0.0,
                                          0.0, 0.4,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* shoot 0 */
                                          1000, 20, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_1: cannot create normal_enemy (%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
    return status;
  }

  if (t == 3280)
  {
    if (tenm_table_add(stage_1_more_5_new()) < 0)
    {
      fprintf(stderr, "scheduler_1: cannot create stage_1_more_5 "
              "(t = %d)\n", t);
      return SCHEDULER_ERROR;
    }
    else
      return SCHEDULER_SUCCESS;
  }

  /* the boss is created by more 5 */

  return SCHEDULER_SUCCESS;
}

static tenm_object *
stage_1_more_1_new(void)
{
  tenm_object *new = NULL;
  int *count = NULL;

  count = (int *) malloc(sizeof(int) * 4);
  if (count == NULL)
  {
    fprintf(stderr, "stage_1_more_1_new: malloc(count) failed\n");
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

  new = tenm_object_new("stage 1 more 1", 0, 0,
                        0, 0.0, 0.0,
                        4, count, 0, NULL, 0, NULL, 
                        (int (*)(tenm_object *, double)) NULL,
                        (int (*)(tenm_object *, tenm_object *)) NULL,
                        (int (*)(tenm_object *, const tenm_object *))
                        stage_1_more_1_act,
                        (int (*)(tenm_object *, int)) NULL);
  if (new == NULL)
  {
    fprintf(stderr, "stage_1_more_1_new: tenm_object_new failed\n");
    free(count);
    return NULL;
  }

  return new;
}

static int
stage_1_more_1_act(tenm_object *my, const tenm_object *player)
{
  int i;
  int color;
  double x;
  double dx;

  /* sanity check */
  if (my == NULL)
    return 0;

  /* max about 130 frames each */
  if (my->count[2] <= 1)
  {
    if ((my->count[1] <= 48) && (my->count[1] % 12 == 0))
    {
      if (my->count[2] == 0)
      {
        dx = -5.0;
        if (my->count[1] == 48)
        {
          x = 60.0;
          color = COLOR_PURPLE;
        }
        else
        {
          x = 120.0;
          color = COLOR_BLUE;
        }
      }
      else
      {
        dx = 5.0;
        if (my->count[1] == 48)
        {
          x = ((double) WINDOW_WIDTH) - 60.0;
          color = COLOR_PURPLE;
        }
        else
        {
          x = ((double) WINDOW_WIDTH) - 120.0;
          color = COLOR_RED;
        }
      }
      if (tenm_table_add(normal_enemy_new(x, -24.0,
                                          NORMAL_BALL_LARGE, 0, color,
                                          0, my->table_index, 3, 3, 1,
                                          /* move 0 */
                                          60 - my->count[1], 0.0, 5.0,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          40, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* move 2 */
                                          1000, dx, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* shoot 0 */
                                          1000, 1000, 0, 0, 0)) < 0)
        fprintf(stderr, "stage_1_more_1_act: cannot create normal_enemy "
                "(%d, %d)\n", my->count[2], my->count[1]);
    }

    if (my->count[1] > 48)
    {
      if (my->count[3] >= 5)
      {
          my->count[1] = 0;
          (my->count[2])++;
          my->count[3] = 0;
      }
    }
    else
      (my->count[1])++;
  }
  /* max about 268 frame */
  else if (my->count[2] == 2)
  {
    if ((my->count[1] >= 20) && (my->count[1] <= 20 + 36)
        && ((my->count[1] - 20) % 12 == 0))
    {
      for (i = 0; i < 6; i++)
      {
        if (my->count[1] <= 20 + 12)
        {
          if (i == 0)
            color = COLOR_PURPLE;
          else
            color = COLOR_BLUE;
        }
        else
        {
          if (i == 5)
            color = COLOR_PURPLE;
          else
            color = COLOR_RED;
        }
        if (i < 3)
          dx = -5.0;
        else
          dx = 5.0;
        if (tenm_table_add(normal_enemy_new(90.0 + ((double) (i * 60)), -24.0,
                                            NORMAL_BALL_LARGE, 0, color,
                                            0, my->table_index, 3, 3, 1,
                                            /* move 0 */
                                            68 - my->count[1], 0.0, 5.0,
                                            0.0, 0.0,
                                            0.0, 0.0, 0.0, 0.0, 1,
                                            /* move 1 */
                                            150, 0.0, 0.0, 0.0, 0.0,
                                            0.0, 0.0, 0.0, 0.0, 2,
                                            /* move 2 */
                                            1000, dx, 0.0, 0.0, 0.0,
                                            0.0, 0.0, 0.0, 0.0, 2,
                                            /* shoot 0 */
                                            1000, 1000, 0, 0, 0)) < 0)
          fprintf(stderr, "stage_1_more_1_act: cannot create normal_enemy "
                  "(%d, %d) (%d)\n", my->count[2], my->count[1], i);
      }
    }

    if (my->count[1] > 20 + 36)
    {
      if (my->count[3] >= 24)
      {
        my->count[1] = 0;
        my->count[2] = 3;
        my->count[3] = 0;
      }
    }
    else
      (my->count[1])++;
  }
  /* the rest */
  else if (my->count[2] == 3)
  {
    if ((my->count[1] >= 20) && ((my->count[1] - 20) % 8 == 0)
        && (my->count[0] + 64 - ((my->count[1] - 20) % 64)
            < 525))
    {
      if ((my->count[1] - 20) % 16 == 0)
      {
        x = 0.0 - 14.0;
        dx = 4.949;
        if ((my->count[1] - 20) % 64 == 48)
          color = COLOR_PURPLE;
        else
          color = COLOR_BLUE;
      }
      else
      {
        x = ((double) WINDOW_WIDTH) + 14.0;
        dx = -4.949;
        if ((my->count[1] - 20) % 64 == 56)
          color = COLOR_PURPLE;
        else
          color = COLOR_RED;
      }
      if (tenm_table_add(normal_enemy_new(x, 20.0,
                                          NORMAL_BALL_SMALL, 0, color,
                                          0, -1, -1, 1, 1,
                                          /* move 0 */
                                          1000, dx, 4.949,
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 0,
                                          /* shoot 0 */
                                          1000, 1000, 0, 0, 0)) < 0)
        fprintf(stderr, "stage_1_more_1_act: cannot create normal_enemy "
                "(%d, %d)\n", my->count[2], my->count[1]);
    }
    
    (my->count[1])++;
  }
  else
    fprintf(stderr, "stage_1_more_1_act: strange mode (%d)\n", my->count[2]);

  (my->count[0])++;
  /* wait enough time to collect all signals */
  if (my->count[0] >= 600)
    return 1;
  return 0;
}

static tenm_object *
stage_1_more_2_new(void)
{
  tenm_object *new = NULL;
  int *count = NULL;

  count = (int *) malloc(sizeof(int) * 4);
  if (count == NULL)
  {
    fprintf(stderr, "stage_1_more_2_new: malloc(count) failed\n");
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

  new = tenm_object_new("stage 1 more 2", 0, 0,
                        0, 0.0, 0.0,
                        4, count, 0, NULL, 0, NULL, 
                        (int (*)(tenm_object *, double)) NULL,
                        (int (*)(tenm_object *, tenm_object *)) NULL,
                        (int (*)(tenm_object *, const tenm_object *))
                        stage_1_more_2_act,
                        (int (*)(tenm_object *, int)) NULL);
  if (new == NULL)
  {
    fprintf(stderr, "stage_1_more_2_new: tenm_object_new failed\n");
    free(count);
    return NULL;
  }

  return new;
}

static int
stage_1_more_2_act(tenm_object *my, const tenm_object *player)
{
  int i;
  int color;
  double x;
  double ddx;

  /* sanity check */
  if (my == NULL)
    return 0;

  /* max about 230 frame */
  if (my->count[2] == 0)
  {
    if ((my->count[1] <= 20) && (my->count[1] % 10 == 0))
    {
      for (i = 0; i < 2; i++)
      {
        if (my->count[1] == 10)
          color = COLOR_PURPLE;
        else if (i == 0)
          color = COLOR_RED;
        else
          color = COLOR_BLUE;

        if (i == 0)
          x = (double) (my->count[1] * 6 + 50);
        else
          x = (double) (WINDOW_WIDTH - (my->count[1] * 6 + 50));

        if (tenm_table_add(normal_enemy_new(x, 0.0 - 23.0,
                                            NORMAL_STING, 0, color,
                                            0, my->table_index, 3, 3, 2,
                                            /* move 0 */
                                            40 - my->count[1],
                                            0.0, 6.0, 0.0, 0.0,
                                            0.0, 0.0, 0.0, 0.0, 1,
                                            /* move 1 */
                                            150, 0.0, 0.0, 0.0, 0.0,
                                            0.0, 0.0, 0.0, 0.0, 2,
                                            /* move 2 */
                                            1000, 0.0, -8.0, 0.0, 0.0,
                                            0.0, 0.0, 0.0, 0.0, 2,
                                            /* shoot 0 */
                                            180, 10, my->count[1] - 50, 0, 1,
                                            /* shoot 1 */
                                            1000, 1000, 0, 0, 1)) < 0)
          fprintf(stderr, "stage_1_more_2_act: cannot create normal_enemy "
                  "(%d, %d)\n", my->count[2], my->count[1]);
      }
    }

    if (my->count[1] > 20)
    {
      if (my->count[3] >= 6)
      {
          my->count[1] = 0;
          my->count[2] = 1;
          my->count[3] = 0;
      }
    }
    else
      (my->count[1])++;
  }
  /* the rest */
  else if (my->count[2] == 1)
  {
    if ((my->count[0] <= 200) && (my->count[1] % 10 == 0))
    {
      if (my->count[1] % 20 == 10)
      {
        x = 35.0;
        ddx = 0.25;
      }
      else
      {
        x = ((double) WINDOW_WIDTH) - 35.0;
        ddx = -0.25;
      }
      
      if (tenm_table_add(normal_enemy_new(x, 0.0 - 23.0,
                                          NORMAL_STING, 0, COLOR_PURPLE,
                                          0, -1, -1, 1, 1,
                                          /* move 0 */
                                          1000, 0.0, 8.0, ddx, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* shoot 0 */
                                          1000, 10, -15, 0, 1)) < 0)
        fprintf(stderr, "stage_1_more_2_act: cannot create normal_enemy "
                "(%d, %d)\n", my->count[2], my->count[1]);
    }
    (my->count[1])++;
  }
  else
    fprintf(stderr, "stage_1_more_2_act: strange mode (%d)\n", my->count[2]);

  (my->count[0])++;
  if (my->count[0] >= 240)
    return 1;
  return 0;
}

static tenm_object *
stage_1_more_3_new(int n)
{
  tenm_object *new = NULL;
  int *count = NULL;

  count = (int *) malloc(sizeof(int) * 4);
  if (count == NULL)
  {
    fprintf(stderr, "stage_1_more_3_new: malloc(count) failed\n");
    return NULL;
  }

  /* list of count
   * [0] number of additional enemy
   * [1] timer
   * [2] killed
   * [3] left/right
   */
  count[0] = 0;
  count[1] = 0;
  count[2] = 0;
  count[3] = n;

  new = tenm_object_new("stage 1 more 3", 0, 0,
                        0, 0.0, 0.0,
                        4, count, 0, NULL, 0, NULL, 
                        (int (*)(tenm_object *, double)) NULL,
                        (int (*)(tenm_object *, tenm_object *)) NULL,
                        (int (*)(tenm_object *, const tenm_object *))
                        stage_1_more_3_act,
                        (int (*)(tenm_object *, int)) NULL);
  if (new == NULL)
  {
    fprintf(stderr, "stage_1_more_3_new: tenm_object_new failed\n");
    free(count);
    return NULL;
  }

  return new;
}

/* max about 390 frame */
static int
stage_1_more_3_act(tenm_object *my, const tenm_object *player)
{
  int color;
  double x;
  double clockwise;

  /* sanity check */
  if (my == NULL)
    return 0;

  if (my->count[1] == 45)
    my->count[0] = my->count[2];

  if ((my->count[1] <= 90) && (my->count[1] % 10 == 0))
  {
    if (my->count[3] == 0)
    {
      color = COLOR_BLUE;
      x = 80.0;
      clockwise = -1.0;
    }
    else
    {
      color = COLOR_RED;
      x = ((double) (WINDOW_WIDTH)) - 80.0;
      clockwise = 1.0;
    }
    
    if (tenm_table_add(normal_enemy_new(x,
                                        0.0 - 19.0,
                                        NORMAL_BALL_MEDIUM, 0, color,
                                        0, my->table_index, 2, 3, 1,
                                        /* move 0 */
                                        73, 0.0, 6.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 1,
                                        /* move 1 */
                                        104, 0.0, 0.0, 0.0, 0.0,
                                        240.0, 420.0 - 19.0,
                                        0.0, 0.15 * clockwise, 2,
                                        /* move 2 */
                                        1000, 0.0, -6.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 2,
                                        /* shoot 0 */
                                        1000, 70, 0, 0, 0)) < 0)
      fprintf(stderr, "stage_1_more_3_act: cannot create normal_enemy "
              "(%d)\n", my->count[1]);

  }
  else if ((my->count[0] > 0) && (my->count[1] % 10 == 0))
  {
    if (my->count[3] == 0)
    {
      x = ((double) (WINDOW_WIDTH)) - 80.0;
      clockwise = 1.0;
    }
    else
    {
      x = 80.0;
      clockwise = -1.0;
    }
    
    if (tenm_table_add(normal_enemy_new(x,
                                        0.0 - 19.0,
                                        NORMAL_BALL_MEDIUM, 0, COLOR_PURPLE,
                                        0, my->table_index, 2, 3, 1,
                                        /* move 0 */
                                        73, 0.0, 6.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 1,
                                        /* move 1 */
                                        104, 0.0, 0.0, 0.0, 0.0,
                                        240.0, 420.0 - 19.0,
                                        0.0, 0.15 * clockwise, 2,
                                        /* move 2 */
                                        1000, 0.0, -6.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 2,
                                        /* shoot 0 */
                                        1000, 35, 0, 0, 0)) < 0)
      fprintf(stderr, "stage_1_more_3_act: cannot create normal_enemy "
              "(%d)\n", my->count[1]);
    (my->count[0])--;
  }

  (my->count[1])++;

  return 0;
}

static tenm_object *
stage_1_more_4_new(int n)
{
  tenm_object *new = NULL;
  int *count = NULL;

  count = (int *) malloc(sizeof(int) * 6);
  if (count == NULL)
  {
    fprintf(stderr, "stage_1_more_4_new: malloc(count) failed\n");
    return NULL;
  }

  /* list of count
   * [0] total timer
   * [1] mode timer
   * [2] mode
   * [3] killed
   * [4] left/right
   */
  count[0] = 0;
  count[1] = 0;
  count[2] = 0;
  count[3] = 0;
  count[4] = n;
  count[5] = 400;

  new = tenm_object_new("stage 1 more 4", 0, 0,
                        0, 0.0, 0.0,
                        6, count, 0, NULL, 0, NULL, 
                        (int (*)(tenm_object *, double)) NULL,
                        (int (*)(tenm_object *, tenm_object *)) NULL,
                        (int (*)(tenm_object *, const tenm_object *))
                        stage_1_more_4_act,
                        (int (*)(tenm_object *, int)) NULL);
  if (new == NULL)
  {
    fprintf(stderr, "stage_1_more_4_new: tenm_object_new failed\n");
    free(count);
    return NULL;
  }

  return new;
}

/* max about 480 frame */
static int
stage_1_more_4_act(tenm_object *my, const tenm_object *player)
{
  int i;
  int color;
  double x;
  double speed_theta;
  double center_x;

  /* sanity check */
  if (my == NULL)
    return 0;

  if ((my->count[1] == 6) || (my->count[1] == 18))
  {
    for (i = 0; i < 2; i++)
    {
      if (i == 0)
        color = COLOR_RED;
      else
        color = COLOR_BLUE;

      speed_theta = 0.33;
      if (my->count[4] != 0)
        speed_theta *= -1.0;

      if (my->count[4] == 0)
        center_x = 90.0;
      else
        center_x = ((double) WINDOW_WIDTH) - 90.0;

      if (i == 0)
        x = center_x - 51.9615;
      else
        x = center_x + 51.9615;

      if (tenm_table_add(normal_enemy_new(x, 0.0 - 24.0,
                                          NORMAL_BALL_LARGE, 0, color,
                                          0, my->table_index, 3, 3, 1,
                                          /* move 0 */
                                          40 - my->count[1],
                                          0.0, 5.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          my->count[5], 0.0, 0.0, 0.0, 0.0,
                                          center_x, 140.0 - 24.0,
                                          0.0, speed_theta, 2,
                                          /* move 2 */
                                          1000, 0.0, -5.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* shoot 0 */
                                          1000, 60, my->count[1], 0, 0)) < 0)
        fprintf(stderr, "stage_1_more_4_act: cannot create normal_enemy "
                "(%d, %d)\n", my->count[2], my->count[1]);
    }
  }
  else if ((my->count[1] >= 0) && (my->count[1] <= 24)
           && (my->count[1] % 12 == 0))
  {
    if (my->count[1] == 0)
      color = COLOR_BLUE;
    else if (my->count[1] == 12)
      color = COLOR_PURPLE;
    else
      color = COLOR_RED;

    if (my->count[1] == 12)
    {
      speed_theta = 0;
    }
    else
    {
      speed_theta = 0.33;
      if (my->count[4] != 0)
        speed_theta *= -1.0;
    }

    if (my->count[4] == 0)
      center_x = 90.0;
    else
      center_x = ((double) WINDOW_WIDTH) - 90.0;

    x = center_x;

    if (tenm_table_add(normal_enemy_new(x, 0.0 - 24.0,
                                        NORMAL_BALL_LARGE, 0, color,
                                        0, my->table_index, 3, 3, 1,
                                        /* move 0 */
                                        40 - my->count[1],
                                        0.0, 5.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 1,
                                        /* move 1 */
                                        my->count[5], 0.0, 0.0, 0.0, 0.0,
                                        center_x, 140.0 - 24.0,
                                        0.0, speed_theta, 2,
                                        /* move 2 */
                                        1000, 0.0, -5.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 2,
                                        /* shoot 0 */
                                        1000, 60, my->count[1], 0, 0)) < 0)
      fprintf(stderr, "stage_1_more_4_act: cannot create normal_enemy "
              "(%d, %d)\n", my->count[2], my->count[1]);
  }

  if (my->count[1] > 24)
  {
    if ((my->count[2] == 0) && (my->count[3] >= 7)
        && (400 - 30 - my->count[0] >= 150) && (my->count[0] + 40 <= 500 - 5))
    {
      my->count[1] = -30;
      my->count[2] = 1;
      my->count[3] = 0;
      my->count[5] = 400 - 30 - my->count[0];
    }
  }
  else
    (my->count[1])++;

  (my->count[0])++;
  if (my->count[0] >= 500)
    return 1;
  return 0;
}

/* you will _miss_ points if you destroy the enemies too fast */
static tenm_object *
stage_1_more_5_new(void)
{
  tenm_object *new = NULL;
  int *count = NULL;

  count = (int *) malloc(sizeof(int) * 4);
  if (count == NULL)
  {
    fprintf(stderr, "stage_1_more_5_new: malloc(count) failed\n");
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

  new = tenm_object_new("stage 1 more 5", 0, 0,
                        0, 0.0, 0.0,
                        4, count, 0, NULL, 0, NULL, 
                        (int (*)(tenm_object *, double)) NULL,
                        (int (*)(tenm_object *, tenm_object *)) NULL,
                        (int (*)(tenm_object *, const tenm_object *))
                        stage_1_more_5_act,
                        (int (*)(tenm_object *, int)) NULL);
  if (new == NULL)
  {
    fprintf(stderr, "stage_1_more_5_new: tenm_object_new failed\n");
    free(count);
    return NULL;
  }

  return new;
}

static int
stage_1_more_5_act(tenm_object *my, const tenm_object *player)
{
  /* sanity check */
  if (my == NULL)
    return 0;

  /* max about 560 frame */
  if (my->count[2] == 0)
  {
    if (my->count[1] == 0)
    { 
      if (tenm_table_add(normal_enemy_new(100.0, -47.0,
                                          NORMAL_PLENTY, 0, COLOR_BLUE,
                                          0, my->table_index, 3, 3, 1,
                                          /* move 0 */
                                          150, 0.0, 3.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          120, 2.1213, -2.1213, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* move 2 */
                                          1000, 0.0, 3.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* shoot 0 */
                                          1000, 30, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "stage_1_more_5_act: cannot create normal_enemy "
                "(%d, %d)\n", my->count[2], my->count[1]);
      }
    }
    else if (my->count[1] == 120)
    {
      if (tenm_table_add(normal_enemy_new((double) (WINDOW_WIDTH/ 2),
                                          -23.0,
                                          NORMAL_STING, 0, COLOR_PURPLE,
                                          0, my->table_index, 3, 3, 1,
                                          /* move 0 */
                                          50, 0.0, 3.5, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          100, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* move 2 */
                                          1000, 0.0, 3.5, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* shoot 0 */
                                          1000, 35, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "stage_1_more_5_act: cannot create normal_enemy "
                "(sting) (%d, %d)\n", my->count[2], my->count[1]);
      }
      if (tenm_table_add(normal_enemy_new(((double) WINDOW_WIDTH) - 100.0,
                                          -47.0,
                                          NORMAL_PLENTY, 0, COLOR_RED,
                                          0, my->table_index, 3, 3, 1,
                                          /* move 0 */
                                          150, 0.0, 3.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          120, -2.1213, -2.1213, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* move 2 */
                                          1000, 0.0, 3.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* shoot 0 */
                                          1000, 30, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "stage_1_more_5_act: cannot create normal_enemy "
                "(plenty) (%d, %d)\n", my->count[2], my->count[1]);
      }
    }

    if (my->count[1] > 120)
    {
      if (my->count[3] >= 3)
      {
          my->count[1] = 0;
          my->count[2] = 1;
          my->count[3] = 0;
      }
    }
    else
      (my->count[1])++;
  }
  else if (my->count[2] == 1)
  {
    if (my->count[1] == 50)
    {
      if (tenm_table_add(message_new(((double) WINDOW_WIDTH) - 1.0, 270.0,
                                     -5.0, 0.0, 1000,
                                     "boss: Midnight", 14)) < 0)
      {
        fprintf(stderr, "stage_1_more_5_act: cannot create message "
                "(%d, %d)\n", my->count[2], my->count[1]);
      }
    }
    else if (my->count[1] == 200)
    {
      if (tenm_table_add(midnight_new()) < 0)
      {
        fprintf(stderr, "stage_1_more_5_act: cannot create midnight "
                "(%d, %d)\n", my->count[2], my->count[1]);
      }
    }

    if (my->count[1] <= 200)
      (my->count[1])++;
  }
  else
    fprintf(stderr, "stage_1_more_5_act: strange mode (%d)\n", my->count[2]);

  (my->count[0])++;
  if (my->count[0] >= 1200)
    return 1;
  return 0;
}
