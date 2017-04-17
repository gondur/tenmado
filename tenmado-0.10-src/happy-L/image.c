/* $Id: image.c,v 1.46 2003/07/09 13:38:59 oohara Exp $ */

/* NOT_HAVE_POSIX */
#include <config.h>

#include <stdio.h>

#include "tenm_graphic.h"

#include "image.h"

#ifdef NOT_HAVE_POSIX
#define IMAGEDIR "./happy-L/image"
#else /* not NOT_HAVE_POSIX */
#define IMAGEDIR DATADIR "/games/tenmado/image"
#endif /* not NOT_HAVE_POSIX */

static tenm_image *player_image = NULL;
static tenm_image *small_ball_image = NULL;
static tenm_image *medium_ball_image = NULL;
static tenm_image *large_ball_image = NULL;
static tenm_image *sting_image = NULL;
static tenm_image *horizon_image = NULL;
static tenm_image *plenty_image = NULL;
static tenm_image *burst_image = NULL;
static tenm_image *disconnection_core_image = NULL;
static tenm_image *disconnection_bit_image = NULL;
static tenm_image *fud_core_image = NULL;
static tenm_image *intuition_image = NULL;
static tenm_image *last_boss_image = NULL;
static tenm_image *last_boss_demo_image = NULL;
static tenm_image *midnight_image = NULL;
static tenm_image *midnight_bit_image = NULL;
static tenm_image *overrun_image = NULL;
static tenm_image *overrun_bit_image = NULL;
static tenm_image *solution_image = NULL;

