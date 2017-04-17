/* $Id: tutorial.c,v 1.47 2003/02/23 14:27:22 oohara Exp $ */

#include <stdio.h>

#include "tenm_table.h"
#include "const.h"
#include "message.h"
#include "normal-enemy.h"
#include "tenm_object.h"
#include "laser.h"
#include "wall.h"

#include "tutorial.h"

int
tutorial(int t)
{
  int i;
  int status = SCHEDULER_SUCCESS;
  int line_number;
  int life;
  const char *string = NULL;
  int length = 0;
  double x;
  int color;

  /* sanity check */
  if (t < 0)
  {
    fprintf(stderr, "tutorial: strange t (%d)\n", t);
    return SCHEDULER_ERROR;
  }

  if (t >= 3260)
    return SCHEDULER_NEXT_STAGE;

  if (t == 0)
  {
    if (tenm_table_add(message_new(160.0, ((double) WINDOW_HEIGHT) - 40.0,
                                   0.0, 0.0, 4000,
                                   "press ESC to quit", 17)) < 0)
    {
      fprintf(stderr, "tutorial: cannot create message (quit instruction) "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
  }

  if (t == 30)
    line_number = 1;
  else if (t == 170)
    line_number = 1;
  else if (t == 360)
    line_number = 1;
  else if (t == 550)
    line_number = 2;
  else if (t == 740)
    line_number = 3;
  else if (t == 1020)
    line_number = 3;
  else if (t == 1310)
    line_number = 4;
  else if (t == 1540)
    line_number = 3;
  else if (t == 1770)
    line_number = 6;
  else if (t == 2160)
    line_number = 4;
  else if (t == 2470)
    line_number = 5;
  else if (t == 2880)
    line_number = 1;
  else if (t == 3070)
    line_number = 2;
  else
    line_number = 0;

  for (i = 1; i <= line_number; i++)
  {
    if (t == 30)
    {
      life = 100;
      string = "tenmado tutorial";
      length = 16;
    }
    else if (t == 170)
    {
      life = 160;
      string = "Use cursor keys to move.";
      length = 24;
    }
    else if (t == 360)
    {
      life = 160;
      string = "Press space to shoot (auto-repeat).";
      length = 35;
    }
    else if (t == 550)
    {
      life = 140;
      if (i == 1)
      {
        string = "The player shot turns black";
        length = 27;
      }
      else
      {
        string = "as it flies.";
        length = 12;
      }
    }
    else if (t == 740)
    {
      life = 260;
      if (i == 1)
      {
        string = "A black shot deals 1 damage.";
        length = 28;
      }
      else if (i == 2)
      {
        string = "(See the hit status at the upper";
        length = 32;
      }
      else
      {
        string = "left corner of the window.)";
        length = 27;
      }
    }
    else if (t == 1020)
    {
      life = 260;
      if (i == 1)
      {
        string = "A non-black shot deals 2 damage.";
        length = 32;
      }
      else if (i == 2)
      {
        string = "Keep close to an enemy";
        length = 22;
      }
      else
      {
        string = "to destroy it quickly.";
        length = 22;
      }
    }
    else if (t == 1310)
    {
      life = 200;
      if (i == 1)
      {
        string = "If your ship gets hit by";
        length = 24;
      }
      else if (i == 2)
      {
        string = "an enemy shot, an enemy ship or";
        length = 31;
      }
      else if (i == 3)
      {
        string = "any other obstacle,";
        length = 19;
      }
      else
      {
        string = "your ship is destroyed.";
        length = 23;
      }
    }
    else if (t == 1540)
    {
      life = 200;
      if (i == 1)
      {
        string = "The black circle at the center is";
        length = 33;
      }
      else if (i == 2)
      {
        string = "the only weak point of your ship.";
        length = 33;
      }
      else
      {
        string = "The rest is safe.";
        length = 17;
      }
    }
    else if (t == 1770)
    {
      life = 360;
      if (i == 1)
      {
        string = "You can get an additional score by";
        length = 34;
      }
      else if (i == 2)
      {
        string = "destroying enemies of the same";
        length = 30;
      }
      else if (i == 3)
      {
        string = "color successively.  This is";
        length = 29;
      }
      else if (i == 4)
      {
        string = "called the \"color chain bonus\".";
        length = 31;
      }
      else if (i == 5)
      {
        string = "(See the chain status at the upper";
        length = 34;
      }
      else
      {
        string = "right corner of the window.)";
        length = 28;
      }
    }
    else if (t == 2160)
    {
      life = 280;
      if (i == 1)
      {
        string = "The color of your ship indicates";
        length = 32;
      }
      else if (i == 2)
      {
        string = "which color of enemy you should";
        length = 31;
      }
      else if (i == 3)
      {
        string = "destroy to get the color chain";
        length = 30;
      }
      else
      {
        string = "bonus.";
        length = 6;
      }
    }
    else if (t == 2470)
    {
      life = 380;
      if (i == 1)
      {
        string = "Purple is special --- you always";
        length = 32;
      }
      else if (i == 2)
      {
        string = "get the color chain bonus if you";
        length = 32;
      }
      else if (i == 3)
      {
        string = "destroy a purple enemy or if the";
        length = 32;
      }
      else if (i == 4)
      {
        string = "enemy you destroyed last is";
        length = 27;
      }
      else
      {
        string = "purple.";
        length = 7;
      }
    }
    else if (t == 2880)
    {
      life = 160;
      string = "You cannot destroy a green enemy.";
      length = 33;
    }
    else if (t == 3070)
    {
      life = 160;
      if (i == 1)
      {
        string = "There are 5 stages in this game.";
        length = 32;
      }
      else
      {
        /* well, you won't see many rand() in this game,
         * but anyway
         */
        string = "Good luck!";
        length = 10;
      }
    }
    else
    {
      fprintf(stderr, "tutorial: string not found (line %d) "
              "(t = %d)\n", i, t);
      life = 1;
      string = NULL;
      length = 0;
    }
    if (tenm_table_add(message_new(160.0, 60.0 + ((double) (i * 30)),
                                   0.0, 0.0, life, string, length)) < 0)
    {
      fprintf(stderr, "tutorial: cannot create message (line %d)"
              "(t = %d)\n", i, t);
      status = SCHEDULER_ERROR;
    }
  }

  if (t == 710)
  {
    if (tenm_table_add(normal_enemy_new(104.0, -23.0,
                                        NORMAL_STING, 0, COLOR_BLUE,
                                        0, -1, -1, 4, 1,
                                        /* move 0 */
                                        20, 0.0, 8.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 1,
                                        /* move 1 */
                                        1030, 0.0, 0.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 2,
                                        /* move 2 */
                                        30, 0.0, 8.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 3,
                                        /* move 3 */
                                        3000, 0.0, 0.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 3,
                                        /* shoot 0 */
                                        3000, 3000, 2350, 0, 0)) < 0)
    {
      fprintf(stderr, "tutorial: cannot create normal_enemy "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
  }
  
  if ((t >= 1540) && (t < 1740) && (t % 5 == 0))
  {
    for (i = 0; i < 2; i++)
    {
      if (t % 10 == 0)
      {
        if (i == 0)
          x = 68.0;
        else
          x = 92.0;
      }
      else
      {
        if (i == 0)
          x = 73.0;
        else
          x = 87.0;
      }

      if (tenm_table_add(laser_angle_new(x, -29.0, 25.0, 90,
                                         30.0, COLOR_GREEN, 1000)) < 0)
      {
        fprintf(stderr, "tutorial: cannot create laser_angle (%d) "
                "(t = %d)\n", i, t);
        status = SCHEDULER_ERROR;
      }
    }
  }

  if ((t >= 1770) && (t <= 1800) && (t % 15 == 0))
  {
    if (tenm_table_add(normal_enemy_new(80.0, -24.0,
                                        NORMAL_BALL_LARGE, 0, COLOR_BLUE,
                                        0, -1, -1, 2, 1,
                                        /* move 0 */
                                        1820 - t, 0.0, 5.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 1,
                                        /* move 1 */
                                        1000, 0.0, 0.0, 0.0, 0.0,
                                        0.0, 0.0, 0.0, 0.0, 2,
                                        /* shoot 0 */
                                        3000, 3000, 0, 0, 0)) < 0)
    {
      fprintf(stderr, "tutorial: cannot create normal_enemy "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
  }

  if (t == 2160)
  {
    for (i = 0; i < 2; i++)
    {
      if (i == 0)
      {
        x = 50.0;
        color = COLOR_BLUE;
      }
      else
      {
        x = 110.0;
        color = COLOR_RED;
      }
      if (tenm_table_add(normal_enemy_new(x, -24.0,
                                          NORMAL_BALL_LARGE, 0, color,
                                          0, -1, -1, 2, 1,
                                          /* move 0 */
                                          50, 0.0, 5.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          1000, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* shoot 0 */
                                          3000, 3000, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "tutorial: cannot create normal_enemy "
                "(t = %d)\n", t);
        status = SCHEDULER_ERROR;
      }
    }
  }

  if ((t >= 2460) && (t <= 2505) && (t % 15 == 0))
  {
    for (i = 0; i < 2; i++)
    {
      if (i == 0)
      {
        x = 50.0;
        if (t % 30 == 0)
          color = COLOR_BLUE;
        else
          color = COLOR_RED;
      }
      else
      {
        x = 110.0;
        color = COLOR_PURPLE;
      }
      if (tenm_table_add(normal_enemy_new(x, -24.0,
                                          NORMAL_BALL_LARGE, 0, color,
                                          0, -1, -1, 2, 1,
                                          /* move 0 */
                                          2525 - t, 0.0, 5.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 1,
                                          /* move 1 */
                                          1000, 0.0, 0.0, 0.0, 0.0,
                                          0.0, 0.0, 0.0, 0.0, 2,
                                          /* shoot 0 */
                                          3000, 3000, 0, 0, 0)) < 0)
      {
        fprintf(stderr, "tutorial: cannot create normal_enemy "
                "(t = %d)\n", t);
        status = SCHEDULER_ERROR;
      }
    }
  }

  if (t == 2880)
  {
    if (tenm_table_add(wall_new(80.0, 60.0, 60.0, 5.0)) < 0)
    {
      fprintf(stderr, "tutorial: cannot create wall "
              "(t = %d)\n", t);
      status = SCHEDULER_ERROR;
    }
  }

  return status;
}
