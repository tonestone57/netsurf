#ifndef NETSURF_QUICKJS_LOCATION_H
#define NETSURF_QUICKJS_LOCATION_H

#include <quickjs.h>
#include "utils/nsurl.h"

/* Initialise the Location class in a context */
void qjsky_init_location(JSContext *ctx);

/* Create a new Location object */
JSValue qjsky_create_location(JSContext *ctx, nsurl *url);

#endif