int
image_init(void)
{
  int status = 0;

  player_image = tenm_load_image(3,
                                 IMAGEDIR "/player-blue.png", 1, 1,
                                 tenm_map_color(255, 255, 255),
                                 IMAGEDIR "/player-purple.png", 1, 1,
                                 tenm_map_color(255, 255, 255),
                                 IMAGEDIR "/player-red.png", 1, 1,
                                 tenm_map_color(255, 255, 255));
  if (player_image == NULL)
  {
    fprintf(stderr, "image_init: tenm_load_image(player_image) failed\n");
    status = 1;
  }

  small_ball_image = tenm_load_image(3,
                                     IMAGEDIR "/small-ball-blue.png", 1, 1,
                                     tenm_map_color(255, 255, 255),
                                     IMAGEDIR "/small-ball-purple.png", 1, 1,
                                     tenm_map_color(255, 255, 255),
                                     IMAGEDIR "/small-ball-red.png", 1, 1,
                                     tenm_map_color(255, 255, 255));
  if (small_ball_image == NULL)
  {
    fprintf(stderr, "image_init: tenm_load_image(small_ball_image) failed\n");
    status = 1;
  }

  medium_ball_image = tenm_load_image(3,
                                      IMAGEDIR "/medium-ball-blue.png", 1, 1,
                                      tenm_map_color(255, 255, 255),
                                      IMAGEDIR "/medium-ball-purple.png", 1, 1,
                                      tenm_map_color(255, 255, 255),
                                      IMAGEDIR "/medium-ball-red.png", 1, 1,
                                      tenm_map_color(255, 255, 255));
  if (medium_ball_image == NULL)
  {
    fprintf(stderr, "image_init: tenm_load_image(medium_ball_image) failed\n");
    status = 1;
  }

  large_ball_image = tenm_load_image(3,
                                     IMAGEDIR "/large-ball-blue.png", 1, 1,
                                     tenm_map_color(255, 255, 255),
                                     IMAGEDIR "/large-ball-purple.png", 1, 1,
                                     tenm_map_color(255, 255, 255),
                                     IMAGEDIR "/large-ball-red.png", 1, 1,
                                     tenm_map_color(255, 255, 255));
  if (large_ball_image == NULL)
  {
    fprintf(stderr, "image_init: tenm_load_image(large_ball_image) failed\n");
    status = 1;
  }

  sting_image = tenm_load_image(6,
                                IMAGEDIR "/sting-blue.png", 1, 1,
                                tenm_map_color(255, 255, 255),
                                IMAGEDIR "/sting-purple.png", 1, 1,
                                tenm_map_color(255, 255, 255),
                                IMAGEDIR "/sting-red.png", 1, 1,
                                tenm_map_color(255, 255, 255),
                                IMAGEDIR "/sting-blue-damaged.png", 1, 1,
                                tenm_map_color(255, 255, 255),
                                IMAGEDIR "/sting-purple-damaged.png", 1, 1,
                                tenm_map_color(255, 255, 255),
                                IMAGEDIR "/sting-red-damaged.png", 1, 1,
                                tenm_map_color(255, 255, 255));

  if (sting_image == NULL)
  {
    fprintf(stderr, "image_init: tenm_load_image(sting_image) failed\n");
    status = 1;
  }

  horizon_image = tenm_load_image(6,
                                  IMAGEDIR "/horizon-blue.png", 1, 1,
                                  tenm_map_color(255, 255, 255),
                                  IMAGEDIR "/horizon-purple.png", 1, 1,
                                  tenm_map_color(255, 255, 255),
                                  IMAGEDIR "/horizon-red.png", 1, 1,
                                  tenm_map_color(255, 255, 255),
                                  IMAGEDIR "/horizon-blue-damaged.png", 1, 1,
                                  tenm_map_color(255, 255, 255),
                                  IMAGEDIR "/horizon-purple-damaged.png", 1, 1,
                                  tenm_map_color(255, 255, 255),
                                  IMAGEDIR "/horizon-red-damaged.png", 1, 1,
                                  tenm_map_color(255, 255, 255));
  if (horizon_image == NULL)
  {
    fprintf(stderr, "image_init: tenm_load_image(horizon_image) failed\n");
    status = 1;
  }

  plenty_image = tenm_load_image(6,
                                 IMAGEDIR "/plenty-blue.png", 1, 1,
                                 tenm_map_color(255, 255, 255),
                                 IMAGEDIR "/plenty-purple.png", 1, 1,
                                 tenm_map_color(255, 255, 255),
                                 IMAGEDIR "/plenty-red.png", 1, 1,
                                 tenm_map_color(255, 255, 255),
                                 IMAGEDIR "/plenty-blue-damaged.png", 1, 1,
                                 tenm_map_color(255, 255, 255),
                                 IMAGEDIR "/plenty-purple-damaged.png", 1, 1,
                                 tenm_map_color(255, 255, 255),
                                 IMAGEDIR "/plenty-red-damaged.png", 1, 1,
                                 tenm_map_color(255, 255, 255));
  if (plenty_image == NULL)
  {
    fprintf(stderr, "image_init: tenm_load_image(plenty_image) failed\n");
    status = 1;
  }

  burst_image = tenm_load_image(6,
                                IMAGEDIR "/burst-blue.png", 1, 1,
                                tenm_map_color(255, 255, 255),
                                IMAGEDIR "/burst-purple.png", 1, 1,
                                tenm_map_color(255, 255, 255),
                                IMAGEDIR "/burst-red.png", 1, 1,
                                tenm_map_color(255, 255, 255),
                                IMAGEDIR "/burst-blue-damaged.png", 1, 1,
                                tenm_map_color(255, 255, 255),
                                IMAGEDIR "/burst-purple-damaged.png", 1, 1,
                                tenm_map_color(255, 255, 255),
                                IMAGEDIR "/burst-red-damaged.png", 1, 1,
                                tenm_map_color(255, 255, 255));
  if (burst_image == NULL)
  {
    fprintf(stderr, "image_init: tenm_load_image(burst_image) failed\n");
    status = 1;
  }

  disconnection_core_image = tenm_load_image(2,
                                             IMAGEDIR
                                             "/disconnection-core.png", 1, 1,
                                             tenm_map_color(255, 255, 255),
                                             IMAGEDIR
                                             "/disconnection-core-damaged.png",
                                             1, 1,
                                             tenm_map_color(255, 255, 255));

  if (disconnection_core_image == NULL)
  {
    fprintf(stderr, "image_init: tenm_load_image(disconnection_core_image) "
            "failed\n");
    status = 1;
  }

  disconnection_bit_image = tenm_load_image(1,
                                            IMAGEDIR "/disconnection-bit.png", 1, 1,
                                            tenm_map_color(255, 255, 255));

  if (disconnection_bit_image == NULL)
  {
    fprintf(stderr, "image_init: tenm_load_image(disconnection_bit_image) "
            "failed\n");
    status = 1;
  }

  fud_core_image = tenm_load_image(8,
                                   IMAGEDIR "/fear.png", 1, 1,
                                   tenm_map_color(255, 255, 255),
                                   IMAGEDIR "/uncertainty.png", 1, 1,
                                   tenm_map_color(255, 255, 255),
                                   IMAGEDIR "/doubt.png", 1, 1,
                                   tenm_map_color(255, 255, 255),
                                   IMAGEDIR "/trinity.png", 1, 1,
                                   tenm_map_color(255, 255, 255),
                                   IMAGEDIR "/fear-damaged.png", 1, 1,
                                   tenm_map_color(255, 255, 255),
                                   IMAGEDIR "/uncertainty-damaged.png", 1, 1,
                                   tenm_map_color(255, 255, 255),
                                   IMAGEDIR "/doubt-damaged.png", 1, 1,
                                   tenm_map_color(255, 255, 255),
                                   IMAGEDIR "/trinity-damaged.png", 1, 1,
                                   tenm_map_color(255, 255, 255));
  if (fud_core_image == NULL)
  {
    fprintf(stderr, "image_init: tenm_load_image(fud_core_image) failed\n");
    status = 1;
  }

  intuition_image = tenm_load_image(2,
                                    IMAGEDIR "/intuition.png", 1, 1,
                                    tenm_map_color(255, 255, 255),
                                    IMAGEDIR "/intuition-damaged.png", 1, 1,
                                    tenm_map_color(255, 255, 255));

  if (intuition_image == NULL)
  {
    fprintf(stderr, "image_init: tenm_load_image(intuition_image) "
            "failed\n");
    status = 1;
  }

  last_boss_image = tenm_load_image(2,
                                    IMAGEDIR "/last-boss.png", 1, 1,
                                    tenm_map_color(255, 255, 255),
                                    IMAGEDIR "/last-boss-damaged.png", 1, 1,
                                    tenm_map_color(255, 255, 255));
  if (last_boss_image == NULL)
  {
    fprintf(stderr, "image_init: tenm_load_image(last_boss_image) failed\n");
    status = 1;
  }

  last_boss_demo_image = tenm_load_image(4,
                                         IMAGEDIR "/last-boss-demo-0.png",1, 1,
                                         tenm_map_color(255, 255, 255),
                                         IMAGEDIR "/last-boss-demo-1.png",1, 1,
                                         tenm_map_color(255, 255, 255),
                                         IMAGEDIR "/last-boss-demo-2.png",1, 1,
                                         tenm_map_color(255, 255, 255),
                                         IMAGEDIR "/last-boss-demo-3.png",1, 1,
                                         tenm_map_color(255, 255, 255));

  if (last_boss_demo_image == NULL)
  {
    fprintf(stderr, "image_init: tenm_load_image(last_boss_demo_image) "
            "failed\n");
    status = 1;
  }

  midnight_image = tenm_load_image(2,
                                   IMAGEDIR "/midnight.png", 1, 1,
                                   tenm_map_color(255, 255, 255),
                                   IMAGEDIR "/midnight-damaged.png", 1, 1,
                                   tenm_map_color(255, 255, 255));
  if (midnight_image == NULL)
  {
    fprintf(stderr, "image_init: tenm_load_image(midnight_image) failed\n");
    status = 1;
  }

  /* this blue-red-purple order is intentional --- see midnight.c */
  midnight_bit_image = tenm_load_image(6,
                                       IMAGEDIR "/midnight-bit-blue.png",
                                       1, 1,
                                       tenm_map_color(255, 255, 255),
                                       IMAGEDIR "/midnight-bit-red.png",
                                       1, 1,
                                       tenm_map_color(255, 255, 255),
                                       IMAGEDIR "/midnight-bit-purple.png",
                                       1, 1,
                                       tenm_map_color(255, 255, 255),
                                       IMAGEDIR
                                       "/midnight-bit-blue-damaged.png",
                                       1, 1,
                                       tenm_map_color(255, 255, 255),
                                       IMAGEDIR
                                       "/midnight-bit-red-damaged.png",
                                       1, 1,
                                       tenm_map_color(255, 255, 255),
                                       IMAGEDIR
                                       "/midnight-bit-purple-damaged.png",
                                       1, 1,
                                       tenm_map_color(255, 255, 255));

  if (midnight_bit_image == NULL)
  {
    fprintf(stderr, "image_init: tenm_load_image(midnight_bit_image) "
            "failed\n");
    status = 1;
  }

  overrun_image = tenm_load_image(2,
                                  IMAGEDIR "/overrun.png", 1, 1,
                                  tenm_map_color(255, 255, 255),
                                  IMAGEDIR "/overrun-damaged.png", 1, 1,
                                  tenm_map_color(255, 255, 255));
  if (overrun_image == NULL)
  {
    fprintf(stderr, "image_init: tenm_load_image(overrun_image) failed\n");
    status = 1;
  }

  overrun_bit_image = tenm_load_image(7,
                                      IMAGEDIR "/overrun-bit-blue.png", 1, 1,
                                      tenm_map_color(255, 255, 255),
                                      IMAGEDIR "/overrun-bit-purple.png", 1, 1,
                                      tenm_map_color(255, 255, 255),
                                      IMAGEDIR "/overrun-bit-red.png", 1, 1,
                                      tenm_map_color(255, 255, 255),
                                      IMAGEDIR "/overrun-bit-green.png", 1, 1,
                                      tenm_map_color(255, 255, 255),
                                      IMAGEDIR
                                      "/overrun-bit-blue-damaged.png", 1, 1,
                                      tenm_map_color(255, 255, 255),
                                      IMAGEDIR
                                      "/overrun-bit-purple-damaged.png", 1, 1,
                                      tenm_map_color(255, 255, 255),
                                      IMAGEDIR
                                      "/overrun-bit-red-damaged.png", 1, 1,
                                      tenm_map_color(255, 255, 255));

  if (overrun_bit_image == NULL)
  {
    fprintf(stderr, "image_init: tenm_load_image(overrun_bit_image) "
            "failed\n");
    status = 1;
  }

  solution_image = tenm_load_image(2,
                                   IMAGEDIR "/solution.png", 1, 1,
                                   tenm_map_color(255, 255, 255),
                                   IMAGEDIR "/solution-damaged.png", 1, 1,
                                   tenm_map_color(255, 255, 255));
  if (solution_image == NULL)
  {
    fprintf(stderr, "image_init: tenm_load_image(solution_image) failed\n");
    status = 1;
  }

  /* Don't call image_quit() here even if status is non-zero.
   * The idea is to load as many image files as possible.
   */
  return status;
}

