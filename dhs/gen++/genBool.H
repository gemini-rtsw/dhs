#ifndef _GENBOOL_H
#define _GENBOOL_H

/* XXX allan: Only used when there is no built-in bool type */

#ifdef HAVE_BOOL_H
#include <bool.h>
#endif

#ifndef HAVE_BOOL
#define bool int
#define true 1
#define false 0
#endif

#endif /* _GENBOOL_H */
