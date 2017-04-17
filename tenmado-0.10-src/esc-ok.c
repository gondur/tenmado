/* $Id: esc-ok.c,v 1.4 2002/07/13 05:02:11 oohara Exp $ */

#include <stdio.h>

#include "esc-ok.h"

static int esc_ok = 1;

void
set_esc_ok(int n)
{
  esc_ok = n;
}

int
get_esc_ok(void)
{
  return esc_ok;
}