void
image_quit(void)
{
  /* these NULL checks are mandatory */
  if (player_image != NULL)
  {
    tenm_image_delete(player_image);
    player_image = NULL;
  }
  if (small_ball_image != NULL)
  {
    tenm_image_delete(small_ball_image);
    small_ball_image = NULL;
  }
  if (medium_ball_image != NULL)
  {
    tenm_image_delete(medium_ball_image);
    medium_ball_image = NULL;
  }
  if (large_ball_image != NULL)
  {
    tenm_image_delete(large_ball_image);
    large_ball_image = NULL;
  }
  if (sting_image != NULL)
  {
    tenm_image_delete(sting_image);
    sting_image = NULL;
  }
  if (horizon_image != NULL)
  {
    tenm_image_delete(horizon_image);
    horizon_image = NULL;
  }
  if (plenty_image != NULL)
  {
    tenm_image_delete(plenty_image);
    plenty_image = NULL;
  }
  if (burst_image != NULL)
  {
    tenm_image_delete(burst_image);
    burst_image = NULL;
  }
  if (disconnection_core_image != NULL)
  {
    tenm_image_delete(disconnection_core_image);
    disconnection_core_image = NULL;
  }
  if (disconnection_bit_image != NULL)
  {
    tenm_image_delete(disconnection_bit_image);
    disconnection_bit_image = NULL;
  }
  if (fud_core_image != NULL)
  {
    tenm_image_delete(fud_core_image);
    fud_core_image = NULL;
  }
  if (intuition_image != NULL)
  {
    tenm_image_delete(intuition_image);
    intuition_image = NULL;
  }
  if (last_boss_image != NULL)
  {
    tenm_image_delete(last_boss_image);
    last_boss_image = NULL;
  }
  if (last_boss_demo_image != NULL)
  {
    tenm_image_delete(last_boss_demo_image);
    last_boss_demo_image = NULL;
  }
  if (midnight_image != NULL)
  {
    tenm_image_delete(midnight_image);
    midnight_image = NULL;
  }
  if (midnight_bit_image != NULL)
  {
    tenm_image_delete(midnight_bit_image);
    midnight_bit_image = NULL;
  }
  if (overrun_image != NULL)
  {
    tenm_image_delete(overrun_image);
    overrun_image = NULL;
  }
  if (overrun_bit_image != NULL)
  {
    tenm_image_delete(overrun_bit_image);
    overrun_bit_image = NULL;
  }
  if (solution_image != NULL)
  {
    tenm_image_delete(solution_image);
    solution_image = NULL;
  }
}

