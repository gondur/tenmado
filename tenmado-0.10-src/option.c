/* $Id: option.c,v 1.49 2003/01/11 22:06:26 oohara Exp $ */

/* VERSION */
#include <config.h>

#include <stdio.h>
/* strcmp */
#include <string.h>
/* strtol, malloc, atexit */
#include <stdlib.h>
/* errno */
#include <errno.h>
/* INT_MIN, INT_MAX */
#include <limits.h>

/* COPYRIGHT_STRING */
#include "const.h"

#include "option.h"

static option *option_pointer = NULL;

static int convert_to_number(int *value, const char *string, int min,
                             int max);
static void option_quit(void);

/* return 0 on success, 1 on error */
int
set_option(int argc, char *argv[])
{
  int i;
  int action;

  /* sanity check */
  if (argc <= 0)
  {
    fprintf(stderr, "set_option: argc is non-positive\n");
    return 1;
  }
  if (argv == NULL)
  {
    fprintf(stderr, "set_option: argv is NULL\n");
    return 1;
  }

  if (option_pointer != NULL)
    free(option_pointer);
  option_pointer = (option *) malloc(sizeof(option));
  if (option_pointer == NULL)
  {
    fprintf(stderr, "set_option: malloc failed\n");
    return 1;
  }

  if (atexit(option_quit) != 0)
  {
    fprintf(stderr, "set_option: cannot register option_quit to exit\n");
    option_quit();
    return 1;
  }

  option_pointer->dump = -1;
  option_pointer->full_screen = 0;
  option_pointer->help = 0;
  option_pointer->slow = 0;
  option_pointer->stage = 0;
  option_pointer->undump = -1;
  option_pointer->undump_default = 0;
  option_pointer->version = 0;

  for (i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "--dump") == 0)
    {
      if (i + 1 >= argc)
      {
        fprintf(stderr, "set_option: missing stage number for --dump "
                "(arg %d)\n", i);
        return 1;
      }
      if (convert_to_number(&(option_pointer->dump), argv[i + 1], 0, 5) != 0)
      {
        fprintf(stderr, "set_option: convert_to_number(dump) (arg %d) "
                "failed\n", i + 1);
        return 1;
      }
      i++;
    }
    else if (strcmp(argv[i], "--full-screen") == 0)
    {
      option_pointer->full_screen = 1;
    }
    else if (strcmp(argv[i], "--help") == 0)
    {
      option_pointer->help = 1;
      /* ignore the rest */
      break;
    }
    else if (strcmp(argv[i], "--slow") == 0)
    {
      option_pointer->slow = 1;
    }
    else if (strcmp(argv[i], "--stage") == 0)
    {
      if (i + 1 >= argc)
      {
        fprintf(stderr, "set_option: missing stage number for --stage "
                "(arg %d)\n", i);
        return 1;
      }
      if (convert_to_number(&(option_pointer->stage), argv[i + 1], 1, 5) != 0)
      {
        fprintf(stderr, "set_option: convert_to_number(stage) (arg %d) "
                "failed\n", i + 1);
        return 1;
      }
      i++;
    }
    else if (strcmp(argv[i], "--undump") == 0)
    {
      if (i + 1 >= argc)
      {
        fprintf(stderr, "set_option: missing stage number for --undump "
                "(arg %d)\n", i);
        return 1;
      }
      if (convert_to_number(&(option_pointer->undump), argv[i + 1], 0, 5) != 0)
      {
        fprintf(stderr, "set_option: convert_to_number(undump) (arg %d) "
                "failed\n", i + 1);
        return 1;
      }
      i++;
    }
    else if (strcmp(argv[i], "--undump-default") == 0)
    {
      option_pointer->undump_default = 1;
    }
    else if (strcmp(argv[i], "--version") == 0)
    {
      option_pointer->version = 1;
      /* ignore the rest */
      break;
    }
    else
    {
      fprintf(stderr, "set_option: unknown option (arg %d)\n", i);
      return 1;
    }
  }

  if ((option_pointer->help == 0) && (option_pointer->version == 0))
  {
    /* we know there are at most 3 actions, so an overflow shouldn't happen */
    action = 0;
    if (option_pointer->dump != -1)
      action++;
    if (option_pointer->undump != -1)
      action++;
    if (option_pointer->undump_default != 0)
      action++;

    if (action > 1)
    {
      fprintf(stderr, "set_option: only one of --dump, --undump or "
              "--undump-defaultmay be specified\n");
      return 1;
    }
  }

  return 0;
}

void
do_help(void)
{
  printf("Usage: tenmado [options]\n"
         "hard-core shoot 'em up game in blue-or-red world\n"
         "Options:\n"
         );
  printf("  --dump _stage_    write high score data to stdout\n"
         "  --full-screen     run the game in the full screen mode\n"
         "  --help            print this message\n"
         );
  printf("  --slow            run the game at the half speed\n"
         "  --stage _stage_   start the game at the specified stage\n"
         "  --undump _stage_  read high score data from stdin and save\n"
         "  --undump-default  fill the high score lists if they are empty\n"
         );
  printf("  --version         print version information\n"
         "\n"
         "Report bugs to <oohara@libra.interq.or.jp>.\n"
         );
}

void
do_version(void)
{
  printf("tenmado %s\n", VERSION);
  printf("%s\n", COPYRIGHT_STRING);
  printf("This program is free software; you can redistribute it and/or\n"
         "modify it under the terms of either the GNU General Public License\n"
         "version 2 or the Artistic License Version 2.0beta5.\n");
  printf("tenmado comes with NO WARRANTY, to the extent permitted by law.\n");
}

const option *
get_option(void)
{
  return (const option *) option_pointer;
}

/* convert string (arg 2) to a number and store it to value (arg 1)
 * return 0 on success, 1 on error
 */
static int
convert_to_number(int *value, const char *string, int min, int max)
{
  long int temp;
  char *tail;

  /* sanity check */
  if (min > max)
  {
    fprintf(stderr, "convert_to_number: min > max\n");
    return 1;
  }

  /* to detect overflow */
  errno = 0;
  temp = strtol(string, &tail, 10);
  if (tail[0] != '\0')
  {
    fprintf(stderr, "convert_to_number: string is not a number\n");
    return 1;
  }
  if (errno != 0)
  {
    fprintf(stderr, "convert_to_number: number overflowed\n");
    return 1;
  }
  if ((temp <= INT_MIN) || (temp >= INT_MAX))
  {
    fprintf(stderr, "convert_to_number: too big or small integer\n");
    return 1;
  }
  if ((((int) temp) < min) || (((int) temp) > max))
  {
    fprintf(stderr, "convert_to_number: integer %d -- %d is expected\n",
            min, max);
    return 1;
  }
  *value = (int) temp;

  return 0;
}

static void
option_quit(void)
{
  if (option_pointer != NULL)
  {
    free(option_pointer);
    option_pointer = NULL;
  }
}
