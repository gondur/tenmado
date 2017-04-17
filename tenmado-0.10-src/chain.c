/* $Id: chain.c,v 1.22 2003/01/12 02:04:57 oohara Exp $ */

#include <stdio.h>
/* strlen */
#include <string.h>

#include "const.h"
#include "util.h"
#include "score.h"
#include "tenm_math.h"

#include "chain.h"

/* the number of colors are hardcoded, no need for malloc */
static int color_expected = COLOR_PURPLE;
static int chain = 0;
static int killed[3];

static void add_chain(int color, int delta);
static void add_chain2(int delta);

void
clear_chain(void)
{
  int i;

  color_expected = COLOR_PURPLE;
  chain = 0;
  for (i = 0; i < 3; i++)
    killed[i] = 0;
}

int
get_color_expected(void)
{
  return color_expected;
}

/* return 0 on success, 1 on error */
int
add_killed(int color)
{
  switch (color)
  {
  case COLOR_BLUE:
    (killed[0])++;
    break;
  case COLOR_PURPLE:
    (killed[1])++;
    break;
  case COLOR_RED:
    (killed[2])++;
    break;
  case COLOR_GREEN:
    fprintf(stderr, "add_killed: color is COLOR_GREEN\n");
    return 1;
    break;
  default:
    fprintf(stderr, "add_killed: strange color (%d)\n", color);
    break;
  }
  return 0;
}

void
calculate_chain(void)
{
  switch (color_expected)
  {
  case COLOR_BLUE:
    if (killed[0] > 0)
    {
      add_chain(COLOR_BLUE, killed[0]);
      killed[0] = 0;
    }
    if (killed[2] > 0)
    {
      add_chain(COLOR_RED, killed[2]);
      killed[2] = 0;
    }
    if (killed[1] > 0)
    {
      add_chain(COLOR_PURPLE, killed[1]);
      killed[1] = 0;
    }
    break;
  case COLOR_PURPLE:
    if (killed[0] >= killed[2])
    {
      if (killed[0] > 0)
      {
        add_chain(COLOR_BLUE, killed[0]);
        killed[0] = 0;
      }
      if (killed[2] > 0)
      {
        add_chain(COLOR_RED, killed[2]);
        killed[2] = 0;
      }
    }
    else
    {
      if (killed[2] > 0)
      {
        add_chain(COLOR_RED, killed[2]);
        killed[2] = 0;
      }
      if (killed[0] > 0)
      {
        add_chain(COLOR_BLUE, killed[0]);
        killed[0] = 0;
      }
    }
    if (killed[1] > 0)
    {
      add_chain(COLOR_PURPLE, killed[1]);
      killed[1] = 0;
    }
    break;
  case COLOR_RED:
    if (killed[2] > 0)
    {
      add_chain(COLOR_RED, killed[2]);
      killed[2] = 0;
    }
    if (killed[0] > 0)
    {
      add_chain(COLOR_BLUE, killed[0]);
      killed[0] = 0;
    }
    if (killed[1] > 0)
    {
      add_chain(COLOR_PURPLE, killed[1]);
      killed[1] = 0;
    }
    break;
  default:
    fprintf(stderr, "calculate_chain: strange color_expected (%d)\n",
            color_expected);
    break;
  }
}

static void
add_chain(int color, int delta)
{
  /* sanity check */
  if (delta <= 0)
    return;
  if ((color != COLOR_BLUE) && (color != COLOR_PURPLE) && (color != COLOR_RED))
  {
    fprintf(stderr, "add_chain: strange color (%d)\n", color);
    return;
  }

  if (color == COLOR_PURPLE)
  {
    add_chain2(delta);
    color_expected = COLOR_PURPLE;
    return;
  }
  if (color_expected == COLOR_PURPLE)
  {
    add_chain2(delta);
    color_expected = color;
    return;
  }
  if (color == color_expected)
  {
    add_chain2(delta);
    return;
  }
  /* check if there is a purple enemy which can join the old chain and
   * this chain
   */
  if (killed[1] > 0)
  {
    /* assume that the player killed one purple enemy before this chain */
    add_chain2(1);
    (killed[1])--;
    add_chain2(delta);
    color_expected = color;
    return;
  }

  /* the old chain is really cut, start a new chain */
  chain = 0;
  if (delta >= 2)
    add_chain2(delta - 1);
  color_expected = color;
}

static void
add_chain2(int delta)
{
  int i;

  /* sanity check */
  if (delta <= 0)
    return;
  /* add chain bouns */
  for (i = 1; i <= delta; i++)
    if (chain + i - 1 >= 10)
      add_score(tenm_pow2(10));
    else
      add_score(tenm_pow2(chain + i - 1));

  chain += delta;
}

/* return 0 on success, 1 on error */
int
show_chain(void)
{
  char temp[16];

  /* sanity check */
  if (chain < 0)
    return 0;

  sprintf(temp, "chain %3d", chain);
  if (draw_string(WINDOW_WIDTH - 90, 10, temp, (int) strlen(temp)) != 0)
  {
    fprintf(stderr, "show_chain: draw_string failed\n");
    return 1;
  }
  return 0;
}
