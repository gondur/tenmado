/* $Id: tenm_table.c,v 1.56 2003/08/31 12:37:58 oohara Exp $ */

#include <stdio.h>
/* malloc, atexit */
#include <stdlib.h>
/* errno */
#include <errno.h>
/* strerror */
#include <string.h>

#include "tenm_object.h"

#include "tenm_table.h"

static int table_size = 0;
static int table_draw_min_priority = 0;
static int table_draw_max_priority = -1;
static tenm_object **object_table = NULL;
static int *action_needed = NULL;

static void tenm_table_quit(void);

/* return 0 on success, 1 on error */
int
tenm_table_init(int size, int draw_min_priority, int draw_max_priority)
{
  int i;

  /* sanity check */
  if (draw_min_priority > draw_max_priority)
  {
    fprintf(stderr, "tenm_table_init: draw_min_priority > "
            "draw_max_priority\n");
    /* this is a fatal error */
    return 1;
  }

  if (object_table != NULL)
  {
    fprintf(stderr, "tenm_table_init: warning: object_table is not NULL,"
            "freeing\n");
    tenm_table_clear_all();
    free(object_table);
    object_table = NULL;
  }
  object_table = (tenm_object **) malloc(sizeof(tenm_object *) * size);
  if (object_table == NULL)
  {
    fprintf(stderr, "tenm_table_init: malloc(object_table) failed\n");
    tenm_table_quit();
    return 1;
  }

  if (action_needed != NULL)
  {
    fprintf(stderr, "tenm_table_init: warning: action_needed is not NULL,"
            "freeing\n");
    free(action_needed);
    action_needed = NULL;
  }
  action_needed = (int *) malloc(sizeof(int) * size);
  if (action_needed == NULL)
  {
    fprintf(stderr, "tenm_table_init: malloc(action_needed) failed\n");
    tenm_table_quit();
    return 1;
  }

  errno = 0;
  if (atexit(tenm_table_quit) != 0)
  {
    fprintf(stderr, "tenm_table_init: cannot register tenm_table_quit "
            "to exit");
    if (errno != 0)
      fprintf(stderr, " (%s)", strerror(errno));
    fprintf(stderr, "\n");
    tenm_table_quit();
    return 1;
  }

  table_size = size;
  for (i = 0; i < size; i++)
  {
    object_table[i] = NULL;
    action_needed[i] = 1;
  }

  table_draw_min_priority = draw_min_priority;
  table_draw_max_priority = draw_max_priority;
  return 0;
}

/* return the index of the table on success, -1 on error
 */
int
tenm_table_add(tenm_object *object)
{
  int i;
  /* sanity check */
  if (object == NULL)
  {
    fprintf(stderr, "tenm_table_add: object is NULL\n");
    return -1;
  }
  if ((object_table == NULL) || (action_needed == NULL))
  {
    fprintf(stderr, "tenm_table_add: table is not initialized\n");
    return -1;
  }
  
  for (i = 0; i < table_size; i++)
    if (object_table[i] == NULL)
    {
      object->table_index = i;
      object_table[i] = object;
      action_needed[i] = 0;
      return i;
    }
  fprintf(stderr, "tenm_table_add: no more space\n");
  return -1;
}

void
tenm_table_delete(int i)
{
  if (object_table == NULL)
    return;
  if (object_table[i] == NULL)
    return;
  if (action_needed == NULL)
    return;

  tenm_object_delete(object_table[i]);
  object_table[i] = NULL;
  action_needed[i] = 1;
}

void
tenm_table_clear_all(void)
{
  int i;

  if (object_table == NULL)
    return;

  for (i = 0; i < table_size; i++)
    if (object_table[i] != NULL)
      tenm_table_delete(i);
}

