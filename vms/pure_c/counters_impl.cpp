#include <time.h>

#ifdef VMS_PROFILING

#include <thread>
#include <mutex>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <unistd.h>
#include <cmath>
#include <cassert>
#include <map>
#include <vector>
#include <algorithm>

#include "counters.h"

std::string hostname()
{
    char hostname[1024];
    gethostname(hostname, 1024);
    return std::string(hostname);
}

std::string thread_id_str()
{
    static std::mutex insert_mutex;
    
    static std::vector<std::thread::id> thread_numbers;

    auto id = std::this_thread::get_id();
    auto it = std::find(thread_numbers.begin(), thread_numbers.end(), id);
    int thread_number;

    if (it == thread_numbers.end()) 
    {
        insert_mutex.lock();
        thread_number = thread_numbers.size();
        thread_numbers.push_back(id);
        insert_mutex.unlock();
    } else {
        thread_number = it - thread_numbers.begin();
    }

    std::stringstream ss;

    ss << hostname() << "/thread_" << thread_number;
    return ss.str();
}

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
        bool hierarchical;
        void print_body(const std::string &, bool) const;

    public:
        //c-tor starts PAPI
        TotalsCounter(bool = true);
        ~TotalsCounter();

        void operator+=(const TotalsCounter &other);

        //prints results
        void print() const;
        void clear() { data.clear(); }
        bool empty() const { return data.empty(); }

        Counter &operator[](const std::string &name) {
            return data[name];
        }

};

static thread_local Counter * active_counter = 0;
static TotalsCounter hier_perf_data(true);
static TotalsCounter flat_perf_data(false);

Counter::Counter(std::string name)
    : name(name), diff(0), count(1), total_counter(false)
{
    parent = active_counter;

    fullname = (parent)
             ?  parent->fullname + "/" + name
             : thread_id_str() + "/" + name; 

    start = tick();
}

Counter::Counter()
    : parent(0), name(std::string()), fullname(std::string()), diff(0), count(0), total_counter(true)
{
}

Counter::~Counter() {
    static std::mutex update_mutex;

    if(total_counter) return;

    stop = tick();
    diff = stop - start;

    update_mutex.lock();
    hier_perf_data[fullname] += *this;
    flat_perf_data[name] += *this;
    update_mutex.unlock();

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

TotalsCounter::TotalsCounter(bool h) 
 : hierarchical(h)
{
}

TotalsCounter::~TotalsCounter()
{
    print();
}

void TotalsCounter::operator+=(const TotalsCounter &other)
{
    for(auto &t : other.data) data[t.first] += t.second;
}

void TotalsCounter::print() const {
    std::string mode = (hierarchical ? "hier" : "flat");
    std::string fname = std::string("vms_") + hostname() + "_" + std::to_string(getpid()) + "_" + mode + std::string("_prof.out");
    std::ofstream os(fname.c_str());

    os << "\nTotals on " << hostname() << " (" << getpid() << ") ";
    os << (hierarchical ? "hierarchical\n" : "flat\n");

    const auto total = data.find("main");
    for(auto &t : data)
    {
        auto parent_name = t.first.substr(0, t.first.find_last_of("/"));
        const auto parent = data.find(parent_name);
        if (hierarchical && parent != data.end())
            os << t.second.as_string(parent->second, hierarchical);
        else if (!hierarchical && total != data.end())
            os << t.second.as_string(total->second, hierarchical);
        else
            os << t.second.as_string(hierarchical);
    }
}

void perf_data_init()
{
}

void perf_data_print() {
    hier_perf_data.print();
    hier_perf_data.clear();
    flat_perf_data.print();
    flat_perf_data.clear();
}

void perf_start(const char *name)
{
    active_counter = new Counter(name);
}

void perf_end(const char *name)
{
    Counter *c = active_counter;
    assert(c->name == std::string(name));
    delete c;
}

#endif // VMS_PROFILING

