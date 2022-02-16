#include <time.h>


extern "C" double tick() {
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    return  (double)(t.tv_sec) + (double)(t.tv_nsec) / 1e9;
}

#ifdef VMS_PROFILING

#include <iostream>
#include <iomanip>
#include <sstream>
#include <unistd.h>
#include <cmath>
#include <map>

#include "counters.h"



struct Counter {
    Counter *parent;
    std::string name, fullname;
    double start, stop, diff; // wallclock time
    long long count;

    bool total_counter;

    Counter(std::string name);
    Counter(); // needed for std::map in TotalsCounter

    ~Counter();

    void operator+=(const Counter &other);

    std::string as_string(const Counter &total, bool hier) const;
    std::string as_string(bool hier) const;
};

struct TotalsCounter {
    private:
        std::map<std::string, Counter> data;
        int procid;
        void print_body(const std::string &, bool) const;

    public:
        //c-tor starts PAPI
        TotalsCounter(int = 0);

        void operator+=(const TotalsCounter &other);

        //prints results
        void print(int, bool) const;
        void print(bool) const;

        Counter &operator[](const std::string &name) {
            return data[name];
        }

        bool empty() const { return data.empty(); }
};

static Counter *active_counter(0);
static TotalsCounter hier_perf_data, flat_perf_data;


void perf_data_print(const TotalsCounter &data, bool hier) {
    std::string title = hier ? 
        "\nHierarchical view:\n==================\n" :
        "\nFlat view:\n==========\n";

    std::cout << title;
    data.print(hier);
}


Counter::Counter(std::string name)
    : name(name), diff(0), count(1), total_counter(false)
{
    parent = active_counter;

    fullname = (parent) ? parent->fullname + "/" + name : name; 

    start = tick();
}

Counter::Counter()
    : parent(0), name(std::string()), fullname(std::string()), diff(0), count(0), total_counter(true)
{
    if (name == "main")
    {
        //init performance counters
        perf_data_init();
    }

}

Counter::~Counter() {
    if(total_counter) return;

    stop = tick();
    diff = stop - start;

    hier_perf_data[fullname] += *this;
    flat_perf_data[name] += *this;
    active_counter = parent;
}

void Counter::operator+=(const Counter &other) {
    if (name.empty()) 
    {
        name = other.name;
        fullname = other.fullname;
    }
    diff += other.diff;
    count += other.count;
}

std::string Counter::as_string(const Counter &total, bool hier) const {
    std::ostringstream os;
    std::string n = hier ? fullname : name;
    int percent = round(100.0 * diff / (total.diff + 0.000001));
    os << ">> " << n << ":\t" << std::fixed << std::setw(11)
       << std::setprecision(4) << diff << "\t(" << percent << "%) in\t" << count << "\n";
    return os.str();
}

std::string Counter::as_string(bool hier) const
{
    std::ostringstream os;
    std::string n = hier ? fullname : name;
    os << ">> " << n << ":\t" << std::fixed << std::setw(11)
       << std::setprecision(4) << diff << " in\t" << count << "\n";
    return os.str();
}

TotalsCounter::TotalsCounter(int p) : procid(p) {}

void TotalsCounter::operator+=(const TotalsCounter &other)
{
    for(auto &t : other.data) data[t.first] += t.second;
}

void TotalsCounter::print(bool hier) const {
    if (data.empty()) return;
    char hostname[1024];
    gethostname(hostname, 1024);
    std::cout << "\nTotals on " << hostname << " (" << procid << ") ";
    std::cout << (hier ? "hierarchical\n" : "flat\n");

    const auto total = data.find("main");
    for(auto &t : data)
    {
        auto parent_name = t.first.substr(0, t.first.find_last_of("/"));
        const auto parent = data.find(parent_name);
        if (hier && parent != data.end())
            std::cout << t.second.as_string(parent->second, hier);
        else if (!hier && total != data.end())
            std::cout << t.second.as_string(total->second, hier);
        else
            std::cout << t.second.as_string(hier);
    }
}

extern "C"
{
    void perf_data_init()
    {
    }

    void perf_data_print()
    {
        perf_data_print(hier_perf_data, true);
        perf_data_print(flat_perf_data, false);
    }

    void perf_start(const char *name)
    {
        active_counter = new Counter(name);
    }

    void perf_end()
    {
        Counter *parent = active_counter->parent;
        delete active_counter;
        active_counter = parent;
    }
}

#else

extern "C"
{
    void perf_data_init() {}
    void perf_data_print() {}
    void perf_start(const char *name) {}
    void perf_end() {}
}

#endif // VMS_PROFILING

