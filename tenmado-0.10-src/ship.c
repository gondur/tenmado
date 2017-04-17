/* $Id: ship.c,v 1.15 2003/01/12 02:09:12 oohara Exp $ */

#include <stdio.h>
/* strlen */
#include <string.h>

#include "util.h"
#include "const.h"

#include "ship.h"

static int ship = 5;

void
set_ship(int n)
{
  ship = n;
}

/* return the new value of ships */
int
add_ship(int delta)
{
  ship += delta;
  if (ship < -1)
    ship = -1;
  return ship;
}

int
get_ship(void)
{
  return ship;
}

/* return 0 on success, 1 on error */
int
show_ship(void)
{
  char temp[16];

  /* sanity check */
  if (ship < 0)
    return 0;

  sprintf(temp, "ship %3d", ship);
  if (draw_string(WINDOW_WIDTH - 80, WINDOW_HEIGHT - 10,
                  temp, (int) strlen(temp)) != 0)
  {
    fprintf(stderr, "show_ship: draw_string failed\n");
    return 1;
  }
  return 0;
}
