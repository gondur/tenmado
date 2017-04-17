/* $Id: tenm_object.c,v 1.43 2003/02/11 11:39:35 oohara Exp $ */

#include <stdio.h>
/* malloc */
#include <stdlib.h>
/* strlen, strncpy */
#include <string.h>

#include "tenm_collision.h"
#include "tenm_object.h"

tenm_mass *
tenm_mass_new(int number_primitive, tenm_primitive **p)
{
  tenm_mass *temp;

  /* sanity check */
  if (number_primitive <= 0)
  {
    fprintf(stderr, "tenm_mass_new: number_primitive is non-positive (%d)\n",
            number_primitive);
    return NULL;
  }

  temp = (tenm_mass *) malloc(sizeof(tenm_mass));
  if (temp == NULL)
  {
    fprintf(stderr, "tenm_mass_new: malloc failed\n");
    return NULL;
  }

  temp->n = number_primitive;
  temp->p = p;
  
  return temp;
}

void
tenm_mass_delete(tenm_mass *p)
{
  int i;

  if (p == NULL)
  {
    fprintf(stderr, "tenm_mass_delete: p is NULL\n");
    return;
  }
  if (p->n <= 0)
  {
    fprintf(stderr, "tenm_mass_delete: p->n is non-positive (%d)\n", p->n);
    return;
  }
  if (p->p == NULL)
  {
    fprintf(stderr, "tenm_mass_delete: p->p is NULL\n");
    return;
  }

  for (i = 0; i < p->n; i++)
    if (p->p[i] != NULL)
      (*(p->p[i]->delete))(p->p[i]);
  free(p->p);
  free(p);
}

/* name (arg 1) is duplicated
 * count (arg 8), count_d (arg 10), p (arg 12) and the callback functions
 * (arg 13 -- 16) are passed directly --- you must NOT free them manually
 * (tenm_object_delete() does not free the callback functions)
 */
tenm_object *
tenm_object_new(const char *name, int attr, int hit_mask,
                int hit_point, double x, double y,
                int number_count, int *count,
                int number_count_d, double *count_d,
                int number_primitive, tenm_primitive **p,
                int (*move)(tenm_object *, double),
                int (*hit)(tenm_object *, tenm_object *),
                int (*act)(tenm_object *, const tenm_object *),
                int (*draw)(tenm_object *, int))
{
  tenm_object *temp;

  temp = (tenm_object *) malloc(sizeof(tenm_object));
  if (temp == NULL)
  {
    fprintf(stderr, "tenm_object_new: malloc(temp) failed\n");
    return NULL;
  }

  if (name != NULL)
  {
    /* +1 is for the trailing \0 */
    temp->name = (char *) malloc(sizeof(char) * (strlen(name) + 1));
    if (temp->name == NULL)
    {
      fprintf(stderr, "tenm_object_new: malloc(temp->name) failed\n");
      free(temp);
      return NULL;
    }
    strncpy(temp->name, name, strlen(name));
    temp->name[strlen(name)] = '\0';
  }

  temp->mass = NULL;
  if (number_primitive >= 1)
  {
    if (p == NULL)
    {
      fprintf(stderr, "tenm_object_new: p is NULL\n");
      if (name != NULL)
        free(temp->name);
      free(temp);
      return NULL;
    }
    temp->mass = tenm_mass_new(number_primitive, p);
    if (temp->mass == NULL)
    {
      fprintf(stderr, "tenm_object_new: tenm_mass_new failed\n");
      if (name != NULL)
        free(temp->name);
      free(temp);
      return NULL;
    }
  }
  
  temp->table_index = -1;
  /* temp->name is set above */
  temp->attr = attr;
  temp->hit_mask = hit_mask;
  temp->hit_point = hit_point;
  temp->x = x;
  temp->y = y;
  temp->n = number_count;
  temp->count = count;
  temp->n_d = number_count_d;
  temp->count_d = count_d;
  temp->move = (int (*)(void *, double)) move;
  temp->hit = (int (*)(void *, void *)) hit;
  temp->act = (int (*)(void *, const void *)) act;
  temp->draw = (int (*)(void *, int)) draw;

  return temp;
}

void
tenm_object_delete(tenm_object *p)
{
  if (p == NULL)
  {
    fprintf(stderr, "tenm_object_delete: p is NULL\n");
    return;
  }

  if (p->name != NULL)
    free(p->name);
  if (p->mass != NULL)
    tenm_mass_delete(p->mass);
  if (p->count_d != NULL)
    free(p->count_d);
  if (p->count != NULL)
    free(p->count);
  free(p);
}

/* return 1 (true) or 0 (false) */
int
tenm_collided_mass(tenm_mass *p, tenm_mass *q)
{
  int i;
  int j;

  /* sanity check */
  if (p == NULL)
  {
    fprintf(stderr, "tenm_collided_mass: p is NULL\n");
    return 0;
  }
  if (p->n <= 0)
  {
    fprintf(stderr, "tenm_collided_mass: p->n is non-positive\n");
    return 0;
  }
  if (p->p == NULL)
  {
    fprintf(stderr, "tenm_collided_mass: p->p is NULL\n");
    return 0;
  }
  if (q == NULL)
  {
    fprintf(stderr, "tenm_collided_mass: q is NULL\n");
    return 0;
  }
  if (q->n <= 0)
  {
    fprintf(stderr, "tenm_collided_mass: q->n is non-positive\n");
    return 0;
  }
  if (q->p == NULL)
  {
    fprintf(stderr, "tenm_collided_mass: q->p is NULL\n");
    return 0;
  }

  for (i = 0; i < p->n; i++)
    for (j = 0; j < q->n; j++)
      if (tenm_collided_primitive(p->p[i], q->p[j]))
        return 1;
  return 0;
}

/* return 0 on success, 1 on error */
int
tenm_move_mass(tenm_mass *p, double dx, double dy)
{
  int i;
  int status = 0;

  /* sanity check */
  if (p == NULL)
  {
    fprintf(stderr, "tenm_move_mass: p is NULL\n");
    return 1;
  }
  if (p->n <= 0)
  {
    fprintf(stderr, "tenm_move_mass: p->n is non-positive\n");
    return 1;
  }
  if (p->p == NULL)
  {
    fprintf(stderr, "tenm_move_mass: p->p is NULL\n");
    return 1;
  }

  for (i = 0; i < p->n; i++)
  {
    if (tenm_move_primitive(p->p[i], dx, dy) != 0)
    {
      fprintf(stderr, "tenm_move_mass: tenm_move_primitive(%d) failed\n", i);
      status = 1;
    }
  }
  
  return status;
}
