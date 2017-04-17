/* $Id: message.h,v 1.3 2002/05/19 08:11:50 oohara Exp $ */

#ifndef __TENMADO_MESSAGE_H__
#define __TENMADO_MESSAGE_H__

#include "tenm_object.h"

tenm_object *message_new(double x, double y, double dx, double dy,
                         int life, const char *string, int length);

#endif /* not __TENMADO_MESSAGE_H__ */
