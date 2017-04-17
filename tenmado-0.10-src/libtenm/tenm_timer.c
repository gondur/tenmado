/* $Id: tenm_timer.c,v 1.65 2009/11/10 18:09:57 oohara Exp $ */

#include <stdio.h>
/* exit */
#include <stdlib.h>
#include <SDL.h>

#include "tenm_sdl_init.h"

#include "tenm_timer.h"

/* The problem of SDL timer is that its granularity is 10 ms, that is,
 * if the machine doesn't have a good hardware clock, whenever SDL_Delay()
 * returns, (SDL_GetTicks() % 10) is always same.  In this case,
 * a simple SDL_Delay(delay - (now - past)) is not enough.
 */

static Uint32 tick = 0;
static Uint32 tick_master = 0;
static int number_wait = 0;
static int clock_granularity = 0;

/* return 0 on success, 1 on error */
int
tenm_timer_init(int num_wait)
{
  int i;
  int t;
  Uint32 tick_begin;

  /* sanity check */
  if (num_wait <= 0)
  {
    fprintf(stderr, "tenm_timer_init: strange num_wait (%d)\n", num_wait);
    return 1;
  }

  if (tenm_sdl_init(SDL_INIT_TIMER) != 0)
  {
    fprintf(stderr, "tenm_timer_init: cannot initialize SDL timer\n");
    return 1;
  }

  number_wait = num_wait;
  /* jump to another 10 ms */
  SDL_Delay(10);

  /* clock accuracy tests */
  tick_begin = SDL_GetTicks();
  for (i = 0; i < 10; i++)
    SDL_Delay(1);
  t = SDL_GetTicks() - tick_begin;
  if (t > 0)
    t = t / 10 + 1;
  if (t < 1)
    t = 1;
  clock_granularity = t;

  tenm_timer_reset();
  return 0;
}

void
tenm_timer_reset(void)
{
  tick_master = SDL_GetTicks();
  tick = tick_master;
}

/* return 0 on success, 1 on error */
int
tenm_wait_next_frame(void)
{
  Uint32 tick_now;

  tick_now = SDL_GetTicks();
  if (tick_now < tick)
  {
    fprintf(stderr, "tenm_wait_next_frame: SDL_GetTicks ticks backward!\n");
    return 1;
  }

  /* yes, this is stupid, but it offers maximum accuracy */
  if (clock_granularity> 0)
  { 
    while (tick_now + clock_granularity < tick + number_wait * 10)
    {
      SDL_Delay(1);
      tick_now = SDL_GetTicks();
    }
  }
  while (tick_now < tick + number_wait * 10)
    tick_now = SDL_GetTicks();

  tick = SDL_GetTicks();

  return 0;
}

/* return average number of frames per second since tenm_timer_init() or
 * tenm_timer_reset() is called
 * return -1.0 on error */
double
tenm_calculate_fps(int frame_passed)
{
  Uint32 time_passed;

  /* sanity check */
  if (frame_passed < 0)
  {
    fprintf(stderr, "tenm_calculate_fps: "
            "negative frame_passed\n");
    return -1.0;
  }

  time_passed = SDL_GetTicks();
  if (time_passed < tick_master)
  {
    fprintf(stderr, "tenm_calculate_fps: "
            "SDL_GetTicks ticks backward!\n");
    return -1.0;
  }
  if (time_passed == tick_master)
  {
    fprintf(stderr, "tenm_calculate_fps: "
            "no time passed\n");
    return -1.0;
  }

  time_passed -= tick_master;
  return 1000.0 * ((double) frame_passed) / ((double) time_passed);
}