int
player_blit(int x, int y, int n)
{
  /* sanity check */
  if ((n < 0) || (n >= 3))
    return 0;
  if (player_image == NULL)
    return 0;

  return tenm_draw_image(x - 30, y - 30, player_image, n);
}

int
small_ball_blit(int x, int y, int n)
{
  /* sanity check */
  if ((n < 0) || (n >= 3))
    return 0;
  if (small_ball_image == NULL)
    return 0;

  return tenm_draw_image(x - 15, y - 15, small_ball_image, n);
}

int
medium_ball_blit(int x, int y, int n)
{
  /* sanity check */
  if ((n < 0) || (n >= 3))
    return 0;
  if (medium_ball_image == NULL)
    return 0;

  return tenm_draw_image(x - 20, y - 20, medium_ball_image, n);
}

int
large_ball_blit(int x, int y, int n)
{
  /* sanity check */
  if ((n < 0) || (n >= 3))
    return 0;
  if (large_ball_image == NULL)
    return 0;

  return tenm_draw_image(x - 25, y - 25, large_ball_image, n);
}

int
sting_blit(int x, int y, int n)
{
  /* sanity check */
  if ((n < 0) || (n >= 6))
    return 0;
  if (sting_image == NULL)
    return 0;

  return tenm_draw_image(x - 32, y - 24, sting_image, n);
}

