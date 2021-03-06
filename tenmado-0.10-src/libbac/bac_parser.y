/* $Id: bac_parser.y,v 1.39 2012/01/29 03:54:41 oohara Exp $ */
/* process this file with bison -d */

/* C declarations */
%{
#include <stdio.h>
/* malloc */
#include <stdlib.h>
/* time_t */
#include <time.h>
/* uid_t */
#include <sys/types.h>
/* INT_MIN, INT_MAX */
#include <limits.h>

#include "bac_array.h"
#include "bac_entry.h"
#include "bac_parser_public.h"
#include "bac_lexical.h"
/* this is generated by bison -d */
#include "bac_parser.h"

static bac_array *p = NULL;
static bac_array *p_temp = NULL;

static int entry_version_available = 0;
static int entry_version = 0;
static int stage_data_version_available = 0;
static int stage_data_version = 0;
static int when_available = 0;
static time_t when = 0;
static int uid_available = 0;
static uid_t uid = 0;
static int user_name_available = 0;
static char *user_name = NULL;
static int score_sort_available = 0;
static int score_sort = 0;
static int score_total_available = 0;
static int score_total = 0;
/* there is no number_stage_available --- you should check
 * if score_stage is not NULL instead */
static int number_stage = 0;
static int *score_stage = NULL;

static int silly_i;
 
static void bac_error(const char *s);
static void bac_parser_clear_entry(void);
static int bac_parser_check_entry(void);

%}

/* bison declatations */
%name-prefix="bac_"

%union{
  int val_int;
  long int val_long_int;
  char *val_string;
}

%token <val_int> NEWLINE
%token <val_int> TAB

%token <val_long_int> LONG_INT
%token <val_string> STRING

%token <val_int> BEGIN_ENTRY_VERSION
%token <val_int> BEGIN_COMMENT
%token <val_int> BEGIN_STAGE_DATA_VERSION
%token <val_int> BEGIN_WHEN
%token <val_int> BEGIN_TOTAL_SCORE
%token <val_int> BEGIN_UID
%token <val_int> BEGIN_USER_NAME
%token <val_int> BEGIN_SCORE_SORT
%token <val_int> BEGIN_SCORE_TOTAL
%token <val_int> BEGIN_NUMBER_STAGE
%token <val_int> BEGIN_SCORE_STAGE

%token <val_int> END_OF_ENTRY

%token <val_int> LEXICAL_ERROR

%type <val_int> input
%type <val_int> entry
/* note the plural */
%type <val_int> fields
%type <val_int> field

%type <val_int> entry_version_field
%type <val_int> comment_field
%type <val_int> stage_data_version_field
%type <val_int> when_field
%type <val_int> uid_field
%type <val_int> user_name_field
%type <val_int> score_sort_field
%type <val_int> score_total_field
%type <val_int> number_stage_field
%type <val_int> score_stage_field

%%
/* grammar rules */
input: /* empty */
       {
         bac_parser_clear_entry();
       }
     | input entry
       {
         bac_parser_clear_entry();
       }
;

entry: fields END_OF_ENTRY
       {
         if (bac_parser_check_entry() != 0)
         {
           bac_error("bac_parser_check_entry failed");
           YYABORT;
         }
         
         p_temp = bac_array_add_entry(p,
                                      bac_entry_new(0, stage_data_version,
                                                    when, uid,
                                                    user_name_available,
                                                    user_name, score_sort,
                                                    score_total, number_stage,
                                                    score_stage));
         if (p_temp == NULL)
         {
           bac_error("bac_array_add_entry failed");
           YYABORT;
         }
         p = p_temp;
       }
;

fields: field
      | fields field
;

field: entry_version_field
     | comment_field
     | stage_data_version_field
     | when_field
     | uid_field
     | user_name_field
     | score_sort_field
     | score_total_field
     | number_stage_field
     | score_stage_field
;

entry_version_field: BEGIN_ENTRY_VERSION TAB LONG_INT NEWLINE
            {
              if (($3 <= INT_MIN) || ($3 >= INT_MAX))
              {
                bac_error("too big or small integer");
                YYABORT;
              }
              entry_version = (int) $3;
              entry_version_available = 1;
            }
;

comment_field: BEGIN_COMMENT TAB STRING NEWLINE
            {
              /* ignore */;
            }
;

stage_data_version_field: BEGIN_STAGE_DATA_VERSION TAB LONG_INT NEWLINE
            {
              if (($3 <= INT_MIN) || ($3 >= INT_MAX))
              {
                bac_error("too big or small integer");
                YYABORT;
              }
              stage_data_version = (int) $3;
              stage_data_version_available = 1;
            }
;

when_field: BEGIN_WHEN TAB LONG_INT NEWLINE
            {
              when = (time_t) $3;
              when_available = 1;
            }
