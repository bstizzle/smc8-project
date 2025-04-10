
// handlers.h

#ifndef MESSAGE_HANDING_H
#define MESSAGE_HANDING_H

#include "detected_object.h"


int get_id(const char *path, const char *types, lo_arg **argv, int argc,void *data, void *user_data);

int get_coordinates(const char *path, const char *types, lo_arg **argv, int argc,void *data, void *user_data);

int get_data(const char *path, const char *types, lo_arg **argv, int argc,void *data, void *user_data);


#endif