/* return 1 if player is deleted, 0 if not */
int
tenm_table_move(tenm_object *player, int turn_per_frame)
{
  int i;
  double tpf = (double) turn_per_frame;

  /* sanity check */
  if ((object_table == NULL) || (action_needed == NULL))
  {
    fprintf(stderr, "tenm_table_move: table is not initialized\n");
    return 0;
  }
  if (turn_per_frame < 1)
  {    
    fprintf(stderr, "tenm_table_move: strange turn_per_frame (%d)\n",
            turn_per_frame);
    return 0;
  }

  for (i = 0; i < table_size; i++)
  {
    if (object_table[i] == NULL)
      continue;
    if (object_table[i]->move == NULL)
      continue;
    if ((*(object_table[i]->move))(object_table[i], tpf))
      tenm_table_delete(i);
  }
  if (player == NULL)
    return 0;
  if (player->move == NULL)
    return 0;
  if ((*(player->move))(player, tpf))
  {
    tenm_object_delete(player);
    /* note that this "player" is a local variable */
    player = NULL;
    return 1;
  }
  return 0;
}


/* return 1 if player is deleted, 0 if not */
int
tenm_table_detect_collision(tenm_object *player)
{
  int i;
  int j;
  int i_need_delete = 0;
  int j_need_delete = 0;
  int status = 0;

  /* sanity check */
  if ((object_table == NULL) || (action_needed == NULL))
  {
    fprintf(stderr, "tenm_table_detect_collision: table is not initialized\n");
    return 0;
  }

  /* collision between objects */
  for (i = 0; i < table_size; i++)
    for (j = i + 1; j < table_size; j++)
    {
      if (object_table[i] == NULL)
        continue;
      if (object_table[j] == NULL)
        continue;
      /* no need to detect collision if there is nothing to do when
       * they collide
       */
      if ((!(object_table[i]->hit_mask & object_table[j]->attr))
          && (!(object_table[j]->hit_mask & object_table[i]->attr)))
        continue;
      if ((object_table[i]->hit == NULL) && (object_table[j]->hit == NULL))
        continue;
      if (object_table[i]->mass == NULL)
        continue;
      if (object_table[j]->mass == NULL)
        continue;

      if (! tenm_collided_mass(object_table[i]->mass, object_table[j]->mass))
        continue;

      i_need_delete = 0;
      j_need_delete = 0;
      if ((object_table[i] != NULL) && (object_table[j] != NULL)
          && (object_table[i]->hit_mask & object_table[j]->attr))
        if ((object_table[i]->hit != NULL)
            && ((*(object_table[i]->hit))(object_table[i],
                                          object_table[j]) != 0))
          i_need_delete = 1;
      /* we have to check if the objects are not NULL because
       * hit() of object_table[i] may delete object_table[j]
       * via tenm_table_apply()
       */
      if ((object_table[i] != NULL) && (object_table[j] != NULL)
          && (object_table[j]->hit_mask & object_table[i]->attr))
        if ((object_table[j]->hit != NULL)
            && ((*(object_table[j]->hit))(object_table[j],
                                          object_table[i]) != 0))
          j_need_delete = 1;

      if (i_need_delete)
        tenm_table_delete(i);
      if (j_need_delete)
        tenm_table_delete(j);
    }
  
  /* collison between player and object */
  for (i = 0; i < table_size; i++)
  {
    if (object_table[i] == NULL)
      continue;
    if (player == NULL)
      continue;
    /* no need to detect collision if there is nothing to do when
     * they collide
     */
    if ((!(object_table[i]->hit_mask & player->attr))
        && (!(player->hit_mask & object_table[i]->attr)))
      continue;
    if ((object_table[i]->hit == NULL) && (player->hit == NULL))
      continue;
    if (object_table[i]->mass == NULL)
      continue;
    if (player->mass == NULL)
      continue;

    if (! tenm_collided_mass(object_table[i]->mass, player->mass))
      continue;

    i_need_delete = 0;
    j_need_delete = 0;
    if ((object_table[i] != NULL) && (player != NULL)
        && (object_table[i]->hit_mask & player->attr))
      if ((object_table[i]->hit != NULL)
          && ((*(object_table[i]->hit))(object_table[i], player) != 0))
        i_need_delete = 1;
    if ((object_table[i] != NULL) && (player != NULL)
        && (player->hit_mask & object_table[i]->attr))
      if ((player->hit != NULL)
          && ((*(player->hit))(player, object_table[i]) != 0))
        j_need_delete = 1;

    if (i_need_delete)
      tenm_table_delete(i);
    if (j_need_delete)
    {
      tenm_object_delete(player);
      /* note that this "player" is a local variable */
      player = NULL;
      status = 1;
    }
  }

  return status;
}

