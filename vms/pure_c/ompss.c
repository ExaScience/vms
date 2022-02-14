#include <stdlib.h>

#include "predict.h"

void *lmalloc(unsigned long size)
{
#ifdef OMPSS
    return nanos6_lmalloc(size);
#else
    return malloc(size);
#endif
}

void *dmalloc(unsigned long size)
{
#ifdef OMPSS
    return nanos6_dmalloc(size, nanos6_equpart_distribution, 0, NULL);
#else
    return malloc(size);
#endif
}