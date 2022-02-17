#include <stdlib.h>

#include "predict.h"



void *lmalloc(unsigned long size, int segment_id)
{
#ifdef USE_OMPSS
    UNUSED(segment_id);
    if (mpi_world_size == 1) return malloc(size);
    else return nanos6_lmalloc(size);
#elif defined(USE_GPI)
    return gaspi_malloc(segment_id, size);
#else
    UNUSED(segment_id);
    return malloc(size);
#endif
}