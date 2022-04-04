#include <cmath>
#include <algorithm>
#include <iostream>
#include <vector>
#include <random>

#include "StreamFMA.h"
#include "MaxSLiCInterface.h"

int main()
{
    const std::size_t size = 16384;
    const std::size_t sizeBytes = size * sizeof(float);

    std::random_device rnd;
    std::seed_seq seed{rnd(),rnd(),rnd(),rnd()};
    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> dist(-1.0,1.0);

    std::vector<float> a,b,c,d;

    for(auto vec : {&a,&b,&c,&d})
    {
        vec->reserve(size);
        std::generate_n(std::back_inserter(*vec),size,[&rng,&dist]{return dist(rng);});
    }
    float alpha = dist(rng);

    max_file_t *maxfile = StreamFMA_init();
    max_engine_t *engine = max_load(maxfile, "*");

    max_actions_t* act = max_actions_init(maxfile, NULL);

    max_set_double (act, "StreamFMAKernel","alpha", alpha);
    max_set_ticks  (act, "StreamFMAKernel", size);
    max_queue_input(act, "a", a.data(), size * sizeof(float));
    max_queue_input(act, "b", b.data(), size * sizeof(float));
    max_queue_input(act, "c", c.data(), size * sizeof(float));
    max_queue_output(act, "output", d.data(), size * sizeof(float));
    max_run(engine, act);

    max_actions_free(act);
    max_unload(engine);


    int ret = 0;
    // TODO Use result data
    for(std::size_t i = 0; i < size; ++i)
    {
        float ref =(c[i] + alpha * a[i] * b[i]);
        if (d[i] != ref)
        {
            std::cout << "Invalid Output at index " << i << ": " << std::endl;
            std::cout << " reference: " << ref << std::endl;
            std::cout << " value:     " << c[i] << std::endl;
            ret = 1;
            break;
        }
    }

    if(0 == ret)
    {
        std::cout << "All " << size << " values calculated correctly on the DFE!" << std::endl;
    }

    std::cout << "Done." << std::endl;
    return ret;
}
