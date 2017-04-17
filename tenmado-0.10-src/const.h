/* $Id: const.h,v 1.26 2012/01/31 04:17:07 oohara Exp $ */

/* we spell "tenmado" instead of "tenm" here because these constants are
 * not portable (that is, not intended to be used in other programs)
 */
#ifndef __TENMADO_CONST_H__
#define __TENMADO_CONST_H__

#define COPYRIGHT_STRING "Copyright (C) 2002, 2012 Oohara Yuuma"

#define WINDOW_WIDTH 480
#define WINDOW_HEIGHT 640

/* attribute
 * these values must be one of 2^n
 */
#define ATTR_PLAYER 1
#define ATTR_PLAYER_SHOT 2
#define ATTR_ENEMY 4
#define ATTR_ENEMY_SHOT 8
/* "weak" enemy gets hit by it */
#define ATTR_OBSTACLE 16
/* normal enemy shot cannot go through it */
#define ATTR_OPAQUE 32

/* color */
#define COLOR_BLUE 0
#define COLOR_PURPLE 1
#define COLOR_RED 2
#define COLOR_GREEN 3

/* scheduler return value */
#define SCHEDULER_SUCCESS 0
#define SCHEDULER_ERROR 1
#define SCHEDULER_NEXT_STAGE 2

#endif /* __TENMADO_CONST_H__ */
