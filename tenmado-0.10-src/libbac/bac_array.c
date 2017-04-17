/* $Id: bac_array.c,v 1.24 2003/02/11 11:38:36 oohara Exp $ */

#include <stdio.h>
/* malloc */
#include <stdlib.h>

#include "bac_array.h"

void
bac_array_delete(bac_array *p)
{
  int i;

  /* sanity check */
  if (p == NULL)
  {
    fprintf(stderr, "bac_array_delete: p is NULL\n");
    return;
  }
  if (p->n <= 0)
  {
    fprintf(stderr, "bac_array_delete: p->n is non-positive (%d)\n", p->n);
    return;
  }
  if (p->p == NULL)
  {
    fprintf(stderr, "bac_array_delete: p->p is NULL\n");
    return;
  }

  for (i = 0; i < p->n; i++)
    if (p->p[i] != NULL)
      (*(p->p[i]->delete))(p->p[i]);
  free(p->p);
  free(p);
}

/* return the pointer to the new bac_array if successful,
 * NULL on error (in this case, p is left untouched)
 * if p is NULL, create a new bac_array which consists of
 * one entry (arg 2)
 * if successful, you must free p (return value) and must NOT
 * free entry (arg 2)
 */
bac_array *
bac_array_add_entry(bac_array *p, bac_entry *entry)
{
  bac_array *temp = NULL;
  bac_entry **p_temp = NULL;

  /* sanity check*/
  /* p == NULL is OK*/
  if (entry == NULL)
  {
    fprintf(stderr, "bac_array_add_entry: entry is NULL\n");
    return NULL;
  }

  if (p != NULL)
  {
    p_temp = (bac_entry **) realloc(p->p, sizeof(bac_entry *) * (p->n + 1));
    if (p_temp == NULL)
    {
      fprintf(stderr, "bac_array_add_entry: realloc failed\n");
      return NULL;
    }
    p->p = p_temp;
    p->p[p->n] = entry;
    (p->n)++;
    return p;
  }

  /* if we reached here, new array is needed */
  temp = (bac_array *) malloc(sizeof(bac_array));
  if (temp == NULL)
  {
    fprintf(stderr, "bac_array_add_entry: malloc(temp) failed\n");
    return NULL;
  }

  temp->p = (bac_entry **) malloc(sizeof(bac_entry *));
  if (temp->p == NULL)
  {
    fprintf(stderr, "bac_array_add_entry: malloc(temp->p) failed\n");
    free(temp);
    return NULL;
  }

  temp->p[0] = entry;
  temp->n = 1;

  return temp;
}

/* add the entries in array (arg 2) to p (arg 1)
 * return the pointer to the new bac_array if successful,
 * NULL on error (in this case, p is left untouched)
 * if p is NULL, simply return array (arg 2)
 * if p is not NULL and if successful, you must free p (return value)
 * and must NOT free array (arg 2)
 */
bac_array *
bac_array_add_array(bac_array *p, bac_array *array)
{
  bac_entry **p_temp = NULL;
  int i;

  /* sanity check*/
  /* p == NULL is OK*/
  if (array == NULL)
  {
    fprintf(stderr, "bac_array_add_array: array is NULL\n");
    return NULL;
  }
  if (array->n <= 0)
  {
    fprintf(stderr, "bac_array_add_array: array->n is non-positive (%d)\n",
            array->n);
    return NULL;
  }
  if (array->p == NULL)
  {
    fprintf(stderr, "bac_array_add_array: array->p is NULL\n");
    return NULL;
  }

  /* don't use bac_array_add_entry here --- we don't need partial success */
  if (p != NULL)
  {
    p_temp = (bac_entry **) realloc(p->p,
                                    sizeof(bac_entry *) * (p->n + array->n));
    if (p_temp == NULL)
    {
      fprintf(stderr, "bac_array_add_array: realloc failed\n");
      return NULL;
    }
    p->p = p_temp;
    for (i = 0; i < array->n; i++)
      p->p[p->n + i] = array->p[i];
    p->n += array->n;
    /* make sure array (arg 2) rests in peace */
    free(array->p);
    free(array);

    return p;
  }

  return array;
}