/* return 1 if player is deleted, 0 if not */
int
tenm_table_do_action(tenm_object *player)
{
  int i;

  /* sanity check */
  if ((object_table == NULL) || (action_needed == NULL))
  {
    fprintf(stderr, "tenm_table_do_action: table is not initialized\n");
    return 0;
  }

  for (i = 0; i < table_size; i++)
  {
    if (object_table[i] == NULL)
      continue;
    if (object_table[i]->act == NULL)
      continue;
    if (action_needed[i] == 0)
      continue;
    if ((*(object_table[i]->act))(object_table[i], player) != 0)
      tenm_table_delete(i);
  }
  if (player == NULL)
    return 0;
  if (player->act == NULL)
    return 0;
  if ((*(player->act))(player, NULL) != 0)
  {
    tenm_object_delete(player);
    /* note that this "player" is a local variable */
    player = NULL;
    return 1;
  }

  for (i = 0; i < table_size; i++)
    action_needed[i] = 1;

  return 0;
}

/* return 0 on success, 1 on error */
int
tenm_table_draw(tenm_object *player)
{
  int i;
  int j;
  int status = 0;

  /* sanity check */
  if ((object_table == NULL) || (action_needed == NULL))
  {
    fprintf(stderr, "tenm_table_draw: table is not initialized\n");
    return 0;
  }

  for (i = table_draw_min_priority; i <= table_draw_max_priority; i++)
  {
    if ((player != NULL) && (player->draw != NULL))
    {
      if ((*(player->draw))(player, i) != 0)
        status = 1;
    }

    for (j = 0; j < table_size; j++)
    {
      if (object_table[j] == NULL)
        continue;
      if (object_table[j]->draw == NULL)
        continue;
      if ((*(object_table[j]->draw))(object_table[j], i) != 0)
        status = 1;
    }
  }

  return status;
}

void
tenm_table_apply(int i, int (*func)(tenm_object *, int), int n)
{
  /* sanity check */
  if ((object_table == NULL) || (action_needed == NULL))
  {
    fprintf(stderr, "tenm_table_apply: table is not initialized\n");
    return;
  }
  if (func == NULL)
  {
    fprintf(stderr, "tenm_table_apply: func is NULL\n");
    return;
  }
  if ((i < 0) || (i >= table_size))
  {
    fprintf(stderr, "tenm_table_apply: i (%d) is out of range (0 -- %d)\n",
            i, table_size - 1);
    return;
  }

  if (object_table[i] == NULL)
    return;
  if ((*func)(object_table[i], n) != 0)
    tenm_table_delete(i);
}

void
tenm_table_apply_all(int (*func)(tenm_object *, int), int n)
{
  int i;

  /* sanity check */
  if ((object_table == NULL) || (action_needed == NULL))
  {
    fprintf(stderr, "tenm_table_apply_all: table is not initialized\n");
    return;
  }
  if (func == NULL)
  {
    fprintf(stderr, "tenm_table_apply_all: func is NULL\n");
    return;
  }

  for (i = 0; i < table_size; i++)
    tenm_table_apply(i, (int (*)(tenm_object *, int)) func, n);
}

static void
tenm_table_quit(void)
{
  table_size = 0;
  table_draw_min_priority = 0;
  table_draw_max_priority = -1;

  if (object_table != NULL)
  {
    tenm_table_clear_all();
    free(object_table);
    object_table = NULL;
  }
  if (action_needed != NULL)
  {
    free(action_needed);
    action_needed = NULL;
  }
}
