/* $Id: bac_string_to_array.c,v 1.19 2012/01/29 03:57:15 oohara Exp $ */

/* FILE */
#include <stdio.h>
/* isprint */
#include <ctype.h>

#include "bac_array.h"
#include "bac_lexical.h"
#include "bac_parser_public.h"

#include "bac_string_to_array.h"

bac_array *
bac_string_to_array(const char *string)
{
  /* sanity check */
  if (string == NULL)
  {
    fprintf(stderr, "bac_string_to_array: string is NULL\n");
    return NULL;
  }

  if (isprint('\n'))
  {
    fprintf(stderr, "bac_string_to_array: \\n is printable\n");
    return NULL;
  }
  if (isprint('\t'))
  {
    fprintf(stderr, "bac_string_to_array: \\t is printable\n");
    return NULL;
  }

  bac_parser_delete_array();
  bac_lineno = 1;
  bac_read_from_string(string);
  if (bac_parse() != 0)
  {
    fprintf(stderr, "bac_string_to_array: bac_parse failed\n");
    /* delete the buffer */
    bac_close_string();
    return NULL;
  }
  /* delete the buffer */
  bac_close_string();

  return bac_parser_get_array();
}

bac_array *
bac_file_to_array(FILE *file)
{
  /* sanity check */
  if (file == NULL)
  {
    fprintf(stderr, "file is NULL\n");
    return NULL;
  }
  
  if (isprint('\n'))
  {
    fprintf(stderr, "bac_file_to_array: \\n is printable\n");
    return NULL;
  }
  if (isprint('\t'))
  {
    fprintf(stderr, "bac_file_to_array: \\t is printable\n");
    return NULL;
  }

  bac_parser_delete_array();
  bac_lineno = 1;
  bac_read_from_file(file);

  if (bac_parse() != 0)
  {
    fprintf(stderr, "bac_file_to_array: bac_parse failed\n");
    return NULL;
  }
  /* don't close the file here --- it is not my job */

  return bac_parser_get_array();
}
