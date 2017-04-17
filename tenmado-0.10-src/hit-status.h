/* $Id: hit-status.h,v 1.3 2002/07/10 07:05:13 oohara Exp $ */

#ifndef __TENMADO_HIT_STATUS_H__
#define __TENMADO_HIT_STATUS_H__

void clear_hit_status(void);
void set_hit_status(const char *name, int hit_point, int remaining);
int show_hit_status(void);

#endif /* not __TENMADO_HIT_STATUS_H__ */
