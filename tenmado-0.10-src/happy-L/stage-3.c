/* $Id: stage-3.c,v 1.169 2003/01/03 22:40:05 oohara Exp $ */

#include <stdio.h>
/* malloc */
#include <stdlib.h>

#include "tenm_table.h"
#include "const.h"
#include "message.h"
#include "normal-enemy.h"
#include "tenm_object.h"
#include "tenm_math.h"
#include "wall.h"
#include "balloon.h"
#include "disconnection.h"

#include "stage-3.h"

#define SCROLL_SPEED 3.0

static tenm_object *stage_3_more_1_new(void);
static int stage_3_more_1_act(tenm_object *my, const tenm_object *player);
static tenm_object *stage_3_more_2_new(int n);
static int stage_3_more_2_act(tenm_object *my, const tenm_object *player);
static tenm_object *stage_3_more_3_new(void);
static int stage_3_more_3_act(tenm_object *my, const tenm_object *player);
static tenm_object *stage_3_more_4_new(void);
static int stage_3_more_4_act(tenm_object *my, const tenm_object *player);
static tenm_object *stage_3_more_5_new(void);
static int stage_3_more_5_act(tenm_object *my, const tenm_object *player);

int
scheduler_3(int t)
{
  int status = SCHEDULER_SUCCESS;
  int i;
  int color;
  double x;
  double dx;
  double y;
  double dy;

  /* sanity check */
  if (t < 0)
  {
    fprintf(stderr, "scheduler_3: strange t (%d)\n", t);
    return SCHEDULER_ERROR;
  }

  if (t == 30)
  {
    if (tenm_table_add(message_new(210.0, 240.0,
                                   0.0, 0.0, 100, "stage 3", 7)) < 0)
    {
      fprintf(stderr, "scheduler_3: cannot create message \"stage 3\""
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
    if (tenm_table_add(message_new(180.0, 270.0,
                                   0.0, 0.0, 100,
                                   "L is binding me", 15)) < 0)
    {
      fprintf(stderr, "scheduler_3: cannot create message (stage 3 title) "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
    return status;
  }

  if ((t == 180) || (t == 600))
  {
    if (tenm_table_add(normal_enemy_new(((double) WINDOW_WIDTH) / 2.0, -24.0,
                                        NORMAL_BALL_LARGE, 0, COLOR_PURPLE,
                                        0, -1, -1, 3, 1,
                                        /* move 0 */
                                        55, 0.0, 5.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 1,
                                        /* move 1 */
                                        100, 0.0, 0.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 2,
                                        /* move 2 */
                                        1000, 0.0, -5.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 2,
                                        /* shoot 0 */
                                        1000, 1000, 970, 0, 0)) < 0)
    {
      fprintf(stderr, "scheduler_3: cannot create normal_enemy "
              "(t = %d)\n", t);
      return SCHEDULER_ERROR;
    }
    return SCHEDULER_SUCCESS;
  }

  if ((t == 230) || (t == 263) || (t == 650) || (t == 683))
  {
    for (i = 0; i < 4; i++)
    {
      if (i < 2)
      {
        x = -24.0;
        dx = 4.0;
      }
      else
      {
        x = ((double) WINDOW_WIDTH) + 24.0;
        dx = -4.0;
      }

      if (i % 2 == 0)
      {
        y = -24.0 + 5.0 * 55.0 - 3.0 * 66.0;
        dy = 3.0;
      }
      else
      {
        y = -24.0 + 5.0 * 55.0 + 3.0 * 66.0;
        dy = -3.0;
      }
      
      if ((t == 263) || (t == 683))
      {
        dx *= 2.0;
        dy *= 2.0;
      }
      
      if ((i == 0) || (i == 3))
        color = COLOR_BLUE;
      else
        color = COLOR_RED;
      

      if (tenm_table_add(normal_enemy_new(x, y,
                                          NORMAL_BALL_LARGE, 0, color,
                                          0, -1, -1, 1, 1,
                                          /* move 0 */
                                          1000, dx, dy, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 0,
                                          /* shoot 0 */
                                          1000, 1000, 995, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_3: cannot create normal_enemy (%i) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
    return status;
  }

  if ((t == 300) || (t == 649))
  {
    if (tenm_table_add(normal_enemy_new(((double) WINDOW_WIDTH) / 2.0 - 64.0,
                                        -24.0,
                                        NORMAL_BALL_LARGE, 0, COLOR_PURPLE,
                                        0, -1, -1, 3, 1,
                                        /* move 0 */
                                        55, 0.0, 5.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 1,
                                        /* move 1 */
                                        100, 0.0, 0.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 2,
                                        /* move 2 */
                                        1000, 0.0, -5.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 2,
                                        /* shoot 0 */
                                        1000, 1000, 975, 0, 0)) < 0)
    {
      fprintf(stderr, "scheduler_3: cannot create normal_enemy "
              "(t = %d)\n", t);
      return SCHEDULER_ERROR;
    }
    return SCHEDULER_SUCCESS;
  }

  if ((t == 340) || (t == 372) || (t == 381) || (t == 397)
      || (t == 690) || (t == 722) || (t == 731) || (t == 747))
  {
    if ((t == 372) || (t == 397) || (t == 722) || (t == 747))
    {
      x = -24.0;
      dx = 4.0;
    }
    else
    {
      x = ((double) WINDOW_WIDTH) + 24.0;
      dx = -4.0;
    }

    if ((t == 381) || (t == 397) || (t == 731) || (t == 747))
      dx *= 2.0;

    for (i = 0; i < 2; i++)
    {
      if ((t == 340) || (t == 381) || (t == 690) || (t == 731))
      {
        if (i % 2 == 0)
        {
          y = -24.0 + 5.0 * 55.0 - 3.0 * 82.0;
          dy = 3.0;
        }
        else
        {
          y = -24.0 + 5.0 * 55.0 + 3.0 * 82.0;
          dy = -3.0;
        }
      }
      else
      {
        if (i % 2 == 0)
        {
          y = -24.0 + 5.0 * 55.0 - 3.0 * 50.0;
          dy = 3.0;
        }
        else
        {
          y = -24.0 + 5.0 * 55.0 + 3.0 * 50.0;
          dy = -3.0;
        }
      }

      if ((t == 381) || (t == 397) || (t == 731) || (t == 747))
        dy *= 2.0;
      
      if ((t == 340) || (t == 381) || (t == 690) || (t == 731))
      {
        if (i == 0)
          color = COLOR_BLUE;
        else
          color = COLOR_RED;
      }
      else
      {
        if (i == 0)
          color = COLOR_RED;
        else
          color = COLOR_BLUE;
      }

      if (tenm_table_add(normal_enemy_new(x, y,
                                          NORMAL_BALL_LARGE, 0, color,
                                          0, -1, -1, 1, 1,
                                          /* move 0 */
                                          1000, dx, dy, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 0,
                                          /* shoot 0 */
                                          1000, 1000, 995, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_3: cannot create normal_enemy (%i) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
    return status;
  }

  if ((t == 450) || (t == 700))
  {
    if (tenm_table_add(normal_enemy_new(((double) WINDOW_WIDTH) / 2.0 + 64.0,
                                        -24.0,
                                        NORMAL_BALL_LARGE, 0, COLOR_PURPLE,
                                        0, -1, -1, 3, 1,
                                        /* move 0 */
                                        55, 0.0, 5.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 1,
                                        /* move 1 */
                                        100, 0.0, 0.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 2,
                                        /* move 2 */
                                        1000, 0.0, -5.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 2,
                                        /* shoot 0 */
                                        1000, 1000, 975, 0, 0)) < 0)
    {
      fprintf(stderr, "scheduler_3: cannot create normal_enemy "
              "(t = %d)\n", t);
      return SCHEDULER_ERROR;
    }
    return SCHEDULER_SUCCESS;
  }

  if ((t == 490) || (t == 522) || (t == 531) || (t == 549)
      || (t == 740) || (t == 772) || (t == 781) || (t == 797))
  {
    if ((t == 490) || (t == 531) || (t == 740) || (t == 781))
    {
      x = -24.0;
      dx = 4.0;
    }
    else
    {
      x = ((double) WINDOW_WIDTH) + 24.0;
      dx = -4.0;
    }

    if ((t == 531) || (t == 549) || (t == 781) || (t == 797))
      dx *= 2.0;

    for (i = 0; i < 2; i++)
    {
      if ((t == 490) || (t == 531) || (t == 740) || (t == 781))
      {
        if (i % 2 == 0)
        {
          y = -24.0 + 5.0 * 55.0 - 3.0 * 82.0;
          dy = 3.0;
        }
        else
        {
          y = -24.0 + 5.0 * 55.0 + 3.0 * 82.0;
          dy = -3.0;
        }
      }
      else
      {
        if (i % 2 == 0)
        {
          y = -24.0 + 5.0 * 55.0 - 3.0 * 50.0;
          dy = 3.0;
        }
        else
        {
          y = -24.0 + 5.0 * 55.0 + 3.0 * 50.0;
          dy = -3.0;
        }
      }

      if ((t == 531) || (t == 549) || (t == 781) || (t == 797))
        dy *= 2.0;
      
      if ((t == 490) || (t == 531) || (t == 740) || (t == 781))
      {
        if (i == 0)
          color = COLOR_RED;
        else
          color = COLOR_BLUE;
      }
      else
      {
        if (i == 0)
          color = COLOR_BLUE;
        else
          color = COLOR_RED;
      }

      if (tenm_table_add(normal_enemy_new(x, y,
                                          NORMAL_BALL_LARGE, 0, color,
                                          0, -1, -1, 1, 1,
                                          /* move 0 */
                                          1000, dx, dy, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 0,
                                          /* shoot 0 */
                                          1000, 1000, 995, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_3: cannot create normal_enemy (%i) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
    return status;
  }

  if ((t >= 854) && (t <= 930))
  {  
    if ((t >= 854) && (t <= 924) && (t % 7 == 0))
    {
      if (t % 21 == 14)
        color = COLOR_BLUE;
      else if (t % 21 == 0)
        color = COLOR_PURPLE;
      else
        color = COLOR_RED;

      if (tenm_table_add(normal_enemy_new(((double) WINDOW_WIDTH) / 2.0 - 10.0,
                                          -14.0,
                                          NORMAL_BALL_SMALL, 0, color,
                                          0, -1, -1, 1, 1,
                                          /* move 0 */
                                          1000, 0.0, 6.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 0,
                                          /* shoot 0 */
                                          1000, 1000, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_3: cannot create normal_enemy "
                "(t = %d)\n", t);
        status = SCHEDULER_ERROR;
      }
    }
    if ((t >= 870) && (t <= 930) && (t % 6 == 0))
    {
      if (t % 18 == 6)
        color = COLOR_BLUE;
      else if (t % 18 == 12)
        color = COLOR_RED;
      else
        color = COLOR_PURPLE;

      if (tenm_table_add(normal_enemy_new(((double) WINDOW_WIDTH) / 2.0 + 10.0,
                                          -14.0,
                                          NORMAL_BALL_SMALL, 0, color,
                                          0, -1, -1, 1, 1,
                                          /* move 0 */
                                          1000, 0.0, 7.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 0,
                                          /* shoot 0 */
                                          1000, 1000, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_3: cannot create normal_enemy "
                "(t = %d)\n", t);
        status = SCHEDULER_ERROR;
      }
    }
    return status;
  }
  
  if (t == 1050)
  {
    if (tenm_table_add(message_new(((double) WINDOW_WIDTH) - 1.0, 240.0,
                                   -5.0, 0.0, 1000,
                                   "It's a good approximation of insanity,",
                                   38)) < 0)
    {
      fprintf(stderr, "scheduler_3: cannot create message (stage 2 demo "
              "line 1) "
              "(t = %d)\n", t);
      return SCHEDULER_ERROR;
    }
    return SCHEDULER_SUCCESS;
  }

  if (t == 1080)
  {
    if (tenm_table_add(message_new(((double) WINDOW_WIDTH) - 1.0, 270.0,
                                   -5.0, 0.0, 1000,
                                   "a sufficient condition of convergence.",
                                   38)) < 0)
    {
      fprintf(stderr, "scheduler_3: cannot create message (stage 2 demo "
              "line 1) "
              "(t = %d)\n", t);
      return SCHEDULER_ERROR;
    }
    return SCHEDULER_SUCCESS;
  }

  if (t == 1260)
  {
    if (tenm_table_add(stage_3_more_1_new()) < 0)
    {
      fprintf(stderr, "scheduler_3: cannot create stage_3_more_1 "
              "(t = %d)\n", t);
      return SCHEDULER_ERROR;
    }
    return SCHEDULER_SUCCESS;
  }

  if (t == 1700)
  {
    for (i = 0; i < 3; i++)
    {
      if (tenm_table_add(stage_3_more_2_new(i)) < 0)
      {
        fprintf(stderr, "scheduler_3: cannot create stage_3_more_2(%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
    return status;
  }

  if (t == 1740)
  {
    if (tenm_table_add(stage_3_more_3_new()) < 0)
    {
      fprintf(stderr, "scheduler_3: cannot create stage_3_more_3 "
              "(t = %d)\n", t);
      return SCHEDULER_ERROR;
    }
    return SCHEDULER_SUCCESS;
  }

  if (t == 2100)
  {
    if (tenm_table_add(stage_3_more_4_new()) < 0)
    {
      fprintf(stderr, "scheduler_3: cannot create stage_3_more_4 "
              "(t = %d)\n", t);
      return SCHEDULER_ERROR;
    }
    return SCHEDULER_SUCCESS;
  }

  if (t == 2460)
  {
    for (i = 0; i < 8; i++)
    {
      if (i != 1)
      {
        x = 30.0 + ((double) i * 60);

        if (tenm_table_add(wall_new(x, 60.0, 60.0, SCROLL_SPEED)) < 0)
        {
          fprintf(stderr, "scheduler_3: cannot create wall (%d) "
                  "(t = %d)\n", i, t);
          status = SCHEDULER_ERROR;
        }
      }      
    }
  }
  
  if (t == 2495)
  {
    for (i = 0; i <= 8; i++)
    {
      if (i % 3 == 0)
        color = COLOR_PURPLE;
      else
        color = COLOR_BLUE;

      if (tenm_table_add(normal_enemy_new((double) (60 * i), -14.0,
                                          NORMAL_BALL_SMALL, 0, color,
                                          0, -1, -1, 1, 1,
                                          /* move 0 */
                                          1000, -2.0, SCROLL_SPEED, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 0,
                                          /* shoot 0 */
                                          1000, 1000, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_3: cannot create normal_enemy (%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
  }

  if ((t >= 2518) && (t <= 2758) && (t % 30 == 28))
  {
    y = -14.0 + ((double) (t - 2495)) * SCROLL_SPEED;
    if (((t - 2518) / 30) % 3 == 0)
      color = COLOR_PURPLE;
    else
      color = COLOR_BLUE;

    if (tenm_table_add(normal_enemy_new(((double) WINDOW_WIDTH) + 14.0, y,
                                        NORMAL_BALL_SMALL, 0, color,
                                        0, -1, -1, 1, 1,
                                        /* move 0 */
                                        1000, -2.0, SCROLL_SPEED, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 0,
                                        /* shoot 0 */
                                        1000, 1000, 0, 0, 0)) < 0)
    {
      fprintf(stderr, "scheduler_3: cannot create normal_enemy "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
  }
  
  if (t == 2520)
  {
    for (i = 0; i < 8; i++)
    {
      if (i != 6)
      {
        x = 30.0 + ((double) i * 60);

        if (tenm_table_add(wall_new(x, 60.0, 60.0, SCROLL_SPEED)) < 0)
        {
          fprintf(stderr, "scheduler_3: cannot create wall (%d) "
                  "(t = %d)\n", i, t);
          status = SCHEDULER_ERROR;
        }
      }      
    }
  }

  if (t == 2555)
  {
    for (i = 0; i <= 8; i++)
    {
      if (i % 3 == 2)
        color = COLOR_PURPLE;
      else
        color = COLOR_RED;

      if (tenm_table_add(normal_enemy_new((double) (60 * i), -14.0,
                                          NORMAL_BALL_SMALL, 0, color,
                                          0, -1, -1, 1, 1,
                                          /* move 0 */
                                          1000, 2.0, SCROLL_SPEED, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 0,
                                          /* shoot 0 */
                                          1000, 1000, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_3: cannot create normal_enemy (%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
  }

  if ((t >= 2578) && (t <= 2818) && (t % 30 == 28))
  {
    y = -14.0 + ((double) (t - 2555)) * SCROLL_SPEED;
    if (((t - 2578) / 30) % 3 == 0)
      color = COLOR_PURPLE;
    else
      color = COLOR_RED;

    if (tenm_table_add(normal_enemy_new(-14.0 , y,
                                        NORMAL_BALL_SMALL, 0, color,
                                        0, -1, -1, 1, 1,
                                        /* move 0 */
                                        1000, 2.0, SCROLL_SPEED, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 0,
                                        /* shoot 0 */
                                        1000, 1000, 0, 0, 0)) < 0)
    {
      fprintf(stderr, "scheduler_3: cannot create normal_enemy "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
  }

  if (t == 2580)
  {
    for (i = 0; i < 8; i++)
    {
      if (i != 1)
      {
        x = 30.0 + ((double) i * 60);

        if (tenm_table_add(wall_new(x, 60.0, 60.0, SCROLL_SPEED)) < 0)
        {
          fprintf(stderr, "scheduler_3: cannot create wall (%d) "
                  "(t = %d)\n", i, t);
          status = SCHEDULER_ERROR;
        }
      }      
    }
  }

  if (t == 2615)
  {
    for (i = 0; i <= 8; i++)
    {
      if (i % 3 == 0)
        color = COLOR_PURPLE;
      else
        color = COLOR_BLUE;

      if (tenm_table_add(normal_enemy_new((double) (60 * i), -14.0,
                                          NORMAL_BALL_SMALL, 0, color,
                                          0, -1, -1, 1, 1,
                                          /* move 0 */
                                          1000, -2.0, SCROLL_SPEED, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 0,
                                          /* shoot 0 */
                                          1000, 1000, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_3: cannot create normal_enemy (%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
  }

  if ((t >= 2638) && (t <= 2878) && (t % 30 == 28))
  {
    y = -14.0 + ((double) (t - 2615)) * SCROLL_SPEED;
    if (((t - 2638) / 30) % 3 == 0)
      color = COLOR_PURPLE;
    else
      color = COLOR_BLUE;

    if (tenm_table_add(normal_enemy_new(((double) WINDOW_WIDTH) + 14.0 , y,
                                        NORMAL_BALL_SMALL, 0, color,
                                        0, -1, -1, 1, 1,
                                        /* move 0 */
                                        1000, -2.0, SCROLL_SPEED, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 0,
                                        /* shoot 0 */
                                        1000, 1000, 0, 0, 0)) < 0)
    {
      fprintf(stderr, "scheduler_3: cannot create normal_enemy "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
  }

  if (t == 2635)
  {
    for (i = 1; i <= 8; i++)
    {
      if (i % 3 == 2)
        color = COLOR_PURPLE;
      else
        color = COLOR_RED;

      if (tenm_table_add(normal_enemy_new((double) (60 * i - 40), -14.0,
                                          NORMAL_BALL_SMALL, 0, color,
                                          0, -1, -1, 1, 1,
                                          /* move 0 */
                                          1000, -2.0, SCROLL_SPEED, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 0,
                                          /* shoot 0 */
                                          1000, 1000, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_3: cannot create normal_enemy (%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
  }

  if ((t >= 2638) && (t <= 2878) && (t % 30 == 28))
  {
    y = -14.0 + ((double) (t - 2635)) * SCROLL_SPEED;
    if (((t - 2638) / 30) % 3 == 2)
      color = COLOR_PURPLE;
    else
      color = COLOR_RED;

    if (tenm_table_add(normal_enemy_new(((double) WINDOW_WIDTH) + 14.0 , y,
                                        NORMAL_BALL_SMALL, 0, color,
                                        0, -1, -1, 1, 1,
                                        /* move 0 */
                                        1000, -2.0, SCROLL_SPEED, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 0,
                                        /* shoot 0 */
                                        1000, 1000, 0, 0, 0)) < 0)
    {
      fprintf(stderr, "scheduler_3: cannot create normal_enemy "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
  }

  if (t == 2655)
  {
    for (i = 2; i <= 9; i++)
    {
      if (i % 3 == 1)
        color = COLOR_PURPLE;
      else
        color = COLOR_BLUE;

      if (tenm_table_add(normal_enemy_new((double) (60 * i - 80), -14.0,
                                          NORMAL_BALL_SMALL, 0, color,
                                          0, -1, -1, 1, 1,
                                          /* move 0 */
                                          1000, -2.0, SCROLL_SPEED, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 0,
                                          /* shoot 0 */
                                          1000, 1000, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_3: cannot create normal_enemy (%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
  }

  if ((t >= 2668) && (t <= 2908) && (t % 30 == 28))
  {
    y = -14.0 + ((double) (t - 2655)) * SCROLL_SPEED;
    if (((t - 2668) / 30) % 3 == 0)
      color = COLOR_PURPLE;
    else
      color = COLOR_BLUE;

    if (tenm_table_add(normal_enemy_new(((double) WINDOW_WIDTH) + 14.0 , y,
                                        NORMAL_BALL_SMALL, 0, color,
                                        0, -1, -1, 1, 1,
                                        /* move 0 */
                                        1000, -2.0, SCROLL_SPEED, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 0,
                                        /* shoot 0 */
                                        1000, 1000, 0, 0, 0)) < 0)
    {
      fprintf(stderr, "scheduler_3: cannot create normal_enemy "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
  }

  if (t == 2680)
  {
    for (i = 0; i < 8; i++)
    {
      if (i != 6)
      {
        x = 30.0 + ((double) i * 60);

        if (tenm_table_add(wall_new(x, 60.0, 60.0, SCROLL_SPEED)) < 0)
        {
          fprintf(stderr, "scheduler_3: cannot create wall (%d) "
                  "(t = %d)\n", i, t);
          status = SCHEDULER_ERROR;
        }
      }      
    }
  }

  if (t == 2750)
  {
    if (tenm_table_add(balloon_new(390.0, 60.0, SCROLL_SPEED,
                                   2.0, 45, 0, 20)) < 0)
    {
      fprintf(stderr, "scheduler_3: cannot create balloon "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
  }

  if (t == 2815)
  {
    if (tenm_table_add(balloon_new(90.0, 60.0, SCROLL_SPEED,
                                   2.0, 45, 0, 20)) < 0)
    {
      fprintf(stderr, "scheduler_3: cannot create balloon "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
  }

  if ((t == 2930 + 15 - 47) || (t == 2930 + 15 + 48))
  {
    if (t == 2930 + 15 - 47)
      y = 2.0 - 14.0;
    else
      y = 1.0 - 14.0;

    if (tenm_table_add(normal_enemy_new(240.0, y,
                                        NORMAL_BALL_LARGE, ENEMY_TYPE_WEAK,
                                        COLOR_PURPLE,
                                        0, -1, -1, 1, 1,
                                        /* move 0 */
                                        1000, 0.0, SCROLL_SPEED, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 0,
                                        /* shoot 0 */
                                        1000, 80, 0, 0, 0)) < 0)
    {
      fprintf(stderr, "scheduler_3: cannot create normal_enemy "
              "(t = %d)\n", t);
      return SCHEDULER_ERROR;
    }
    return SCHEDULER_SUCCESS;
  }

  if ((t == 2930 + 15 - 41) || (t == 2930 + 15 + 42))
  {
    if (t == 2930 + 15 - 41)
      y = 0.8416 - 14.0;
    else
      y = 2.1584 - 14.0;
    for (i = 0; i < 2; i++)
    {
      if (i == 0)
      {
        color = COLOR_BLUE;
        x = 240.0 - 71.5;
      }
      else
      {
        color = COLOR_RED;
        x = 240.0 + 71.5;
      }
      
      if (tenm_table_add(normal_enemy_new(x, 0.8416 -14.0,
                                          NORMAL_BALL_LARGE, ENEMY_TYPE_WEAK,
                                          color,
                                          0, -1, -1, 1, 1,
                                          /* move 0 */
                                          1000, 0.0, SCROLL_SPEED, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 0,
                                          /* shoot 0 */
                                          1000, 80, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_3: cannot create normal_enemy (%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
    return status;
  }

  if ((t == 2930 + 15 - 29) || (t == 2930 + 15 + 30))
  {
    if (t == 2930 + 15 - 29)
      y = 2.0 - 14.0;
    else
      y = 1.0 - 14.0;
    if (tenm_table_add(normal_enemy_new(240.0, y,
                                        NORMAL_BALL_LARGE, ENEMY_TYPE_WEAK,
                                        COLOR_PURPLE,
                                        0, -1, -1, 1, 1,
                                        /* move 0 */
                                        1000, 0.0, SCROLL_SPEED, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 0,
                                        /* shoot 0 */
                                        1000, 80, 0, 0, 0)) < 0)
    {
      fprintf(stderr, "scheduler_3: cannot create normal_enemy "
              "(t = %d)\n", t);
      return SCHEDULER_ERROR;
    }
    return SCHEDULER_SUCCESS;
  }

  if ((t == 2930 + 15 - 23) || (t == 2930 + 15 + 24))
  {
    if (t == 2930 + 15 - 23)
      y = 0.8416 - 14.0;
    else
      y = 2.1584 - 14.0;
    for (i = 0; i < 2; i++)
    {
      if (i == 0)
      {
        color = COLOR_BLUE;
        x = 240.0 - 123.8416;
      }
      else
      {
        color = COLOR_RED;
        x = 240.0 + 123.8416;
      }
      
      if (tenm_table_add(normal_enemy_new(x, 0.8416 -14.0,
                                          NORMAL_BALL_LARGE, ENEMY_TYPE_WEAK,
                                          color,
                                          0, -1, -1, 1, 1,
                                          /* move 0 */
                                          1000, 0.0, SCROLL_SPEED, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 0,
                                          /* shoot 0 */
                                          1000, 80, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_3: cannot create normal_enemy (%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
    return status;
  }

  if ((t == 2930 + 15 - 14) || (t == 2930 + 15 + 15))
  {
    if (t == 2930 + 15 - 14)
      y = 2.5 - 14.0;
    else
      y = 0.5 - 14.0;
    for (i = 0; i < 2; i++)
    {
      if (i == 0)
      {
        color = COLOR_RED;
        x = 240.0 - 77.0762;
      }
      else
      {
        color = COLOR_BLUE;
        x = 240.0 + 77.0762;
      }
      
      if (tenm_table_add(normal_enemy_new(x, y,
                                          NORMAL_BALL_LARGE, ENEMY_TYPE_WEAK,
                                          color,
                                          0, -1, -1, 1, 1,
                                          /* move 0 */
                                          1000, 0.0, SCROLL_SPEED, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 0,
                                          /* shoot 0 */
                                          1000, 80, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_3: cannot create normal_enemy (%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
    return status;
  }

  if (t == 2930 + 15)
  {
    for (i = 0; i < 2; i++)
    {
      if (i == 0)
      {
        color = COLOR_BLUE;
        x = 240.0 - 143.0;
      }
      else
      {
        color = COLOR_RED;
        x = 240.0 + 143.0;
      }
      
      if (tenm_table_add(normal_enemy_new(x, 0.0 - 14.0,
                                          NORMAL_BALL_LARGE, ENEMY_TYPE_WEAK,
                                          color,
                                          0, -1, -1, 1, 1,
                                          /* move 0 */
                                          1000, 0.0, SCROLL_SPEED, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 0,
                                          /* shoot 0 */
                                          1000, 80, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "scheduler_3: cannot create normal_enemy (%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
    return status;
  }
  
  if (t == 2930)
  {
    if (tenm_table_add(balloon_new(240.0, 60.0, SCROLL_SPEED,
                                   -1.0, 120, 935, 1000)) < 0)
    {
      fprintf(stderr, "scheduler_3: cannot create balloon "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
  }

  if (t == 3100)
  {
    if (tenm_table_add(balloon_new(90.0, 60.0, SCROLL_SPEED,
                                   -2.0, 25, 1015, 1000)) < 0)
    {
      fprintf(stderr, "scheduler_3: cannot create balloon "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
  }

  if ((t >= 3160) && (t <= 3360) && (t % 20 == 0))
  {
    y = -59.0 + ((double) (t - 3100)) * SCROLL_SPEED;
    if (t % 60 == 20)
      color = COLOR_PURPLE;
    else
      color = COLOR_BLUE;
    if (tenm_table_add(normal_enemy_new(-24.0, y + 81.2650,
                                        NORMAL_BALL_LARGE, 0, color,
                                        0, -1, -1, 1, 1,
                                        /* move 0 */
                                        1000, 0.0, SCROLL_SPEED, 0.0, 0.0,
                                        90.0, y, 0.0, -0.15, 0,
                                        /* shoot 0 */
                                        1000, 60, t % 60, 0, 0)) < 0)
    {
      fprintf(stderr, "scheduler_3: cannot create normal_enemy "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
  }
  
  if (t == 3140)
  {
    if (tenm_table_add(balloon_new(((double) WINDOW_WIDTH) - 90.0,
                                   110.0, SCROLL_SPEED,
                                   0.0, 1, 10, 1000)) < 0)
    {
      fprintf(stderr, "scheduler_3: cannot create balloon "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
  }

  if ((t >= 3220) && (t <= 3420) && (t % 20 == 0))
  {
    x = ((double) WINDOW_WIDTH) + 24.0;
    y = -109.0 + ((double) (t - 3140)) * SCROLL_SPEED;
    if (t % 60 == 20)
      color = COLOR_PURPLE;
    else
      color = COLOR_RED;
    if (tenm_table_add(normal_enemy_new(x, y + 81.2650,
                                        NORMAL_BALL_LARGE, 0, color,
                                        0, -1, -1, 1, 1,
                                        /* move 0 */
                                        1000, 0.0, SCROLL_SPEED, 0.0, 0.0,
                                        ((double) WINDOW_WIDTH) - 90.0, y,
                                        0.0, 0.15, 0,
                                        /* shoot 0 */
                                        1000, 60, t % 60, 0, 0)) < 0)
    {
      fprintf(stderr, "scheduler_3: cannot create normal_enemy "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
  }

  if (t == 3300)
  {
    if (tenm_table_add(balloon_new(90.0, 110.0, SCROLL_SPEED,
                                   0.0, 1, 10, 1000)) < 0)
    {
      fprintf(stderr, "scheduler_3: cannot create balloon "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
  }

  if ((t >= 3380) && (t <= 3580) && (t % 20 == 0))
  {
    y = -109.0 + ((double) (t - 3300)) * SCROLL_SPEED;
    if (t % 60 == 0)
      color = COLOR_PURPLE;
    else
      color = COLOR_RED;
    if (tenm_table_add(normal_enemy_new(-24.0, y - 81.2650,
                                        NORMAL_BALL_LARGE, 0, color,
                                        0, -1, -1, 1, 1,
                                        /* move 0 */
                                        1000, 0.0, SCROLL_SPEED, 0.0, 0.0,
                                        90.0, y, 0.0, 0.15, 0,
                                        /* shoot 0 */
                                        1000, 60, t % 60, 0, 0)) < 0)
    {
      fprintf(stderr, "scheduler_3: cannot create normal_enemy "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
  }

  if (t == 3360)
  {
    if (tenm_table_add(balloon_new(((double) WINDOW_WIDTH) - 90.0,
                                   110.0, SCROLL_SPEED,
                                   0.0, 1, 10, 1000)) < 0)
    {
      fprintf(stderr, "scheduler_3: cannot create balloon "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
  }

  if ((t >= 3440) && (t <= 3640) && (t % 20 == 0))
  {
    x = ((double) WINDOW_WIDTH) + 24.0;
    y = -109.0 + ((double) (t - 3360)) * SCROLL_SPEED;
    if (t % 60 == 0)
      color = COLOR_PURPLE;
    else
      color = COLOR_BLUE;
    if (tenm_table_add(normal_enemy_new(x, y - 81.2650,
                                        NORMAL_BALL_LARGE, 0, color,
                                        0, -1, -1, 1, 1,
                                        /* move 0 */
                                        1000, 0.0, SCROLL_SPEED, 0.0, 0.0,
                                        ((double) WINDOW_WIDTH) - 90.0, y,
                                        0.0, -0.15, 0,
                                        /* shoot 0 */
                                        1000, 60, t % 60, 0, 0)) < 0)
    {
      fprintf(stderr, "scheduler_3: cannot create normal_enemy "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
  }

  if (t == 3560)
  {
    if (tenm_table_add(balloon_new(60.0, 60.0, SCROLL_SPEED,
                                   2.0, 30, 0, 20)) < 0)
    {
      fprintf(stderr, "scheduler_3: cannot create balloon "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
  }

  if (t == 3579)
  {
    if (tenm_table_add(balloon_new(60.0 + 360.0, 98.0, SCROLL_SPEED,
                                   2.0, 30, 19, 20)) < 0)
    {
      fprintf(stderr, "scheduler_3: cannot create balloon "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
  }

  if (t == 3610)
  {
    if (tenm_table_add(balloon_new(60.0 + 151.0572, 60.0, SCROLL_SPEED,
                                   2.0, 30, 0, 20)) < 0)
    {
      fprintf(stderr, "scheduler_3: cannot create balloon "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
  }

  if (t == 3672)
  {
    if (tenm_table_add(balloon_new(60.0 + 93.1743, 84.0, SCROLL_SPEED,
                                   2.0, 30, 12, 20)) < 0)
    {
      fprintf(stderr, "scheduler_3: cannot create balloon "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
  }

  if (t == 3740)
  {
    for (i = 0; i < 8; i++)
    {
      if (i != 2)
      {
        x = 30.0 + ((double) i * 60);

        if (tenm_table_add(wall_new(x, 60.0, 60.0, SCROLL_SPEED)) < 0)
        {
          fprintf(stderr, "scheduler_3: cannot create wall (%d) "
                  "(t = %d)\n", i, t);
          status = SCHEDULER_ERROR;
        }
      }      
    }
    return status;
  }

  if ((t >= 3912) && (t <= 4296) && (t % 24 == 0))
  {
    for (i = 0; i < 2; i++)
    {
      if (i == 0)
      {
        x = -24.0;
        dx = 2.5;
        if (t <= 3960)
          color = COLOR_BLUE;
        else if (t == 3984)
          color = COLOR_PURPLE;
        else if (t <= 4056)
          color = COLOR_RED;
        else if (t == 4080)
          color = COLOR_PURPLE;
        else if (t % 72 == 0)
          color = COLOR_BLUE;
        else if (t % 72 == 24)
          color = COLOR_RED;
        else
          color = COLOR_PURPLE;
      }
      else
      {
        x = ((double) WINDOW_WIDTH) + 24.0;
        dx = -2.5;
        if (t <= 3960)
          color = COLOR_RED;
        else if (t == 3984)
          color = COLOR_PURPLE;
        else if (t <= 4056)
          color = COLOR_BLUE;
        else if (t == 4080)
          color = COLOR_PURPLE;
        else if (t % 72 == 0)
          color = COLOR_RED;
        else if (t % 72 == 24)
          color = COLOR_BLUE;
        else
          color = COLOR_PURPLE;
      }
      if (tenm_table_add(normal_enemy_new(x, 250.0,
                                          NORMAL_BALL_LARGE,
                                          ENEMY_TYPE_WEAK |ENEMY_TYPE_OBSTACLE,
                                          color,
                                          0, -1, -1, 1, 3,
                                          /* move 0 */
                                          1000, dx, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 0,
                                          /* shoot 0 */
                                          24, 12, 0, 100, 1,
                                          /* shoot 1 */
                                          24, 12, 0, 50, 2,
                                          /* shoot 2 */
                                          1000, 12, 0, 0, 2)) < 0)
      {
        fprintf(stderr, "scheduler_3: cannot create normal_enemy (%i) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
    return status;
  }

  if (t == 4400)
  {
    if (tenm_table_add(stage_3_more_5_new()) < 0)
    {
      fprintf(stderr, "scheduler_3: cannot create stage_3_more_5 "
              "(t = %d)\n", t);
      return SCHEDULER_ERROR;
    }
    return SCHEDULER_SUCCESS;
  }

  if (t == 4700)
  {
    if (tenm_table_add(message_new(((double) WINDOW_WIDTH) - 1.0, 270.0,
                                   -5.0, 0.0, 1000,
                                   "boss: Disconnection", 19)) < 0)
    {
      fprintf(stderr, "scheduler_3: cannot create message "
              "(t = %d)\n", t);
      return SCHEDULER_ERROR;
    }
    return SCHEDULER_SUCCESS;
  }

  if (t == 4860)
  {
    if (tenm_table_add(disconnection_new()) < 0)
    {
      fprintf(stderr, "scheduler_3: cannot create disconnection "
              "(t = %d)\n", t);
      return SCHEDULER_ERROR;
    }
    return SCHEDULER_SUCCESS;
  }

  return status;
}

static tenm_object *
stage_3_more_1_new(void)
{
  tenm_object *new = NULL;
  int *count = NULL;

  count = (int *) malloc(sizeof(int) * 4);
  if (count == NULL)
  {
    fprintf(stderr, "stage_3_more_1_new: malloc(count) failed\n");
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
  new = tenm_object_new("stage 3 more 1", 0, 0,
                        0, 0.0, 0.0,
                        4, count, 0, NULL, 0, NULL, 
                        (int (*)(tenm_object *, double)) NULL,
                        (int (*)(tenm_object *, tenm_object *)) NULL,
                        (int (*)(tenm_object *, const tenm_object *))
                        stage_3_more_1_act,
                        (int (*)(tenm_object *, int)) NULL);
  if (new == NULL)
  {
    fprintf(stderr, "stage_3_more_1_new: tenm_object_new failed\n");
    free(count);
    return NULL;
  }

  return new;
}

static int
stage_3_more_1_act(tenm_object *my, const tenm_object *player)
{
  int color;
  int theta;
  double x;
  double dx;
  double ddx;

  /* sanity check */
  if (my == NULL)
    return 0;

  if (my->count[2] <= 3)
  {  
    if ((my->count[1] >= 0) && (my->count[1] <= 45) && (my->count[1] % 9 == 0))
    {
      if (my->count[1] == 45)
        color = COLOR_PURPLE;
      else if (my->count[2] % 2 == 0)
        color = COLOR_RED;
      else
        color = COLOR_BLUE;

      if (my->count[2] % 2 == 0)
        x = (double) ((70 - my->count[1]) * 2);
      else
        x = (double) (WINDOW_WIDTH - ((70 - my->count[1]) * 2));

      if (my->count[2] == 0)
        theta = -60;
      else if (my->count[2] == 1)
        theta = -120;
      else if (my->count[2] == 2)
        theta = -30;
      else
        theta = -150;

      if (tenm_table_add(normal_enemy_new(x, -31.0,
                                          NORMAL_BURST, 0, color,
                                          0, my->table_index, 3, 2, 1,
                                          /* move 0 */
                                          25, 0.0, 8.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          7,
                                          8.0 * tenm_cos(theta),
                                          8.0 * tenm_sin(theta),
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 0,
                                          /* shoot 0 */
                                          1000, 29, 22, 0, 0)) < 0)
        fprintf(stderr, "stage_3_more_1_act: cannot create normal_enemy "
                "(%d, %d)\n", my->count[2], my->count[1]);
    }
    if (my->count[1] > 45)
    {
      if (my->count[3] >= 6)
      {
        if (((my->count[2] <= 3) && (my->count[0] + 45 + 186 <= 430))
            || (my->count[0] + 89 <= 430))
        {
          my->count[1] = 0;
          (my->count[2])++;
          my->count[3] = 0;
        }
      }
    }
    else
    {
      (my->count[1])++;
    }    
  }
  else if (my->count[2] == 4)
  {
    if ((my->count[0] + 89 <= 430) && (my->count[1] % 5 == 0))
    {
      if (my->count[1] % 10 == 0)
      {
        dx = 6.0;
        ddx = -0.4;
      }
      else
      {
        dx = -6.0;
        ddx = 0.4;
      }

      if (tenm_table_add(normal_enemy_new(((double) WINDOW_WIDTH) / 2.0, -31.0,
                                          NORMAL_BURST, 0, COLOR_PURPLE,
                                          0, -1, -1, 2, 1,
                                          /* move 0 */
                                          30, dx, 8.0, ddx, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          30, -dx, 8.0, -ddx, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 0,
                                          /* shoot 0 */
                                          1000, 33, (my->count[1] + 25) % 33,
                                          0, 0)) < 0)
        fprintf(stderr, "stage_3_more_1_act: cannot create normal_enemy "
                "(%d, %d)\n", my->count[2], my->count[1]);
    }
    (my->count[1])++;
  }

  (my->count[0])++;
  if (my->count[0] >= 435)
    return 1;

  return 0;
}

static tenm_object *
stage_3_more_2_new(int n)
{
  tenm_object *new = NULL;
  int *count = NULL;

  count = (int *) malloc(sizeof(int) * 4);
  if (count == NULL)
  {
    fprintf(stderr, "stage_3_more_2_new: malloc(count) failed\n");
    return NULL;
  }

  /* list of count
   * [0] total timer
   * [1] mode timer
   * [2] left/center/right
   * [3] killed
   */
  count[0] = 0;
  count[1] = 0;
  count[2] = n;
  count[3] = 0;

  new = tenm_object_new("stage 3 more 2", 0, 0,
                        0, 0.0, 0.0,
                        4, count, 0, NULL, 0, NULL, 
                        (int (*)(tenm_object *, double)) NULL,
                        (int (*)(tenm_object *, tenm_object *)) NULL,
                        (int (*)(tenm_object *, const tenm_object *))
                        stage_3_more_2_act,
                        (int (*)(tenm_object *, int)) NULL);
  if (new == NULL)
  {
    fprintf(stderr, "stage_3_more_2_new: tenm_object_new failed\n");
    free(count);
    return NULL;
  }

  return new;
}

static int
stage_3_more_2_act(tenm_object *my, const tenm_object *player)
{
  double x;

  /* sanity check */
  if (my == NULL)
    return 0;

  if ((my->count[1] == 20) && (my->count[0] < 300))
  {
    if (my->count[2] == 0)
      x = 120.0;
    else if (my->count[2] == 1)
      x = ((double) WINDOW_WIDTH) / 2.0;
    else
      x = ((double) WINDOW_WIDTH) - 120.0;

    if (tenm_table_add(normal_enemy_new(x, -23.0,
                                        NORMAL_STING, 0, COLOR_PURPLE,
                                        0, my->table_index, 3, 3, 1,
                                        /* move 0 */
                                        10, 0.0, 8.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 1,
                                        /* move 1 */
                                        300 - my->count[0], 0.0, 0.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 2,
                                        /* move 2 */
                                        1000, 0.0, -8.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 2,
                                        /* shoot 0 */
                                        1000, 15, 0, 50, 0)) < 0)
      fprintf(stderr, "stage_3_more_2_act: cannot create normal_enemy (%d) "
              "(%d)\n", my->count[2], my->count[1]);
  }

  if (my->count[1] > 20)
  {
    if (my->count[3] >= 1)
    {
      if (my->count[0] + 20 <= 300)
      {
        my->count[1] = 0;
        my->count[3] = 0;
      }
    }
  }
  else
  {
    (my->count[1])++;
  }    

  (my->count[0])++;
  if (my->count[0] >= 350)
    return 1;

  return 0;
}

static tenm_object *
stage_3_more_3_new(void)
{
  tenm_object *new = NULL;
  int *count = NULL;

  count = (int *) malloc(sizeof(int) * 4);
  if (count == NULL)
  {
    fprintf(stderr, "stage_3_more_3_new: malloc(count) failed\n");
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

  new = tenm_object_new("stage 3 more 3", 0, 0,
                        0, 0.0, 0.0,
                        4, count, 0, NULL, 0, NULL, 
                        (int (*)(tenm_object *, double)) NULL,
                        (int (*)(tenm_object *, tenm_object *)) NULL,
                        (int (*)(tenm_object *, const tenm_object *))
                        stage_3_more_3_act,
                        (int (*)(tenm_object *, int)) NULL);
  if (new == NULL)
  {
    fprintf(stderr, "stage_3_more_3_new: tenm_object_new failed\n");
    free(count);
    return NULL;
  }

  return new;
}

static int
stage_3_more_3_act(tenm_object *my, const tenm_object *player)
{
  double x;
  double dx;
  double y;
  int color;
  int time_shoot;

  /* sanity check */
  if (my == NULL)
    return 0;

  if ((my->count[1] <= 20) && (my->count[1] % 10 == 0))
  {
    y = 120.0 + ((double) my->count[2]) * 50.0;
    time_shoot = 30 - my->count[2] * 4;
    if (time_shoot < 10)
      time_shoot = 10;

    if (my->count[2] % 2 == 0)
    {
      color = COLOR_BLUE;
      x = -31.0;
      dx = 8.0;
    }
    else
    {
      color = COLOR_RED;
      x = ((double) WINDOW_WIDTH) + 31.0;
      dx = -8.0;
    }
    
    if (tenm_table_add(normal_enemy_new(x, y,
                                        NORMAL_STING, 0, color,
                                        0, my->table_index, 3, 1, 1,
                                        /* move 0 */
                                        1000, dx, 0.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 0,
                                        /* shoot 0 */
                                        1000, time_shoot, 0, 0, 0)) < 0)
      fprintf(stderr, "stage_3_more_3_act: cannot create normal_enemy (%d) "
              "(%d)\n", my->count[2], my->count[1]);
  }

  if (my->count[1] > 20)
  {
    if (my->count[3] >= 3)
    {
      if ((my->count[0] + 68 <= 310) && (my->count[2] < 5))
      {
        my->count[1] = 0;
        (my->count[2])++;
        my->count[3] = 0;
      }
    }
  }
  else
  {
    (my->count[1])++;
  }    

  (my->count[0])++;
  if (my->count[0] >= 330)
    return 1;

  return 0;
}

static tenm_object *
stage_3_more_4_new(void)
{
  tenm_object *new = NULL;
  int *count = NULL;

  count = (int *) malloc(sizeof(int) * 5);
  if (count == NULL)
  {
    fprintf(stderr, "stage_3_more_4_new: malloc(count) failed\n");
    return NULL;
  }

  /* list of count
   * [0] total timer
   * [1] mode timer
   * [2] mode
   * [3] killed
   * [4] row
   */
  count[0] = 0;
  count[1] = 0;
  count[2] = 0;
  count[3] = 0;
  count[4] = 0;

  new = tenm_object_new("stage 3 more 4", 0, 0,
                        0, 0.0, 0.0,
                        5, count, 0, NULL, 0, NULL, 
                        (int (*)(tenm_object *, double)) NULL,
                        (int (*)(tenm_object *, tenm_object *)) NULL,
                        (int (*)(tenm_object *, const tenm_object *))
                        stage_3_more_4_act,
                        (int (*)(tenm_object *, int)) NULL);
  if (new == NULL)
  {
    fprintf(stderr, "stage_3_more_4_new: tenm_object_new failed\n");
    free(count);
    return NULL;
  }

  return new;
}

static int
stage_3_more_4_act(tenm_object *my, const tenm_object *player)
{
  int i;
  double x;
  int color;
  int time_shoot_initial;
  int t_move;

  /* sanity check */
  if (my == NULL)
    return 0;

  /* max about 384 frame */
  if (my->count[2] == 0)
  {
    if ((my->count[1] <= 40) && (my->count[1] % 10 == 0))
    {
      for (i = 0; i < 7; i++)
      {
        x = ((double) (i + 1) * 50);
        time_shoot_initial = ((i + my->count[1] / 10) * 8) % 64
          - (50 - my->count[1]);
        if (i == 3)
          color = COLOR_PURPLE;
        else if ((i == 0) || (i == 2) || (i == 5))
          color = COLOR_BLUE;
        else
          color = COLOR_RED;

        if (tenm_table_add(normal_enemy_new(x, -19.0,
                                            NORMAL_BALL_MEDIUM, 0, color,
                                            0, my->table_index, 3, 5, 1,
                                            /* move 0 */
                                            50 - my->count[1], 0.0, 5.0,
                                            0.0, 0.0,
                                            0.0, 0.0, 0.0, 0.0, 1,
                                            /* move 1 */
                                            16, 5.0, 0.0, 0.0, 0.0,
                                            0.0, 0.0, 0.0, 0.0, 2,
                                            /* move 2 */
                                            10, 0.0, 5.0, 0.0, 0.0,
                                            0.0, 0.0, 0.0, 0.0, 3,
                                            /* move 3 */
                                            16, -5.0, 0.0, 0.0, 0.0,
                                            0.0, 0.0, 0.0, 0.0, 4,
                                            /* move 4 */
                                            10, 0.0, 5.0, 0.0, 0.0,
                                            0.0, 0.0, 0.0, 0.0, 1,
                                            /* shoot 0 */
                                            1000, 64, time_shoot_initial,
                                            0, 0)) < 0)
          fprintf(stderr, "stage_3_more_4_act: cannot create normal_enemy "
                  "(%d) "
                  "(%d, %d)\n", i, my->count[2], my->count[1]);
      }
    }
    if (my->count[1] > 40)
    {
      if (my->count[3] >= 35)
      {
        my->count[1] = 0;
        (my->count[2])++;
        my->count[3] = 0;
        my->count[4] = (my->count[0] - (50 + 16 - 26)) / 26;
      }
    }
    else
    {
      (my->count[1])++;
    }    
  }
  else if (my->count[2] == 1)
  {
    if ((my->count[4] <= 7) && (my->count[1] >= 20) && (my->count[1] <= 30)
        && (my->count[1] % 5 == 0))
    {
      for (i = 0; i < 6; i++)
      {  
        x = ((double) (i + 1) * 50);
        t_move = 20 + my->count[4] * 5 - (my->count[1] - 20);
        if (my->count[1] == 25)
        {
          if (i == 0)
            color = COLOR_PURPLE;
          else
            color = COLOR_RED;
        }
        else
        {
          if (i == 5)
            color = COLOR_PURPLE;
          else
            color = COLOR_BLUE;
        }

        if (tenm_table_add(normal_enemy_new(x, -19.0,
                                            NORMAL_BALL_MEDIUM, 0, color,
                                            0, -1, -1, 5, 1,
                                            /* move 0 */
                                            t_move, 0.0, 10.0,
                                            0.0, 0.0,
                                            0.0, 0.0, 0.0, 0.0, 1,
                                            /* move 1 */
                                            26, 5.0, 0.0, 0.0, 0.0,
                                            0.0, 0.0, 0.0, 0.0, 2,
                                            /* move 2 */
                                            10, 0.0, 5.0, 0.0, 0.0,
                                            0.0, 0.0, 0.0, 0.0, 3,
                                            /* move 3 */
                                            26, -5.0, 0.0, 0.0, 0.0,
                                            0.0, 0.0, 0.0, 0.0, 4,
                                            /* move 4 */
                                            10, 0.0, 5.0, 0.0, 0.0,
                                            0.0, 0.0, 0.0, 0.0, 1,
                                            /* shoot 0 */
                                            1000, 1000, 0, 0, 0)) < 0)
          fprintf(stderr, "stage_3_more_4_act: cannot create normal_enemy "
                  "(%d) "
                  "(%d, %d)\n", i, my->count[2], my->count[1]);
      }
    }
    if (my->count[1] <= 40)
      (my->count[1])++;
  }
  
  (my->count[0])++;
  if (my->count[0] >= 420)
    return 1;

  return 0;
}

static tenm_object *
stage_3_more_5_new(void)
{
  tenm_object *new = NULL;
  int *count = NULL;

  count = (int *) malloc(sizeof(int) * 4);
  if (count == NULL)
  {
    fprintf(stderr, "stage_3_more_5_new: malloc(count) failed\n");
    return NULL;
  }

  /* list of count
   * [0] total timer
   * [1] time since the last enemy appeared
   * [2] number of enemies appeared
   * [3] killed
   */
  count[0] = 0;
  count[1] = 0;
  count[2] = 0;
  count[3] = 0;

  new = tenm_object_new("stage 3 more 5", 0, 0,
                        0, 0.0, 0.0,
                        4, count, 0, NULL, 0, NULL,
                        (int (*)(tenm_object *, double)) NULL,
                        (int (*)(tenm_object *, tenm_object *)) NULL,
                        (int (*)(tenm_object *, const tenm_object *))
                        stage_3_more_5_act,
                        (int (*)(tenm_object *, int)) NULL);
  if (new == NULL)
  {
    fprintf(stderr, "stage_3_more_5_new: tenm_object_new failed\n");
    free(count);
    return NULL;
  }

  return new;
}

static int
stage_3_more_5_act(tenm_object *my, const tenm_object *player)
{
  int theta;
  double x;

  /* sanity check */
  if (my == NULL)
    return 0;

  (my->count[1])++;
  if ((my->count[0] <= 200) && (my->count[2] - my->count[3] <= 4))
  {
    if (my->count[1] >= 5)
    {
      if (my->count[2] % 2 == 0)
      {
        x = 0.0;
        theta = (my->count[2] % 8) * 5 + 20;
      }
      else
      {
        x = (double) WINDOW_WIDTH;
        theta = 180 - ((my->count[2] % 8) * 5 + 20);
      }
      
      if (tenm_table_add(normal_enemy_new(x, -31.0,
                                          NORMAL_BURST, 0, COLOR_PURPLE,
                                          0, my->table_index, 3, 1, 1,
                                          /* move 0 */
                                          1000,
                                          8.0 * tenm_cos(theta),
                                          8.0 * tenm_sin(theta),
                                          0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 0,
                                          /* shoot 0 */
                                          1000, 15, 0, 0, 0)) < 0)
        fprintf(stderr, "stage_3_more_5_act: cannot create normal_enemy (%d) "
                "(%d)\n", my->count[2], my->count[1]);
      my->count[1] = 0;
      (my->count[2])++;
    }
  }

  (my->count[0])++;
  if (my->count[0] >= 450)
    return 1;

  return 0;
}
