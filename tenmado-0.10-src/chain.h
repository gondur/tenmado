/* $Id: chain.h,v 1.5 2002/05/14 16:27:17 oohara Exp $ */

#ifndef __TENMADO_CHAIN_H__
#define __TENMADO_CHAIN_H__

void clear_chain(void);
int get_color_expected(void);
int add_killed(int color);
void calculate_chain(void);
int show_chain(void);

#endif /* not __TENMADO_CHAIN_H__ */
