#include <stdlib.h>

#include "predict.h"

void *lmalloc(unsigned long size, int segment_id)
{
#ifdef USE_OMPSS
    UNUSED(segment_id);
    return nanos6_lmalloc(size);
#elif defined(USE_GPI)
    return gaspi_malloc(segment_id, size);
#else
    UNUSED(segment_id);
    return malloc(size);
#endif
}


void *dmalloc(unsigned long size, int segment_id)
{
#ifdef USE_OMPSS
    return nanos6_dmalloc(size, nanos6_equpart_distribution, 0, NULL);
#else
    return lmalloc(size, segment_id);
#endif
}