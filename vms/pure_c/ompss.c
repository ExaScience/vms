#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>


#ifdef USE_ARGO
#include "argo.h"
#endif

#include "predict.h"

static const char     *sizes[]   = { "EiB", "PiB", "TiB", "GiB", "MiB", "KiB", "B" };
static const uint64_t  exbibytes = 1024ULL * 1024ULL * 1024ULL *
                                   1024ULL * 1024ULL * 1024ULL;

#define DIM(x) (sizeof(x)/sizeof(*(x)))

char *
calculateSize(uint64_t size)
{   
    char     *result = (char *) malloc(sizeof(char) * 20);
    uint64_t  multiplier = exbibytes;
    int i;

    for (i = 0; i < DIM(sizes); i++, multiplier /= 1024)
    {   
        if (size < multiplier)
            continue;
        if (size % multiplier == 0)
            sprintf(result, "%" PRIu64 " %s", size / multiplier, sizes[i]);
        else
            sprintf(result, "%.1f %s", (float) size / multiplier, sizes[i]);
        return result;
    }
    strcpy(result, "0");
    return result;
}


void *lmalloc(unsigned long size, int segment_id)
{
    char *sizestr = calculateSize(size);
    printf("lmalloc of %s\n", sizestr);
    free(sizestr);
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
    char *sizestr = calculateSize(size);
    printf("dmalloc of %s\n", sizestr);
    free(sizestr);
    return nanos6_dmalloc(size, nanos6_equpart_distribution, 0, NULL);
#elif defined(USE_ARGO)
    return collective_alloc(size);
#else
    return lmalloc(size, segment_id);
#endif
}
