/* $Id: high-score.h,v 1.18 2002/12/20 12:19:39 oohara Exp $ */

#ifndef __TENMADO_HIGH_SCORE_H__
#define __TENMADO_HIGH_SCORE_H__

#include "bac_array.h"
/* constants for flag in read_high_score */
#include "bac_load.h"

int high_score_list_size(int stage);
bac_array *make_high_score(int total_score, const int *stage_score);
int save_high_score(bac_array *array);
int dump_high_score(int stage);
int undump_high_score(int stage);
int show_high_score(void);
bac_array **read_high_score(int flag, int *no_entry);
int truncate_high_score(bac_array **array_list);
int read_default_score(void);

#endif /* not __TENMADO_HIGH_SCORE_H__ */
