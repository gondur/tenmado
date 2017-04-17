/* $Id: image.h,v 1.8 2002/07/13 16:10:24 oohara Exp $ */

#ifndef __TENMADO_IMAGE_H__
#define __TENMADO_IMAGE_H__

int image_init(void);
void image_quit(void);

int player_blit(int x, int y, int n);
int small_ball_blit(int x, int y, int n);
int medium_ball_blit(int x, int y, int n);
int large_ball_blit(int x, int y, int n);
int sting_blit(int x, int y, int n);
int horizon_blit(int x, int y, int n);
int plenty_blit(int x, int y, int n);
int burst_blit(int x, int y, int n);
int disconnection_core_blit(int x, int y, int n);
int disconnection_bit_blit(int x, int y, int n);
int fud_core_blit(int x, int y, int n);
int intuition_blit(int x, int y, int n);
int last_boss_blit(int x, int y, int n);
int last_boss_demo_blit(int x, int y, int n);
int midnight_blit(int x, int y, int n);
int midnight_bit_blit(int x, int y, int n);
int overrun_blit(int x, int y, int n);
int overrun_bit_blit(int x, int y, int n);
int solution_blit(int x, int y, int n);

#endif /* not __TENMADO_IMAGE_H__*/