int
horizon_blit(int x, int y, int n)
{
  /* sanity check */
  if ((n < 0) || (n >= 6))
    return 0;
  if (horizon_image == NULL)
    return 0;

  return tenm_draw_image(x - 25, y - 25, horizon_image, n);
}

int
plenty_blit(int x, int y, int n)
{
  /* sanity check */
  if ((n < 0) || (n >= 6))
    return 0;
  if (plenty_image == NULL)
    return 0;

  return tenm_draw_image(x - 36, y - 48, plenty_image, n);
}

int
burst_blit(int x, int y, int n)
{
  /* sanity check */
  if ((n < 0) || (n >= 6))
    return 0;
  if (burst_image == NULL)
    return 0;

  return tenm_draw_image(x - 24, y - 32, burst_image, n);
}

int
disconnection_core_blit(int x, int y, int n)
{
  /* sanity check */
  if ((n < 0) || (n >= 2))
    return 0;
  if (disconnection_core_image == NULL)
    return 0;

  return tenm_draw_image(x - 35, y - 35, disconnection_core_image, n);
}

int
disconnection_bit_blit(int x, int y, int n)
{
  /* sanity check */
  if ((n < 0) || (n >= 3))
    return 0;
  if (disconnection_bit_image == NULL)
    return 0;

  return tenm_draw_image(x - 25, y - 25, disconnection_bit_image, n);
}

int
fud_core_blit(int x, int y, int n)
{
  /* sanity check */
  if ((n < 0) || (n >= 8))
    return 0;
  if (fud_core_image == NULL)
    return 0;

  return tenm_draw_image(x - 25, y - 25, fud_core_image, n);
}

int
intuition_blit(int x, int y, int n)
{
  /* sanity check */
  if ((n < 0) || (n >= 2))
    return 0;
  if (intuition_image == NULL)
    return 0;

  return tenm_draw_image(x - 50, y - 43, intuition_image, n);
}

int
last_boss_blit(int x, int y, int n)
{
  /* sanity check */
  if ((n < 0) || (n >= 2))
    return 0;
  if (last_boss_image == NULL)
    return 0;

  return tenm_draw_image(x - 60, y - 60, last_boss_image, n);
}

int
last_boss_demo_blit(int x, int y, int n)
{
  /* sanity check */
  if ((n < 0) || (n >= 4))
    return 0;
  if (last_boss_demo_image == NULL)
    return 0;

  return tenm_draw_image(x - 60, y - 60, last_boss_demo_image, n);
}

int
midnight_blit(int x, int y, int n)
{
  /* sanity check */
  if ((n < 0) || (n >= 2))
    return 0;
  if (midnight_image == NULL)
    return 0;

  return tenm_draw_image(x - 64, y - 48, midnight_image, n);
}

int
midnight_bit_blit(int x, int y, int n)
{
  /* sanity check */
  if ((n < 0) || (n >= 6))
    return 0;
  if (midnight_bit_image == NULL)
    return 0;

  return tenm_draw_image(x - 32, y - 10, midnight_bit_image, n);
}

int
overrun_blit(int x, int y, int n)
{
  /* sanity check */
  if ((n < 0) || (n >= 2))
    return 0;
  if (overrun_image == NULL)
    return 0;

  return tenm_draw_image(x - 50, y - 50, overrun_image, n);
}

int
overrun_bit_blit(int x, int y, int n)
{
  /* sanity check */
  if ((n < 0) || (n >= 7))
    return 0;
  if (overrun_bit_image == NULL)
    return 0;

  return tenm_draw_image(x - 25, y - 25, overrun_bit_image, n);
}

int
solution_blit(int x, int y, int n)
{
  /* sanity check */
  if ((n < 0) || (n >= 2))
    return 0;
  if (solution_image == NULL)
    return 0;

  return tenm_draw_image(x - 48, y - 36, solution_image, n);
}
