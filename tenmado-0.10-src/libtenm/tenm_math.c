/* $Id: tenm_math.c,v 1.24 2003/01/14 03:03:58 oohara Exp $ */

#include <stdio.h>
/* malloc, atexit */
#include <stdlib.h>
#include <math.h>
/* errno */
#include <errno.h>
/* strerror */
#include <string.h>

#include "tenm_math.h"

/* L said, "Easy, it's 3.1415926... [2.38M digits followed]" */
static double pi = 3.1415;
static double *sin_table = NULL;
static double *cos_table = NULL;
static double *sqrt_table = NULL;
static int sqrt_table_size = 0;
static int *pow2_table = NULL;
static int pow2_table_size = 0;

static void tenm_math_quit(void);

/* return 0 on success, 1 on error */
int
tenm_math_init(int sqrt_size, int pow2_size)
{
  int i;
  int temp = 1;
  pi = acos(-1.0);

  /* sanity check */
  if (sqrt_size <= 0)
  {
    fprintf(stderr, "tenm_math_init: sqrt_size is non-positive\n");
    return 1;
  }
  if (pow2_size <= 0)
  {
    fprintf(stderr, "tenm_math_init: pow2_size is non-positive\n");
    return 1;
  }

  if (sin_table != NULL)
  {
    fprintf(stderr, "tenm_math_init: warning: sin_table is not NULL,"
            "freeing\n");
    free(sin_table);
    sin_table = NULL;
  }
  sin_table = (double *) malloc(sizeof(double) * 360);
  if (sin_table == NULL)
  {
    fprintf(stderr, "tenm_math_init: malloc(sin_table) failed\n");
    tenm_math_quit();
    return 1;
  }
  sin_table[0] = 0.0;
  sin_table[90] = 1.0;
  sin_table[180] = 0.0;
  sin_table[270] = -1.0;

  if (cos_table != NULL)
  {
    fprintf(stderr, "tenm_math_init: warning: cos_table is not NULL,"
            "freeing\n");
    free(cos_table);
    cos_table = NULL;
  }
  cos_table = (double *) malloc(sizeof(double) * 360);
  if (cos_table == NULL)
  {
    fprintf(stderr, "tenm_math_init: malloc(cos_table) failed\n");
    tenm_math_quit();
    return 1;
  }
  cos_table[0] = 1.0;
  cos_table[90] = 0.0;
  cos_table[180] = -1.0;
  cos_table[270] = 0.0;

  if (sqrt_table != NULL)
  {
    fprintf(stderr, "tenm_math_init: warning: sqrt_table is not NULL,"
            "freeing\n");
    free(sqrt_table);
    sqrt_table = NULL;
  }
  sqrt_table = (double *) malloc(sizeof(double) * sqrt_size);
  if (sqrt_table == NULL)
  {
    fprintf(stderr, "tenm_math_init: malloc(sqrt_table) failed\n");
    tenm_math_quit();
    return 1;
  }

  if (pow2_table != NULL)
  {
    fprintf(stderr, "tenm_math_init: warning: pow2_table is not NULL,"
            "freeing\n");
    free(pow2_table);
    pow2_table = NULL;
  }
  pow2_table = (int *) malloc(sizeof(int) * pow2_size);
  if (pow2_table == NULL)
  {
    fprintf(stderr, "tenm_math_init: malloc(pow2_table) failed\n");
    tenm_math_quit();
    return 1;
  }

  errno = 0;
  if (atexit(tenm_math_quit) != 0)
  {
    fprintf(stderr, "tenm_math_init: cannot register tenm_math_quit "
            " to exit");
    if (errno != 0)
      fprintf(stderr, " (%s)", strerror(errno));
    fprintf(stderr, "\n");
    tenm_math_quit();
    return 1;
  }

  for (i = 0; i < 360; i++)
    sin_table[i] = sin(2.0 * pi * ((double) i) / 360.0);
  for (i = 0; i < 360; i++)
    cos_table[i] = cos(2.0 * pi * ((double) i) / 360.0);
  sqrt_table_size = sqrt_size;
  for (i = 0; i < sqrt_size; i++)
    sqrt_table[i] = sqrt((double) i);
  temp = 1;
  pow2_table_size = pow2_size;
  for (i = 0; i < pow2_size; i++)
  {
    pow2_table[i] = temp;
    temp *= 2;
  }
  
  return 0;
}

double
tenm_sin(int theta)
{
  if (sin_table == NULL)
    return sin(2.0 * pi * ((double) theta) / 360.0);
  
  while (theta < 0)
    theta += 360;
  while (theta >= 360)
    theta -= 360;
  return sin_table[theta];
}

double
tenm_cos(int theta)
{
  if (cos_table == NULL)
    return cos(2.0 * pi * ((double) theta) / 360.0);
  
  while (theta < 0)
    theta += 360;
  while (theta >= 360)
    theta -= 360;
  return cos_table[theta];
}

double
tenm_sqrt(int n)
{
  if (n < 0)
    return 0.0;
  if (sqrt_table == NULL)
    return sqrt((double) n);
  if (n >= sqrt_table_size)
    return sqrt((double) n);
  return sqrt_table[n];
}

/* return 0 if n is negative */
int
tenm_pow2(int n)
{
  int temp = 1;
  if (n < 0)
    return 0;
  if ((pow2_table == NULL) || (n >= pow2_table_size))
  {
    for (; n > 0; n--)
      temp *= 2;
    return temp;  
  }
  return pow2_table[n];
}

static void
tenm_math_quit(void)
{
  pi = 3.1415;
  if (sin_table != NULL)
  {
    free(sin_table);
    sin_table = NULL;
  }
  if (cos_table != NULL)
  {
    free(cos_table);
    cos_table = NULL;
  }
  if (sqrt_table != NULL)
  {
    free(sqrt_table);
    sqrt_table = NULL;
  }
  sqrt_table_size = 0;
  if (pow2_table != NULL)
  {
    free(pow2_table);
    pow2_table = NULL;
  }
  pow2_table_size = 0;
}
