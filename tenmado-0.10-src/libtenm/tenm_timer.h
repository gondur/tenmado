/* $Id: tenm_timer.h,v 1.8 2002/05/27 11:17:46 oohara Exp $ */

#ifndef __TENM_TIMER_H__
#define __TENM_TIMER_H__

int tenm_timer_init(int num_wait);
void tenm_timer_reset(void);
int tenm_wait_next_frame(void);
double tenm_calculate_fps(int frame_passed);

#endif /* not __TENM_TIMER_H__ */