;

uid_field: BEGIN_UID TAB LONG_INT NEWLINE
           {
             uid = (uid_t) $3;
             uid_available = 1;
           }
;

user_name_field: BEGIN_USER_NAME TAB STRING NEWLINE
                 {
                   if (user_name != NULL)
                   {
                     free(user_name);
                     user_name = NULL;
                   }
                   user_name = $3;
                   user_name_available = 1;
                 }
;

score_sort_field: BEGIN_SCORE_SORT TAB LONG_INT NEWLINE
                   {
                     if (($3 <= INT_MIN) || ($3 >= INT_MAX))
                     {
                       bac_error("too big or small integer");
                       YYABORT;
                     }
                     score_sort = (int) $3;
                     score_sort_available = 1;
                   }
;

score_total_field: BEGIN_SCORE_TOTAL TAB LONG_INT NEWLINE
                   {
                     if (($3 <= INT_MIN) || ($3 >= INT_MAX))
                     {
                       bac_error("too big or small integer");
                       YYABORT;
                     }
                     score_total = (int) $3;
                     score_total_available = 1;
                   }
;

number_stage_field: BEGIN_NUMBER_STAGE TAB LONG_INT NEWLINE
                    {
                      if (($3 <= INT_MIN) || ($3 >= INT_MAX))
                      {
                        bac_error("too big or small integer");
                        YYABORT;
                      }
                      if (((int) $3) < 1)
                      {
                        bac_error("strange stage number");
                        YYABORT;
                      }
                      if (score_stage != NULL)
                      {
                        free(score_stage);
                        score_stage = NULL;
                      }
                      score_stage = (int *) malloc(sizeof(int) * $3);
                      if (score_stage == NULL)
                      {
                        bac_error("malloc(score_stage) failed");
                        YYABORT;
                      }
                      for (silly_i = 0; silly_i < ((int) $3); silly_i++)
                        score_stage[silly_i] = 0;
                      number_stage = $3;
                    }
;

score_stage_field: BEGIN_SCORE_STAGE TAB LONG_INT TAB LONG_INT NEWLINE
                   {
                     if (($3 <= INT_MIN) || ($3 >= INT_MAX)
                         || ($5 <= INT_MIN) || ($5 >= INT_MAX))
                     {
                       bac_error("too big or small integer");
                       YYABORT;
                     }
                     if (score_stage == NULL)
                     {
                       bac_error("total stage number not set "
                                 "before stage score");
                       YYABORT;
                     }
                     if ((((int) $3) < 1)
                         || (((int) $3) > number_stage))
                     {
                       bac_error("strange stage number");
                       YYABORT;
                     }
                     score_stage[((int) $3) - 1] = (int) $5;
                   }
;

%%
/* additional C code */

/* this is the yyerror for bison */
static void
bac_error(const char *s)
{
  fprintf (stderr, "bac_parse: %s (line %d)\n", s, bac_lineno);
}

static void
bac_parser_clear_entry(void)
{
  entry_version_available = 0;
  entry_version = 0;
  stage_data_version_available = 0;
  stage_data_version = 0;
  when_available = 0;
  when = 0;
  uid_available = 0;
  uid = 0;
  user_name_available = 0;
  if (user_name != NULL)
    free(user_name);
  user_name = NULL;
  score_sort_available = 0;
  score_sort = 0;
  score_total_available = 0;
  score_total = 0;
  number_stage = 0;
  if (score_stage != NULL)
    free(score_stage);
  score_stage = NULL;
}

static int
bac_parser_check_entry(void)
{
  if (!entry_version_available)
  {
    fprintf(stderr, "bac_parser_check_entry: Entry-Version missing\n");
    return 1;
  }
  if (entry_version != 0)
  {
    fprintf(stderr, "bac_parser_check_entry: unsupported Entry-Version\n");
    return 1;
  }
  if (!stage_data_version_available)
  {
    fprintf(stderr, "bac_parser_check_entry: Stage-Data-Version missing\n");
    return 1;
  }
  if (!when_available)
  {
    fprintf(stderr, "bac_parser_check_entry: When missing\n");
    return 1;
  }
  if (!uid_available)
  {
    fprintf(stderr, "bac_parser_check_entry: Uid missing\n");
    return 1;
  }
  /* missing user_name is OK */
  if (!score_sort_available)
  {
    fprintf(stderr, "bac_parser_check_entry: Score-Sort missing\n");
    return 1;
  }
  if (!score_total_available)
  {
    fprintf(stderr, "bac_parser_check_entry: Score-Total missing\n");
    return 1;
  }

  return 0;
}

bac_array *
bac_parser_get_array(void)
{
  return p;
}

void
bac_parser_delete_array(void)
{
  /* don't delete p or p_temp here */
  p = NULL;
  p_temp = NULL;
}
