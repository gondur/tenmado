/* $Id: tenm_sdl_init.c,v 1.2 2003/01/14 03:04:59 oohara Exp $ */

#include <stdio.h>
/* atexit */
#include <stdlib.h>
/* errno */
#include <errno.h>
/* strerror */
#include <string.h>

#include <SDL.h>

#include "tenm_sdl_init.h"

/* return 0 on success, 1 on error */
int
tenm_sdl_init(Uint32 flags)
{
  if (SDL_WasInit(flags) != flags)
  {
    if (SDL_WasInit(SDL_INIT_EVERYTHING) != 0)
    {
      if (SDL_InitSubSystem(flags) != 0)
      {
        {
          fprintf(stderr, "tenm_sdl_init: cannot initialize subsystem of SDL "
                  "(%s)\n", SDL_GetError());
          return 1;
        }
      }
    }
    else
    {
      if (SDL_Init(flags) != 0)
      {
        fprintf(stderr, "tenm_sdl_init: cannot initialize SDL "
                "(%s)\n", SDL_GetError());
        return 1;
      }
      errno = 0;
      if (atexit(SDL_Quit) != 0)
      {
        fprintf(stderr, "tenm_sdl_init: "
                "cannot register SDL_Quit to exit");
        if (errno != 0)
          fprintf(stderr, " (%s)", strerror(errno));
        fprintf(stderr, "\n");
        SDL_Quit();
        return 1;
      }
    }
  }

  return 0;
}
