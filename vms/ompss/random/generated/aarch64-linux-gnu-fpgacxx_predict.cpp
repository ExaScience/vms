typedef long int ptrdiff_t;
typedef unsigned long int size_t;
struct  max_align_t
{
    __attribute__((aligned(alignof(long long int)))) long long int __max_align_ll;
    __attribute__((aligned(alignof(long double)))) long double __max_align_ld;
};
typedef decltype(nullptr) nullptr_t;
typedef signed char int8_t;
typedef short int int16_t;
typedef int int32_t;
typedef long int int64_t;
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long int uint64_t;
typedef signed char int_least8_t;
typedef short int int_least16_t;
typedef int int_least32_t;
typedef long int int_least64_t;
typedef unsigned char uint_least8_t;
typedef unsigned short int uint_least16_t;
typedef unsigned int uint_least32_t;
typedef unsigned long int uint_least64_t;
typedef signed char int_fast8_t;
typedef long int int_fast16_t;
typedef long int int_fast32_t;
typedef long int int_fast64_t;
typedef unsigned char uint_fast8_t;
typedef unsigned long int uint_fast16_t;
typedef unsigned long int uint_fast32_t;
typedef unsigned long int uint_fast64_t;
typedef long int intptr_t;
typedef unsigned long int uintptr_t;
typedef long int intmax_t;
typedef unsigned long int uintmax_t;
struct  nanos_region_dimension_internal_t
{
    ::size_t size;
    ::size_t lower_bound;
    ::size_t accessed_length;
};
struct  nanos_access_type_internal_t
{
    bool input:1;
    bool output:1;
    bool can_rename:1;
    bool concurrent:1;
    bool commutative:1;
};
struct  nanos_data_access_internal_t
{
    void *address;
    ::nanos_access_type_internal_t flags;
    short int dimension_count;
    const ::nanos_region_dimension_internal_t *dimensions;
    ::ptrdiff_t offset;
};
enum nanos_sharing_t
{
  NANOS_PRIVATE = 0,
  NANOS_SHARED = 1
};
struct  nanos_reduction_t
{
    void *original;
    void *privates;
    ::size_t element_size;
    ::size_t num_scalars;
    void *descriptor;
    void (*bop)(void *, void *, int);
    void (*vop)(int, void *, void *);
    void (*cleanup)(void *);
};
typedef unsigned int reg_t;
typedef unsigned int memory_space_id_t;
struct  nanos_copy_data_internal_t
{
    void *address;
    ::nanos_sharing_t sharing;
    struct  mcc_struct_anon_6
    {
        bool input:1;
        bool output:1;
    };
    ::nanos_copy_data_internal_t::mcc_struct_anon_6 flags;
    short int dimension_count;
    const ::nanos_region_dimension_internal_t *dimensions;
    ::ptrdiff_t offset;
    ::uint64_t host_base_address;
    ::reg_t host_region_id;
    bool remote_host;
    void *deducted_cd;
};
typedef ::nanos_access_type_internal_t nanos_access_type_t;
typedef ::nanos_region_dimension_internal_t nanos_region_dimension_t;
typedef ::nanos_data_access_internal_t nanos_data_access_t;
typedef ::nanos_copy_data_internal_t nanos_copy_data_t;
typedef void *nanos_thread_t;
typedef void *nanos_wd_t;
typedef void *nanos_pe_t;
struct  nanos_compound_wd_data_t
{
    int nsect;
    ::nanos_wd_t lwd[];
};
struct  nanos_repeat_n_info_t
{
    int n;
};
struct  nanos_loop_info_t
{
    ::int64_t lower;
    ::int64_t upper;
    ::int64_t step;
    bool last;
    bool wait;
    ::int64_t chunk;
    ::int64_t stride;
    int thid;
    int threads;
    void *args;
};
typedef void *nanos_ws_t;
typedef void *nanos_ws_info_t;
typedef void *nanos_ws_data_t;
typedef void *nanos_ws_item_t;
struct  nanos_ws_info_loop_t
{
    ::int64_t lower_bound;
    ::int64_t upper_bound;
    ::int64_t loop_step;
    ::int64_t chunk_size;
};
struct  nanos_ws_item_loop_t
{
    ::int64_t lower;
    ::int64_t upper;
    bool execute:1;
    bool last:1;
};
struct nanos_ws_desc;
struct  nanos_ws_desc
{
    volatile ::nanos_ws_t ws;
    ::nanos_ws_data_t data;
    ::nanos_ws_desc *next;
    ::nanos_thread_t *threads;
    int nths;
};
typedef ::nanos_ws_desc nanos_ws_desc_t;
struct  nanos_wd_props_t
{
    bool mandatory_creation:1;
    bool tied:1;
    bool clear_chunk:1;
    bool reserved0:1;
    bool reserved1:1;
    bool reserved2:1;
    bool reserved3:1;
    bool reserved4:1;
};
struct  nanos_wd_dyn_flags_t
{
    bool is_final:1;
    bool is_recover:1;
    bool is_implicit:1;
    bool reserved3:1;
    bool reserved4:1;
    bool reserved5:1;
    bool reserved6:1;
    bool reserved7:1;
};
struct  nanos_wd_dyn_props_t
{
    ::nanos_wd_dyn_flags_t flags;
    ::nanos_thread_t tie_to;
    int priority;
    void *callback;
    void *arguments;
};
struct  nanos_device_t
{
    void *(*factory)(void *);
    void *arg;
};
struct  nanos_smp_args_t
{
    void (*outline)(void *);
};
extern "C"
{
  extern void *nanos_smp_factory_(void *args);
}
extern "C"
{
  extern void *nanos_smp_factory(void *args);
}
enum nanos_event_type_t
{
  NANOS_STATE_START = 0,
  NANOS_STATE_END = 1,
  NANOS_SUBSTATE_START = 2,
  NANOS_SUBSTATE_END = 3,
  NANOS_BURST_START = 4,
  NANOS_BURST_END = 5,
  NANOS_PTP_START = 6,
  NANOS_PTP_END = 7,
  NANOS_POINT = 8,
  EVENT_TYPES = 9
};
typedef unsigned int nanos_event_key_t;
typedef unsigned long long int nanos_event_value_t;
typedef unsigned long long int nanos_event_time_t;
enum nanos_event_state_value_t
{
  NANOS_NOT_CREATED = 0,
  NANOS_NOT_RUNNING = 1,
  NANOS_STARTUP = 2,
  NANOS_SHUTDOWN = 3,
  NANOS_ERROR = 4,
  NANOS_IDLE = 5,
  NANOS_RUNTIME = 6,
  NANOS_RUNNING = 7,
  NANOS_SYNCHRONIZATION = 8,
  NANOS_SCHEDULING = 9,
  NANOS_CREATION = 10,
  NANOS_MEM_TRANSFER_ISSUE = 11,
  NANOS_CACHE = 12,
  NANOS_YIELD = 13,
  NANOS_ACQUIRING_LOCK = 14,
  NANOS_CONTEXT_SWITCH = 15,
  NANOS_FILL1 = 16,
  NANOS_WAKINGUP = 17,
  NANOS_STOPPED = 18,
  NANOS_SYNCED_RUNNING = 19,
  NANOS_DEBUG = 20,
  NANOS_EVENT_STATE_TYPES = 21
};
enum nanos_event_domain_t
{
  NANOS_WD_DOMAIN = 0,
  NANOS_WD_DEPENDENCY = 1,
  NANOS_WAIT = 2,
  NANOS_XFER_DATA = 3,
  NANOS_XFER_REQ = 4,
  NANOS_WD_REMOTE = 5,
  NANOS_AM_WORK = 6,
  NANOS_AM_WORK_DONE = 7,
  NANOS_XFER_WAIT_REQ_PUT = 8,
  NANOS_XFER_FREE_TMP_BUFF = 9
};
typedef long long int nanos_event_id_t;
struct  nanos_event_t
{
    ::nanos_event_type_t type;
    ::nanos_event_key_t key;
    ::nanos_event_value_t value;
    ::nanos_event_domain_t domain;
    ::nanos_event_id_t id;
};
enum nanos_lock_state_t
{
  NANOS_LOCK_FREE = 0,
  NANOS_LOCK_BUSY = 1
};
struct  nanos_lock_t
{
    volatile ::nanos_lock_state_t state_;
    inline nanos_lock_t(::nanos_lock_state_t init  = (::NANOS_LOCK_FREE))
      : state_(init)
    {
    }
};
typedef void (*nanos_translate_args_t)(void *, ::nanos_wd_t);
typedef void nanos_init_func_t(void *);
struct  nanos_init_desc_t
{
    ::nanos_init_func_t (*func);
    void *data;
};
enum nanos_err_t
{
  NANOS_OK = 0,
  NANOS_UNKNOWN_ERR = 1,
  NANOS_UNIMPLEMENTED = 2,
  NANOS_ENOMEM = 3,
  NANOS_INVALID_PARAM = 4,
  NANOS_INVALID_REQUEST = 5
};
typedef void *nanos_wg_t;
typedef void *nanos_team_t;
typedef void *nanos_sched_t;
typedef void *nanos_slicer_t;
typedef void *nanos_dd_t;
typedef void *nanos_sync_cond_t;
typedef unsigned int nanos_copy_id_t;
struct  nanos_const_wd_definition_tag
{
    ::nanos_wd_props_t props;
    ::size_t data_alignment;
    ::size_t num_copies;
    ::size_t num_devices;
    ::size_t num_dimensions;
    const char *description;
};
typedef ::nanos_const_wd_definition_tag nanos_const_wd_definition_t;
struct  nanos_const_wd_definition_1
{
    ::nanos_const_wd_definition_t base;
    ::nanos_device_t devices[1L];
};
struct  nanos_constraint_t
{
    int nthreads;
    void *arch;
};
typedef void *nanos_cpu_set_t;
typedef const void *const_nanos_cpu_set_t;
struct  nanos_const_wd_definition_internal_t : ::nanos_const_wd_definition_tag
{
    ::nanos_device_t devices[1L];
};
extern "C"
{
  extern char *nanos_get_mode_();
}
extern "C"
{
  extern char *nanos_get_mode();
}
extern "C"
{
  extern ::nanos_wd_t nanos_current_wd_();
}
extern "C"
{
  extern ::nanos_wd_t nanos_current_wd();
}
extern "C"
{
  extern int nanos_get_wd_id_(::nanos_wd_t wd);
}
extern "C"
{
  extern int nanos_get_wd_id(::nanos_wd_t wd);
}
extern "C"
{
  extern int nanos_get_wd_priority_(::nanos_wd_t wd);
}
extern "C"
{
  extern int nanos_get_wd_priority(::nanos_wd_t wd);
}
extern "C"
{
  extern void nanos_set_wd_priority_(::nanos_wd_t wd, int p);
}
extern "C"
{
  extern void nanos_set_wd_priority(::nanos_wd_t wd, int p);
}
extern "C"
{
  extern ::nanos_err_t nanos_get_wd_description_(const char **description, ::nanos_wd_t wd);
}
extern "C"
{
  extern ::nanos_err_t nanos_get_wd_description(const char **description, ::nanos_wd_t wd);
}
extern "C"
{
  extern bool nanos_try_lock_pe_(::nanos_pe_t pe);
}
extern "C"
{
  extern bool nanos_try_lock_pe(::nanos_pe_t pe);
}
extern "C"
{
  extern void nanos_unlock_pe_(::nanos_pe_t pe);
}
extern "C"
{
  extern void nanos_unlock_pe(::nanos_pe_t pe);
}
extern "C"
{
  extern ::nanos_slicer_t nanos_find_slicer_(const char *slicer);
}
extern "C"
{
  extern ::nanos_slicer_t nanos_find_slicer(const char *slicer);
}
extern "C"
{
  extern ::nanos_ws_t nanos_find_worksharing_(const char *label);
}
extern "C"
{
  extern ::nanos_ws_t nanos_find_worksharing(const char *label);
}
extern "C"
{
  extern ::nanos_err_t nanos_find_smp_pe_(void *req, ::nanos_pe_t *pe);
}
extern "C"
{
  extern ::nanos_err_t nanos_find_smp_pe(void *req, ::nanos_pe_t *pe);
}
extern "C"
{
  extern ::nanos_err_t nanos_create_wd_compact_(::nanos_wd_t *wd, ::nanos_const_wd_definition_t *const_data, ::nanos_wd_dyn_props_t *dyn_props, ::size_t data_size, void **data, ::nanos_wg_t wg, ::nanos_copy_data_t **copies, ::nanos_region_dimension_internal_t **dimensions);
}
extern "C"
{
  extern ::nanos_err_t nanos_create_wd_compact(::nanos_wd_t *wd, ::nanos_const_wd_definition_t *const_data, ::nanos_wd_dyn_props_t *dyn_props, ::size_t data_size, void **data, ::nanos_wg_t wg, ::nanos_copy_data_t **copies, ::nanos_region_dimension_internal_t **dimensions);
}
extern "C"
{
  extern ::nanos_err_t nanos_set_translate_function_(::nanos_wd_t wd, ::nanos_translate_args_t translate_args);
}
extern "C"
{
  extern ::nanos_err_t nanos_set_translate_function(::nanos_wd_t wd, ::nanos_translate_args_t translate_args);
}
extern "C"
{
  extern ::nanos_err_t nanos_create_sliced_wd_(::nanos_wd_t *uwd, ::size_t num_devices, ::nanos_device_t *devices, ::size_t outline_data_size, int outline_data_align, void **outline_data, ::nanos_wg_t uwg, ::nanos_slicer_t slicer, ::nanos_wd_props_t *props, ::nanos_wd_dyn_props_t *dyn_props, ::size_t num_copies, ::nanos_copy_data_t **copies, ::size_t num_dimensions, ::nanos_region_dimension_internal_t **dimensions);
}
extern "C"
{
  extern ::nanos_err_t nanos_create_sliced_wd(::nanos_wd_t *uwd, ::size_t num_devices, ::nanos_device_t *devices, ::size_t outline_data_size, int outline_data_align, void **outline_data, ::nanos_wg_t uwg, ::nanos_slicer_t slicer, ::nanos_wd_props_t *props, ::nanos_wd_dyn_props_t *dyn_props, ::size_t num_copies, ::nanos_copy_data_t **copies, ::size_t num_dimensions, ::nanos_region_dimension_internal_t **dimensions);
}
extern "C"
{
  extern ::nanos_err_t nanos_submit_(::nanos_wd_t wd, ::size_t num_data_accesses, ::nanos_data_access_t *data_accesses, ::nanos_team_t team);
}
extern "C"
{
  extern ::nanos_err_t nanos_submit(::nanos_wd_t wd, ::size_t num_data_accesses, ::nanos_data_access_t *data_accesses, ::nanos_team_t team);
}
extern "C"
{
  extern ::nanos_err_t nanos_outline_(::nanos_wd_t wd, ::nanos_pe_t pe);
}
extern "C"
{
  extern ::nanos_err_t nanos_outline(::nanos_wd_t wd, ::nanos_pe_t pe);
}
extern "C"
{
  extern ::nanos_err_t nanos_create_wd_and_run_compact_(::nanos_const_wd_definition_t *const_data, ::nanos_wd_dyn_props_t *dyn_props, ::size_t data_size, void *data, ::size_t num_data_accesses, ::nanos_data_access_t *data_accesses, ::nanos_copy_data_t *copies, ::nanos_region_dimension_internal_t *dimensions, ::nanos_translate_args_t translate_args);
}
extern "C"
{
  extern ::nanos_err_t nanos_create_wd_and_run_compact(::nanos_const_wd_definition_t *const_data, ::nanos_wd_dyn_props_t *dyn_props, ::size_t data_size, void *data, ::size_t num_data_accesses, ::nanos_data_access_t *data_accesses, ::nanos_copy_data_t *copies, ::nanos_region_dimension_internal_t *dimensions, ::nanos_translate_args_t translate_args);
}
extern "C"
{
  extern ::nanos_err_t nanos_create_for_();
}
extern "C"
{
  extern ::nanos_err_t nanos_create_for();
}
extern "C"
{
  extern ::nanos_err_t nanos_set_internal_wd_data_(::nanos_wd_t wd, void *data);
}
extern "C"
{
  extern ::nanos_err_t nanos_set_internal_wd_data(::nanos_wd_t wd, void *data);
}
extern "C"
{
  extern ::nanos_err_t nanos_get_internal_wd_data_(::nanos_wd_t wd, void **data);
}
extern "C"
{
  extern ::nanos_err_t nanos_get_internal_wd_data(::nanos_wd_t wd, void **data);
}
extern "C"
{
  extern ::nanos_err_t nanos_yield_();
}
extern "C"
{
  extern ::nanos_err_t nanos_yield();
}
extern "C"
{
  extern ::nanos_err_t nanos_slicer_get_specific_data_(::nanos_slicer_t slicer, void **data);
}
extern "C"
{
  extern ::nanos_err_t nanos_slicer_get_specific_data(::nanos_slicer_t slicer, void **data);
}
extern "C"
{
  extern ::nanos_err_t nanos_get_num_ready_tasks_(unsigned int *ready_tasks);
}
extern "C"
{
  extern ::nanos_err_t nanos_get_num_ready_tasks(unsigned int *ready_tasks);
}
extern "C"
{
  extern ::nanos_err_t nanos_get_num_total_tasks_(unsigned int *total_tasks);
}
extern "C"
{
  extern ::nanos_err_t nanos_get_num_total_tasks(unsigned int *total_tasks);
}
extern "C"
{
  extern ::nanos_err_t nanos_get_num_nonready_tasks_(unsigned int *nonready_tasks);
}
extern "C"
{
  extern ::nanos_err_t nanos_get_num_nonready_tasks(unsigned int *nonready_tasks);
}
extern "C"
{
  extern ::nanos_err_t nanos_get_num_running_tasks_(unsigned int *running_tasks);
}
extern "C"
{
  extern ::nanos_err_t nanos_get_num_running_tasks(unsigned int *running_tasks);
}
extern "C"
{
  extern ::nanos_err_t nanos_get_num_blocked_tasks_(unsigned int *blocked_tasks);
}
extern "C"
{
  extern ::nanos_err_t nanos_get_num_blocked_tasks(unsigned int *blocked_tasks);
}
extern "C"
{
  extern ::nanos_err_t nanos_in_final_(bool *result);
}
extern "C"
{
  extern ::nanos_err_t nanos_in_final(bool *result);
}
extern "C"
{
  extern ::nanos_err_t nanos_set_final_(bool value);
}
extern "C"
{
  extern ::nanos_err_t nanos_set_final(bool value);
}
extern "C"
{
  extern ::nanos_err_t nanos_switch_to_thread_(unsigned int *thid);
}
extern "C"
{
  extern ::nanos_err_t nanos_switch_to_thread(unsigned int *thid);
}
extern "C"
{
  extern ::nanos_err_t nanos_is_tied_(bool *result);
}
extern "C"
{
  extern ::nanos_err_t nanos_is_tied(bool *result);
}
extern "C"
{
  extern ::nanos_err_t nanos_create_team_(::nanos_team_t *team, ::nanos_sched_t sg, unsigned int *nthreads, ::nanos_constraint_t *constraints, bool reuse, ::nanos_thread_t *info, ::nanos_const_wd_definition_t *const_data);
}
extern "C"
{
  extern ::nanos_err_t nanos_create_team(::nanos_team_t *team, ::nanos_sched_t sg, unsigned int *nthreads, ::nanos_constraint_t *constraints, bool reuse, ::nanos_thread_t *info, ::nanos_const_wd_definition_t *const_data);
}
extern "C"
{
  extern ::nanos_err_t nanos_create_team_mapped_(::nanos_team_t *team, ::nanos_sched_t sg, unsigned int *nthreads, unsigned int *mapping);
}
extern "C"
{
  extern ::nanos_err_t nanos_create_team_mapped(::nanos_team_t *team, ::nanos_sched_t sg, unsigned int *nthreads, unsigned int *mapping);
}
extern "C"
{
  extern ::nanos_err_t nanos_enter_team_();
}
extern "C"
{
  extern ::nanos_err_t nanos_enter_team();
}
extern "C"
{
  extern ::nanos_err_t nanos_leave_team_();
}
extern "C"
{
  extern ::nanos_err_t nanos_leave_team();
}
extern "C"
{
  extern ::nanos_err_t nanos_end_team_(::nanos_team_t team);
}
extern "C"
{
  extern ::nanos_err_t nanos_end_team(::nanos_team_t team);
}
extern "C"
{
  extern ::nanos_err_t nanos_team_barrier_();
}
extern "C"
{
  extern ::nanos_err_t nanos_team_barrier();
}
extern "C"
{
  extern ::nanos_err_t nanos_single_guard_(bool *);
}
extern "C"
{
  extern ::nanos_err_t nanos_single_guard(bool *);
}
extern "C"
{
  extern ::nanos_err_t nanos_enter_sync_init_(bool *b);
}
extern "C"
{
  extern ::nanos_err_t nanos_enter_sync_init(bool *b);
}
extern "C"
{
  extern ::nanos_err_t nanos_wait_sync_init_();
}
extern "C"
{
  extern ::nanos_err_t nanos_wait_sync_init();
}
extern "C"
{
  extern ::nanos_err_t nanos_release_sync_init_();
}
extern "C"
{
  extern ::nanos_err_t nanos_release_sync_init();
}
extern "C"
{
  extern ::nanos_err_t nanos_memory_fence_();
}
extern "C"
{
  extern ::nanos_err_t nanos_memory_fence();
}
extern "C"
{
  extern ::nanos_err_t nanos_team_get_num_supporting_threads_(int *n);
}
extern "C"
{
  extern ::nanos_err_t nanos_team_get_num_supporting_threads(int *n);
}
extern "C"
{
  extern ::nanos_err_t nanos_team_get_supporting_threads_(int *n, ::nanos_thread_t *list_of_threads);
}
extern "C"
{
  extern ::nanos_err_t nanos_team_get_supporting_threads(int *n, ::nanos_thread_t *list_of_threads);
}
extern "C"
{
  extern ::nanos_err_t nanos_register_reduction_(::nanos_reduction_t *red);
}
extern "C"
{
  extern ::nanos_err_t nanos_register_reduction(::nanos_reduction_t *red);
}
extern "C"
{
  extern ::nanos_err_t nanos_reduction_get_private_data_(void **copy, void *original);
}
extern "C"
{
  extern ::nanos_err_t nanos_reduction_get_private_data(void **copy, void *original);
}
extern "C"
{
  extern ::nanos_err_t nanos_reduction_get_(::nanos_reduction_t **dest, void *original);
}
extern "C"
{
  extern ::nanos_err_t nanos_reduction_get(::nanos_reduction_t **dest, void *original);
}
extern "C"
{
  extern ::nanos_err_t nanos_task_reduction_register_(void *orig, ::size_t size_target, ::size_t size_elem, void (*init)(void *, void *), void (*reducer)(void *, void *));
}
extern "C"
{
  extern ::nanos_err_t nanos_task_reduction_register(void *orig, ::size_t size_target, ::size_t size_elem, void (*init)(void *, void *), void (*reducer)(void *, void *));
}
extern "C"
{
  extern ::nanos_err_t nanos_task_fortran_array_reduction_register_(void *orig, void *dep, ::size_t array_descriptor_size, void (*init)(void *, void *), void (*reducer)(void *, void *), void (*reducer_orig_var)(void *, void *));
}
extern "C"
{
  extern ::nanos_err_t nanos_task_fortran_array_reduction_register(void *orig, void *dep, ::size_t array_descriptor_size, void (*init)(void *, void *), void (*reducer)(void *, void *), void (*reducer_orig_var)(void *, void *));
}
extern "C"
{
  extern ::nanos_err_t nanos_task_reduction_get_thread_storage_(void *orig, void **tpd);
}
extern "C"
{
  extern ::nanos_err_t nanos_task_reduction_get_thread_storage(void *orig, void **tpd);
}
extern "C"
{
  extern ::nanos_err_t nanos_admit_current_thread_();
}
extern "C"
{
  extern ::nanos_err_t nanos_admit_current_thread();
}
extern "C"
{
  extern ::nanos_err_t nanos_expel_current_thread_();
}
extern "C"
{
  extern ::nanos_err_t nanos_expel_current_thread();
}
extern "C"
{
  extern ::nanos_err_t nanos_dependence_release_all_();
}
extern "C"
{
  extern ::nanos_err_t nanos_dependence_release_all();
}
extern "C"
{
  extern ::nanos_err_t nanos_dependence_pendant_writes_(bool *res, void *addr);
}
extern "C"
{
  extern ::nanos_err_t nanos_dependence_pendant_writes(bool *res, void *addr);
}
extern "C"
{
  extern ::nanos_err_t nanos_dependence_create_(::nanos_wd_t pred, ::nanos_wd_t succ);
}
extern "C"
{
  extern ::nanos_err_t nanos_dependence_create(::nanos_wd_t pred, ::nanos_wd_t succ);
}
extern "C"
{
  extern ::nanos_err_t nanos_worksharing_create_(::nanos_ws_desc_t **wsd, ::nanos_ws_t ws, ::nanos_ws_info_t *info, bool *b);
}
extern "C"
{
  extern ::nanos_err_t nanos_worksharing_create(::nanos_ws_desc_t **wsd, ::nanos_ws_t ws, ::nanos_ws_info_t *info, bool *b);
}
extern "C"
{
  extern ::nanos_err_t nanos_worksharing_next_item_(::nanos_ws_desc_t *wsd, ::nanos_ws_item_t *wsi);
}
extern "C"
{
  extern ::nanos_err_t nanos_worksharing_next_item(::nanos_ws_desc_t *wsd, ::nanos_ws_item_t *wsi);
}
extern "C"
{
  extern ::nanos_err_t nanos_wg_wait_completion_mandatory_(::nanos_wg_t wg, bool avoid_flush);
}
extern "C"
{
  extern ::nanos_err_t nanos_wg_wait_completion_mandatory(::nanos_wg_t wg, bool avoid_flush);
}
extern "C"
{
  extern ::nanos_err_t nanos_wg_wait_completion_(::nanos_wg_t wg, bool avoid_flush);
}
extern "C"
{
  extern ::nanos_err_t nanos_wg_wait_completion(::nanos_wg_t wg, bool avoid_flush);
}
extern "C"
{
  extern ::nanos_err_t nanos_create_int_sync_cond_(::nanos_sync_cond_t *sync_cond, volatile int *p, int condition);
}
extern "C"
{
  extern ::nanos_err_t nanos_create_int_sync_cond(::nanos_sync_cond_t *sync_cond, volatile int *p, int condition);
}
extern "C"
{
  extern ::nanos_err_t nanos_create_bool_sync_cond_(::nanos_sync_cond_t *sync_cond, volatile bool *p, bool condition);
}
extern "C"
{
  extern ::nanos_err_t nanos_create_bool_sync_cond(::nanos_sync_cond_t *sync_cond, volatile bool *p, bool condition);
}
extern "C"
{
  extern ::nanos_err_t nanos_sync_cond_wait_(::nanos_sync_cond_t sync_cond);
}
extern "C"
{
  extern ::nanos_err_t nanos_sync_cond_wait(::nanos_sync_cond_t sync_cond);
}
extern "C"
{
  extern ::nanos_err_t nanos_sync_cond_signal_(::nanos_sync_cond_t sync_cond);
}
extern "C"
{
  extern ::nanos_err_t nanos_sync_cond_signal(::nanos_sync_cond_t sync_cond);
}
extern "C"
{
  extern ::nanos_err_t nanos_destroy_sync_cond_(::nanos_sync_cond_t sync_cond);
}
extern "C"
{
  extern ::nanos_err_t nanos_destroy_sync_cond(::nanos_sync_cond_t sync_cond);
}
extern "C"
{
  extern ::nanos_err_t nanos_wait_on_(::size_t num_data_accesses, ::nanos_data_access_t *data_accesses);
}
extern "C"
{
  extern ::nanos_err_t nanos_wait_on(::size_t num_data_accesses, ::nanos_data_access_t *data_accesses);
}
extern "C"
{
  extern ::nanos_err_t nanos_init_lock_(::nanos_lock_t **lock);
}
extern "C"
{
  extern ::nanos_err_t nanos_init_lock(::nanos_lock_t **lock);
}
extern "C"
{
  extern ::nanos_err_t nanos_init_lock_at_(::nanos_lock_t *lock);
}
extern "C"
{
  extern ::nanos_err_t nanos_init_lock_at(::nanos_lock_t *lock);
}
extern "C"
{
  extern ::nanos_err_t nanos_set_lock_(::nanos_lock_t *lock);
}
extern "C"
{
  extern ::nanos_err_t nanos_set_lock(::nanos_lock_t *lock);
}
extern "C"
{
  extern ::nanos_err_t nanos_unset_lock_(::nanos_lock_t *lock);
}
extern "C"
{
  extern ::nanos_err_t nanos_unset_lock(::nanos_lock_t *lock);
}
extern "C"
{
  extern ::nanos_err_t nanos_try_lock_(::nanos_lock_t *lock, bool *result);
}
extern "C"
{
  extern ::nanos_err_t nanos_try_lock(::nanos_lock_t *lock, bool *result);
}
extern "C"
{
  extern ::nanos_err_t nanos_destroy_lock_(::nanos_lock_t *lock);
}
extern "C"
{
  extern ::nanos_err_t nanos_destroy_lock(::nanos_lock_t *lock);
}
extern "C"
{
  extern ::nanos_err_t nanos_get_lock_address_(void *addr, ::nanos_lock_t **lock);
}
extern "C"
{
  extern ::nanos_err_t nanos_get_lock_address(void *addr, ::nanos_lock_t **lock);
}
extern "C"
{
  extern ::nanos_err_t nanos_set_copies_(::nanos_wd_t wd, int num_copies, ::nanos_copy_data_t *copies);
}
extern "C"
{
  extern ::nanos_err_t nanos_set_copies(::nanos_wd_t wd, int num_copies, ::nanos_copy_data_t *copies);
}
extern "C"
{
  extern ::nanos_err_t nanos_get_addr_(::nanos_copy_id_t copy_id, void **addr, ::nanos_wd_t cwd);
}
extern "C"
{
  extern ::nanos_err_t nanos_get_addr(::nanos_copy_id_t copy_id, void **addr, ::nanos_wd_t cwd);
}
extern "C"
{
  extern ::nanos_err_t nanos_copy_value_(void *dst, ::nanos_copy_id_t copy_id, ::nanos_wd_t cwd);
}
extern "C"
{
  extern ::nanos_err_t nanos_copy_value(void *dst, ::nanos_copy_id_t copy_id, ::nanos_wd_t cwd);
}
extern "C"
{
  extern const char *nanos_get_runtime_version_();
}
extern "C"
{
  extern const char *nanos_get_runtime_version();
}
extern "C"
{
  extern const char *nanos_get_default_architecture_();
}
extern "C"
{
  extern const char *nanos_get_default_architecture();
}
extern "C"
{
  extern const char *nanos_get_pm_();
}
extern "C"
{
  extern const char *nanos_get_pm();
}
extern "C"
{
  extern ::nanos_err_t nanos_get_default_binding_(bool *res);
}
extern "C"
{
  extern ::nanos_err_t nanos_get_default_binding(bool *res);
}
extern "C"
{
  extern ::nanos_err_t nanos_get_binding_(::nanos_cpu_set_t *);
}
extern "C"
{
  extern ::nanos_err_t nanos_get_binding(::nanos_cpu_set_t *);
}
extern "C"
{
  extern ::nanos_err_t nanos_delay_start_();
}
extern "C"
{
  extern ::nanos_err_t nanos_delay_start();
}
extern "C"
{
  extern ::nanos_err_t nanos_start_();
}
extern "C"
{
  extern ::nanos_err_t nanos_start();
}
extern "C"
{
  extern ::nanos_err_t nanos_finish_();
}
extern "C"
{
  extern ::nanos_err_t nanos_finish();
}
extern "C"
{
  extern ::nanos_err_t nanos_current_socket_(int socket);
}
extern "C"
{
  extern ::nanos_err_t nanos_current_socket(int socket);
}
extern "C"
{
  extern ::nanos_err_t nanos_get_num_sockets_(int *num_sockets);
}
extern "C"
{
  extern ::nanos_err_t nanos_get_num_sockets(int *num_sockets);
}
extern "C"
{
  extern ::nanos_err_t nanos_malloc_(void **p, ::size_t size, const char *file, int line);
}
extern "C"
{
  extern ::nanos_err_t nanos_malloc(void **p, ::size_t size, const char *file, int line);
}
extern "C"
{
  extern ::nanos_err_t nanos_memalign_(void **p, ::size_t size, const char *file, int line);
}
extern "C"
{
  extern ::nanos_err_t nanos_memalign(void **p, ::size_t size, const char *file, int line);
}
extern "C"
{
  extern ::nanos_err_t nanos_cmalloc_(void **p, ::size_t size, unsigned int node, const char *file, int line);
}
extern "C"
{
  extern ::nanos_err_t nanos_cmalloc(void **p, ::size_t size, unsigned int node, const char *file, int line);
}
extern "C"
{
  extern ::nanos_err_t nanos_cmalloc_2dim_distributed_(void **p, ::size_t rows, ::size_t cols, ::size_t elem_size, unsigned int start_node, ::size_t num_nodes, const char *file, int line);
}
extern "C"
{
  extern ::nanos_err_t nanos_cmalloc_2dim_distributed(void **p, ::size_t rows, ::size_t cols, ::size_t elem_size, unsigned int start_node, ::size_t num_nodes, const char *file, int line);
}
extern "C"
{
  extern ::nanos_err_t nanos_stick_to_producer_(void *p, ::size_t size);
}
extern "C"
{
  extern ::nanos_err_t nanos_stick_to_producer(void *p, ::size_t size);
}
extern "C"
{
  extern ::nanos_err_t nanos_free_(void *p);
}
extern "C"
{
  extern ::nanos_err_t nanos_free(void *p);
}
extern "C"
{
  extern void nanos_free0_(void *p);
}
extern "C"
{
  extern void nanos_free0(void *p);
}
extern "C"
{
  extern void nanos_handle_error_(::nanos_err_t err);
}
extern "C"
{
  extern void nanos_handle_error(::nanos_err_t err);
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_register_key_(::nanos_event_key_t *event_key, const char *key, const char *description, bool abort_when_registered);
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_register_key(::nanos_event_key_t *event_key, const char *key, const char *description, bool abort_when_registered);
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_register_key_with_key_(::nanos_event_key_t event_key, const char *key, const char *description, bool abort_when_registered);
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_register_key_with_key(::nanos_event_key_t event_key, const char *key, const char *description, bool abort_when_registered);
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_register_value_(::nanos_event_value_t *event_value, const char *key, const char *value, const char *description, bool abort_when_registered);
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_register_value(::nanos_event_value_t *event_value, const char *key, const char *value, const char *description, bool abort_when_registered);
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_register_value_with_val_(::nanos_event_value_t val, const char *key, const char *value, const char *description, bool abort_when_registered);
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_register_value_with_val(::nanos_event_value_t val, const char *key, const char *value, const char *description, bool abort_when_registered);
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_get_key_(const char *key, ::nanos_event_key_t *event_key);
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_get_key(const char *key, ::nanos_event_key_t *event_key);
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_get_value_(const char *key, const char *value, ::nanos_event_value_t *event_value);
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_get_value(const char *key, const char *value, ::nanos_event_value_t *event_value);
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_events_(unsigned int num_events, ::nanos_event_t events[]);
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_events(unsigned int num_events, ::nanos_event_t events[]);
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_close_user_fun_event_();
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_close_user_fun_event();
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_raise_gpu_kernel_launch_event_();
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_raise_gpu_kernel_launch_event();
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_close_gpu_kernel_launch_event_();
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_close_gpu_kernel_launch_event();
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_enable_();
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_enable();
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_disable_();
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_disable();
}
extern "C"
{
  extern ::nanos_err_t nanos_get_node_num_(unsigned int *num);
}
extern "C"
{
  extern ::nanos_err_t nanos_get_node_num(unsigned int *num);
}
extern "C"
{
  extern int nanos_get_num_nodes_();
}
extern "C"
{
  extern int nanos_get_num_nodes();
}
extern "C"
{
  extern ::nanos_err_t nanos_set_create_local_tasks_(bool value);
}
extern "C"
{
  extern ::nanos_err_t nanos_set_create_local_tasks(bool value);
}
typedef const char *nanos_string_t;
extern "C"
{
  extern ::nanos_err_t nanos_instrument_begin_burst_(::nanos_string_t key, ::nanos_string_t key_descr, ::nanos_string_t value, ::nanos_string_t value_descr);
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_begin_burst(::nanos_string_t key, ::nanos_string_t key_descr, ::nanos_string_t value, ::nanos_string_t value_descr);
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_end_burst_(::nanos_string_t key, ::nanos_string_t value);
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_end_burst(::nanos_string_t key, ::nanos_string_t value);
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_begin_burst_with_val_(::nanos_string_t key, ::nanos_string_t key_descr, ::nanos_event_value_t *val);
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_begin_burst_with_val(::nanos_string_t key, ::nanos_string_t key_descr, ::nanos_event_value_t *val);
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_end_burst_with_val_(::nanos_string_t key, ::nanos_event_value_t *val);
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_end_burst_with_val(::nanos_string_t key, ::nanos_event_value_t *val);
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_burst_begin_(::nanos_event_key_t key, ::nanos_event_value_t value);
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_burst_begin(::nanos_event_key_t key, ::nanos_event_value_t value);
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_burst_end_(::nanos_event_key_t key, ::nanos_event_value_t value);
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_burst_end(::nanos_event_key_t key, ::nanos_event_value_t value);
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_point_event_(::nanos_event_key_t key, ::nanos_event_value_t value);
}
extern "C"
{
  extern ::nanos_err_t nanos_instrument_point_event(::nanos_event_key_t key, ::nanos_event_value_t value);
}
extern "C"
{
  extern ::nanos_err_t nanos_memcpy_(void *dest, const void *src, ::size_t n);
}
extern "C"
{
  extern ::nanos_err_t nanos_memcpy(void *dest, const void *src, ::size_t n);
}
extern "C"
{
  extern ::nanos_err_t nanos_register_object_(int num_objects, ::nanos_copy_data_t *obj);
}
extern "C"
{
  extern ::nanos_err_t nanos_register_object(int num_objects, ::nanos_copy_data_t *obj);
}
extern "C"
{
  extern ::nanos_err_t nanos_unregister_object_(int num_objects, void *base_addresses);
}
extern "C"
{
  extern ::nanos_err_t nanos_unregister_object(int num_objects, void *base_addresses);
}
extern "C"
{
  extern const char *nanos_get_default_scheduler_();
}
extern "C"
{
  extern const char *nanos_get_default_scheduler();
}
extern "C"
{
  extern ::nanos_err_t nanos_start_scheduler_();
}
extern "C"
{
  extern ::nanos_err_t nanos_start_scheduler();
}
extern "C"
{
  extern ::nanos_err_t nanos_stop_scheduler_();
}
extern "C"
{
  extern ::nanos_err_t nanos_stop_scheduler();
}
extern "C"
{
  extern ::nanos_err_t nanos_scheduler_enabled_(bool *res);
}
extern "C"
{
  extern ::nanos_err_t nanos_scheduler_enabled(bool *res);
}
extern "C"
{
  extern ::nanos_err_t nanos_wait_until_threads_paused_();
}
extern "C"
{
  extern ::nanos_err_t nanos_wait_until_threads_paused();
}
extern "C"
{
  extern ::nanos_err_t nanos_wait_until_threads_unpaused_();
}
extern "C"
{
  extern ::nanos_err_t nanos_wait_until_threads_unpaused();
}
extern "C"
{
  extern ::nanos_err_t nanos_scheduler_get_stealing_(bool *res);
}
extern "C"
{
  extern ::nanos_err_t nanos_scheduler_get_stealing(bool *res);
}
extern "C"
{
  extern ::nanos_err_t nanos_scheduler_set_stealing_(bool value);
}
extern "C"
{
  extern ::nanos_err_t nanos_scheduler_set_stealing(bool value);
}
extern "C"
{
  extern void ompss_nanox_main_begin_(void *addr, const char *filename, int line);
}
extern "C"
{
  extern void ompss_nanox_main_begin(void *addr, const char *filename, int line);
}
extern "C"
{
  extern void ompss_nanox_main_end_();
}
extern "C"
{
  extern void ompss_nanox_main_end();
}
extern "C"
{
  extern void ompss_nanox_main_();
}
extern "C"
{
  extern void ompss_nanox_main();
}
extern "C"
{
  extern void nanos_atexit_(void *);
}
extern "C"
{
  extern void nanos_atexit(void *);
}
extern "C"
{
  void nanos_reduction_int_vop(int, void *, void *);
}
extern "C"
{
  extern int nanos_cmpi_init_(int *argc, char **argv[]);
}
extern "C"
{
  extern int nanos_cmpi_init(int *argc, char **argv[]);
}
extern "C"
{
  extern void nanos_cmpi_finalize_();
}
extern "C"
{
  extern void nanos_cmpi_finalize();
}
extern "C"
{
  extern void nanos_into_blocking_mpi_call_();
}
extern "C"
{
  extern void nanos_into_blocking_mpi_call();
}
extern "C"
{
  extern void nanos_out_of_blocking_mpi_call_();
}
extern "C"
{
  extern void nanos_out_of_blocking_mpi_call();
}
extern "C"
{
  extern void nanos_thread_print_(char *str);
}
extern "C"
{
  extern void nanos_thread_print(char *str);
}
extern "C"
{
  extern void nanos_set_watch_addr_(void *addr);
}
extern "C"
{
  extern void nanos_set_watch_addr(void *addr);
}
extern "C"
{
  extern void nanos_print_bt_();
}
extern "C"
{
  extern void nanos_print_bt();
}
extern "C"
{
  extern void nanos_enable_verbose_copies_();
}
extern "C"
{
  extern void nanos_enable_verbose_copies();
}
extern "C"
{
  extern void nanos_disable_verbose_copies_();
}
extern "C"
{
  extern void nanos_disable_verbose_copies();
}
extern "C"
{
  extern void nanos_atomic_assig_schar_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_assig_schar(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_assig_short_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_assig_short(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_assig_int_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_assig_int(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_assig_long_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_assig_long(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_assig_longlong_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_assig_longlong(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_assig_uchar_(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_assig_uchar(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_assig_ushort_(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_assig_ushort(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_assig_uint_(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_assig_uint(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_assig_ulong_(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_assig_ulong(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_assig_ulonglong_(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_assig_ulonglong(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_assig_bytebool_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_assig_bytebool(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_assig_shortbool_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_assig_shortbool(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_assig_intbool_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_assig_intbool(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_assig_longbool_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_assig_longbool(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_assig_longlongbool_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_assig_longlongbool(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_assig_float_(volatile float *, float);
}
extern "C"
{
  extern void nanos_atomic_assig_float(volatile float *, float);
}
extern "C"
{
  extern void nanos_atomic_assig_double_(volatile double *, double);
}
extern "C"
{
  extern void nanos_atomic_assig_double(volatile double *, double);
}
extern "C"
{
  extern void nanos_atomic_assig_ldouble_(volatile long double *, long double);
}
extern "C"
{
  extern void nanos_atomic_assig_ldouble(volatile long double *, long double);
}
extern "C"
{
  extern void nanos_atomic_assig_cfloat_(volatile _Complex float *, _Complex float);
}
extern "C"
{
  extern void nanos_atomic_assig_cfloat(volatile _Complex float *, _Complex float);
}
extern "C"
{
  extern void nanos_atomic_assig_cdouble_(volatile _Complex double *, _Complex double);
}
extern "C"
{
  extern void nanos_atomic_assig_cdouble(volatile _Complex double *, _Complex double);
}
extern "C"
{
  extern void nanos_atomic_assig_cldouble_(volatile _Complex long double *, _Complex long double);
}
extern "C"
{
  extern void nanos_atomic_assig_cldouble(volatile _Complex long double *, _Complex long double);
}
extern "C"
{
  extern void nanos_atomic_add_schar_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_add_schar(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_add_short_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_add_short(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_add_int_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_add_int(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_add_long_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_add_long(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_add_longlong_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_add_longlong(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_add_uchar_(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_add_uchar(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_add_ushort_(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_add_ushort(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_add_uint_(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_add_uint(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_add_ulong_(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_add_ulong(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_add_ulonglong_(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_add_ulonglong(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_add_bytebool_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_add_bytebool(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_add_shortbool_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_add_shortbool(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_add_intbool_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_add_intbool(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_add_longbool_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_add_longbool(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_add_longlongbool_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_add_longlongbool(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_add_float_(volatile float *, float);
}
extern "C"
{
  extern void nanos_atomic_add_float(volatile float *, float);
}
extern "C"
{
  extern void nanos_atomic_add_double_(volatile double *, double);
}
extern "C"
{
  extern void nanos_atomic_add_double(volatile double *, double);
}
extern "C"
{
  extern void nanos_atomic_add_ldouble_(volatile long double *, long double);
}
extern "C"
{
  extern void nanos_atomic_add_ldouble(volatile long double *, long double);
}
extern "C"
{
  extern void nanos_atomic_add_cfloat_(volatile _Complex float *, _Complex float);
}
extern "C"
{
  extern void nanos_atomic_add_cfloat(volatile _Complex float *, _Complex float);
}
extern "C"
{
  extern void nanos_atomic_add_cdouble_(volatile _Complex double *, _Complex double);
}
extern "C"
{
  extern void nanos_atomic_add_cdouble(volatile _Complex double *, _Complex double);
}
extern "C"
{
  extern void nanos_atomic_add_cldouble_(volatile _Complex long double *, _Complex long double);
}
extern "C"
{
  extern void nanos_atomic_add_cldouble(volatile _Complex long double *, _Complex long double);
}
extern "C"
{
  extern void nanos_atomic_sub_schar_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_sub_schar(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_sub_short_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_sub_short(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_sub_int_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_sub_int(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_sub_long_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_sub_long(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_sub_longlong_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_sub_longlong(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_sub_uchar_(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_sub_uchar(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_sub_ushort_(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_sub_ushort(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_sub_uint_(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_sub_uint(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_sub_ulong_(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_sub_ulong(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_sub_ulonglong_(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_sub_ulonglong(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_sub_bytebool_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_sub_bytebool(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_sub_shortbool_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_sub_shortbool(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_sub_intbool_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_sub_intbool(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_sub_longbool_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_sub_longbool(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_sub_longlongbool_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_sub_longlongbool(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_sub_float_(volatile float *, float);
}
extern "C"
{
  extern void nanos_atomic_sub_float(volatile float *, float);
}
extern "C"
{
  extern void nanos_atomic_sub_double_(volatile double *, double);
}
extern "C"
{
  extern void nanos_atomic_sub_double(volatile double *, double);
}
extern "C"
{
  extern void nanos_atomic_sub_ldouble_(volatile long double *, long double);
}
extern "C"
{
  extern void nanos_atomic_sub_ldouble(volatile long double *, long double);
}
extern "C"
{
  extern void nanos_atomic_sub_cfloat_(volatile _Complex float *, _Complex float);
}
extern "C"
{
  extern void nanos_atomic_sub_cfloat(volatile _Complex float *, _Complex float);
}
extern "C"
{
  extern void nanos_atomic_sub_cdouble_(volatile _Complex double *, _Complex double);
}
extern "C"
{
  extern void nanos_atomic_sub_cdouble(volatile _Complex double *, _Complex double);
}
extern "C"
{
  extern void nanos_atomic_sub_cldouble_(volatile _Complex long double *, _Complex long double);
}
extern "C"
{
  extern void nanos_atomic_sub_cldouble(volatile _Complex long double *, _Complex long double);
}
extern "C"
{
  extern void nanos_atomic_mul_schar_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_mul_schar(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_mul_short_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_mul_short(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_mul_int_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_mul_int(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_mul_long_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_mul_long(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_mul_longlong_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_mul_longlong(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_mul_uchar_(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_mul_uchar(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_mul_ushort_(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_mul_ushort(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_mul_uint_(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_mul_uint(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_mul_ulong_(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_mul_ulong(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_mul_ulonglong_(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_mul_ulonglong(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_mul_bytebool_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_mul_bytebool(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_mul_shortbool_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_mul_shortbool(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_mul_intbool_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_mul_intbool(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_mul_longbool_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_mul_longbool(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_mul_longlongbool_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_mul_longlongbool(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_mul_float_(volatile float *, float);
}
extern "C"
{
  extern void nanos_atomic_mul_float(volatile float *, float);
}
extern "C"
{
  extern void nanos_atomic_mul_double_(volatile double *, double);
}
extern "C"
{
  extern void nanos_atomic_mul_double(volatile double *, double);
}
extern "C"
{
  extern void nanos_atomic_mul_ldouble_(volatile long double *, long double);
}
extern "C"
{
  extern void nanos_atomic_mul_ldouble(volatile long double *, long double);
}
extern "C"
{
  extern void nanos_atomic_mul_cfloat_(volatile _Complex float *, _Complex float);
}
extern "C"
{
  extern void nanos_atomic_mul_cfloat(volatile _Complex float *, _Complex float);
}
extern "C"
{
  extern void nanos_atomic_mul_cdouble_(volatile _Complex double *, _Complex double);
}
extern "C"
{
  extern void nanos_atomic_mul_cdouble(volatile _Complex double *, _Complex double);
}
extern "C"
{
  extern void nanos_atomic_mul_cldouble_(volatile _Complex long double *, _Complex long double);
}
extern "C"
{
  extern void nanos_atomic_mul_cldouble(volatile _Complex long double *, _Complex long double);
}
extern "C"
{
  extern void nanos_atomic_div_schar_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_div_schar(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_div_short_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_div_short(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_div_int_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_div_int(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_div_long_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_div_long(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_div_longlong_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_div_longlong(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_div_uchar_(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_div_uchar(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_div_ushort_(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_div_ushort(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_div_uint_(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_div_uint(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_div_ulong_(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_div_ulong(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_div_ulonglong_(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_div_ulonglong(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_div_bytebool_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_div_bytebool(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_div_shortbool_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_div_shortbool(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_div_intbool_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_div_intbool(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_div_longbool_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_div_longbool(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_div_longlongbool_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_div_longlongbool(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_div_float_(volatile float *, float);
}
extern "C"
{
  extern void nanos_atomic_div_float(volatile float *, float);
}
extern "C"
{
  extern void nanos_atomic_div_double_(volatile double *, double);
}
extern "C"
{
  extern void nanos_atomic_div_double(volatile double *, double);
}
extern "C"
{
  extern void nanos_atomic_div_ldouble_(volatile long double *, long double);
}
extern "C"
{
  extern void nanos_atomic_div_ldouble(volatile long double *, long double);
}
extern "C"
{
  extern void nanos_atomic_div_cfloat_(volatile _Complex float *, _Complex float);
}
extern "C"
{
  extern void nanos_atomic_div_cfloat(volatile _Complex float *, _Complex float);
}
extern "C"
{
  extern void nanos_atomic_div_cdouble_(volatile _Complex double *, _Complex double);
}
extern "C"
{
  extern void nanos_atomic_div_cdouble(volatile _Complex double *, _Complex double);
}
extern "C"
{
  extern void nanos_atomic_div_cldouble_(volatile _Complex long double *, _Complex long double);
}
extern "C"
{
  extern void nanos_atomic_div_cldouble(volatile _Complex long double *, _Complex long double);
}
extern "C"
{
  extern void nanos_atomic_pow_schar_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_pow_schar(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_pow_short_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_pow_short(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_pow_int_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_pow_int(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_pow_long_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_pow_long(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_pow_longlong_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_pow_longlong(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_pow_uchar_(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_pow_uchar(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_pow_ushort_(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_pow_ushort(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_pow_uint_(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_pow_uint(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_pow_ulong_(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_pow_ulong(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_pow_ulonglong_(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_pow_ulonglong(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_pow_bytebool_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_pow_bytebool(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_pow_shortbool_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_pow_shortbool(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_pow_intbool_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_pow_intbool(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_pow_longbool_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_pow_longbool(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_pow_longlongbool_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_pow_longlongbool(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_pow_float_(volatile float *, float);
}
extern "C"
{
  extern void nanos_atomic_pow_float(volatile float *, float);
}
extern "C"
{
  extern void nanos_atomic_pow_double_(volatile double *, double);
}
extern "C"
{
  extern void nanos_atomic_pow_double(volatile double *, double);
}
extern "C"
{
  extern void nanos_atomic_pow_ldouble_(volatile long double *, long double);
}
extern "C"
{
  extern void nanos_atomic_pow_ldouble(volatile long double *, long double);
}
extern "C"
{
  extern void nanos_atomic_pow_cfloat_(volatile _Complex float *, _Complex float);
}
extern "C"
{
  extern void nanos_atomic_pow_cfloat(volatile _Complex float *, _Complex float);
}
extern "C"
{
  extern void nanos_atomic_pow_cdouble_(volatile _Complex double *, _Complex double);
}
extern "C"
{
  extern void nanos_atomic_pow_cdouble(volatile _Complex double *, _Complex double);
}
extern "C"
{
  extern void nanos_atomic_pow_cldouble_(volatile _Complex long double *, _Complex long double);
}
extern "C"
{
  extern void nanos_atomic_pow_cldouble(volatile _Complex long double *, _Complex long double);
}
extern "C"
{
  extern void nanos_atomic_max_schar_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_max_schar(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_max_short_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_max_short(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_max_int_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_max_int(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_max_long_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_max_long(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_max_longlong_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_max_longlong(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_max_uchar_(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_max_uchar(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_max_ushort_(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_max_ushort(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_max_uint_(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_max_uint(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_max_ulong_(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_max_ulong(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_max_ulonglong_(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_max_ulonglong(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_max_bytebool_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_max_bytebool(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_max_shortbool_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_max_shortbool(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_max_intbool_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_max_intbool(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_max_longbool_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_max_longbool(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_max_longlongbool_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_max_longlongbool(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_min_schar_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_min_schar(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_min_short_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_min_short(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_min_int_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_min_int(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_min_long_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_min_long(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_min_longlong_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_min_longlong(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_min_uchar_(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_min_uchar(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_min_ushort_(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_min_ushort(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_min_uint_(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_min_uint(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_min_ulong_(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_min_ulong(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_min_ulonglong_(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_min_ulonglong(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_min_bytebool_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_min_bytebool(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_min_shortbool_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_min_shortbool(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_min_intbool_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_min_intbool(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_min_longbool_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_min_longbool(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_min_longlongbool_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_min_longlongbool(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_max_float_(volatile float *, float);
}
extern "C"
{
  extern void nanos_atomic_max_float(volatile float *, float);
}
extern "C"
{
  extern void nanos_atomic_max_double_(volatile double *, double);
}
extern "C"
{
  extern void nanos_atomic_max_double(volatile double *, double);
}
extern "C"
{
  extern void nanos_atomic_max_ldouble_(volatile long double *, long double);
}
extern "C"
{
  extern void nanos_atomic_max_ldouble(volatile long double *, long double);
}
extern "C"
{
  extern void nanos_atomic_min_float_(volatile float *, float);
}
extern "C"
{
  extern void nanos_atomic_min_float(volatile float *, float);
}
extern "C"
{
  extern void nanos_atomic_min_double_(volatile double *, double);
}
extern "C"
{
  extern void nanos_atomic_min_double(volatile double *, double);
}
extern "C"
{
  extern void nanos_atomic_min_ldouble_(volatile long double *, long double);
}
extern "C"
{
  extern void nanos_atomic_min_ldouble(volatile long double *, long double);
}
extern "C"
{
  extern void nanos_atomic_eq_schar_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_eq_schar(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_eq_short_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_eq_short(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_eq_int_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_eq_int(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_eq_long_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_eq_long(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_eq_longlong_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_eq_longlong(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_eq_uchar_(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_eq_uchar(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_eq_ushort_(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_eq_ushort(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_eq_uint_(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_eq_uint(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_eq_ulong_(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_eq_ulong(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_eq_ulonglong_(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_eq_ulonglong(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_eq_bytebool_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_eq_bytebool(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_eq_shortbool_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_eq_shortbool(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_eq_intbool_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_eq_intbool(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_eq_longbool_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_eq_longbool(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_eq_longlongbool_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_eq_longlongbool(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_eq_float_(volatile float *, float);
}
extern "C"
{
  extern void nanos_atomic_eq_float(volatile float *, float);
}
extern "C"
{
  extern void nanos_atomic_eq_double_(volatile double *, double);
}
extern "C"
{
  extern void nanos_atomic_eq_double(volatile double *, double);
}
extern "C"
{
  extern void nanos_atomic_eq_ldouble_(volatile long double *, long double);
}
extern "C"
{
  extern void nanos_atomic_eq_ldouble(volatile long double *, long double);
}
extern "C"
{
  extern void nanos_atomic_eq_cfloat_(volatile _Complex float *, _Complex float);
}
extern "C"
{
  extern void nanos_atomic_eq_cfloat(volatile _Complex float *, _Complex float);
}
extern "C"
{
  extern void nanos_atomic_eq_cdouble_(volatile _Complex double *, _Complex double);
}
extern "C"
{
  extern void nanos_atomic_eq_cdouble(volatile _Complex double *, _Complex double);
}
extern "C"
{
  extern void nanos_atomic_eq_cldouble_(volatile _Complex long double *, _Complex long double);
}
extern "C"
{
  extern void nanos_atomic_eq_cldouble(volatile _Complex long double *, _Complex long double);
}
extern "C"
{
  extern void nanos_atomic_neq_schar_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_neq_schar(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_neq_short_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_neq_short(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_neq_int_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_neq_int(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_neq_long_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_neq_long(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_neq_longlong_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_neq_longlong(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_neq_uchar_(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_neq_uchar(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_neq_ushort_(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_neq_ushort(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_neq_uint_(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_neq_uint(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_neq_ulong_(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_neq_ulong(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_neq_ulonglong_(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_neq_ulonglong(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_neq_bytebool_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_neq_bytebool(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_neq_shortbool_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_neq_shortbool(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_neq_intbool_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_neq_intbool(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_neq_longbool_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_neq_longbool(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_neq_longlongbool_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_neq_longlongbool(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_neq_float_(volatile float *, float);
}
extern "C"
{
  extern void nanos_atomic_neq_float(volatile float *, float);
}
extern "C"
{
  extern void nanos_atomic_neq_double_(volatile double *, double);
}
extern "C"
{
  extern void nanos_atomic_neq_double(volatile double *, double);
}
extern "C"
{
  extern void nanos_atomic_neq_ldouble_(volatile long double *, long double);
}
extern "C"
{
  extern void nanos_atomic_neq_ldouble(volatile long double *, long double);
}
extern "C"
{
  extern void nanos_atomic_neq_cfloat_(volatile _Complex float *, _Complex float);
}
extern "C"
{
  extern void nanos_atomic_neq_cfloat(volatile _Complex float *, _Complex float);
}
extern "C"
{
  extern void nanos_atomic_neq_cdouble_(volatile _Complex double *, _Complex double);
}
extern "C"
{
  extern void nanos_atomic_neq_cdouble(volatile _Complex double *, _Complex double);
}
extern "C"
{
  extern void nanos_atomic_neq_cldouble_(volatile _Complex long double *, _Complex long double);
}
extern "C"
{
  extern void nanos_atomic_neq_cldouble(volatile _Complex long double *, _Complex long double);
}
extern "C"
{
  extern void nanos_atomic_mod_schar_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_mod_schar(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_mod_short_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_mod_short(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_mod_int_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_mod_int(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_mod_long_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_mod_long(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_mod_longlong_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_mod_longlong(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_mod_uchar_(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_mod_uchar(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_mod_ushort_(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_mod_ushort(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_mod_uint_(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_mod_uint(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_mod_ulong_(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_mod_ulong(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_mod_ulonglong_(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_mod_ulonglong(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_mod_bytebool_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_mod_bytebool(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_mod_shortbool_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_mod_shortbool(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_mod_intbool_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_mod_intbool(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_mod_longbool_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_mod_longbool(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_mod_longlongbool_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_mod_longlongbool(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_shl_schar_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_shl_schar(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_shl_short_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_shl_short(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_shl_int_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_shl_int(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_shl_long_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_shl_long(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_shl_longlong_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_shl_longlong(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_shl_uchar_(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_shl_uchar(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_shl_ushort_(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_shl_ushort(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_shl_uint_(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_shl_uint(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_shl_ulong_(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_shl_ulong(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_shl_ulonglong_(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_shl_ulonglong(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_shl_bytebool_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_shl_bytebool(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_shl_shortbool_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_shl_shortbool(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_shl_intbool_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_shl_intbool(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_shl_longbool_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_shl_longbool(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_shl_longlongbool_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_shl_longlongbool(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_shr_schar_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_shr_schar(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_shr_short_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_shr_short(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_shr_int_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_shr_int(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_shr_long_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_shr_long(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_shr_longlong_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_shr_longlong(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_shr_uchar_(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_shr_uchar(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_shr_ushort_(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_shr_ushort(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_shr_uint_(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_shr_uint(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_shr_ulong_(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_shr_ulong(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_shr_ulonglong_(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_shr_ulonglong(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_shr_bytebool_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_shr_bytebool(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_shr_shortbool_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_shr_shortbool(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_shr_intbool_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_shr_intbool(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_shr_longbool_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_shr_longbool(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_shr_longlongbool_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_shr_longlongbool(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_land_schar_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_land_schar(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_land_short_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_land_short(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_land_int_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_land_int(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_land_long_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_land_long(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_land_longlong_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_land_longlong(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_land_uchar_(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_land_uchar(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_land_ushort_(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_land_ushort(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_land_uint_(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_land_uint(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_land_ulong_(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_land_ulong(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_land_ulonglong_(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_land_ulonglong(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_land_bytebool_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_land_bytebool(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_land_shortbool_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_land_shortbool(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_land_intbool_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_land_intbool(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_land_longbool_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_land_longbool(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_land_longlongbool_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_land_longlongbool(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_lor_schar_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_lor_schar(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_lor_short_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_lor_short(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_lor_int_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_lor_int(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_lor_long_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_lor_long(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_lor_longlong_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_lor_longlong(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_lor_uchar_(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_lor_uchar(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_lor_ushort_(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_lor_ushort(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_lor_uint_(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_lor_uint(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_lor_ulong_(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_lor_ulong(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_lor_ulonglong_(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_lor_ulonglong(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_lor_bytebool_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_lor_bytebool(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_lor_shortbool_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_lor_shortbool(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_lor_intbool_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_lor_intbool(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_lor_longbool_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_lor_longbool(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_lor_longlongbool_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_lor_longlongbool(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_band_schar_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_band_schar(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_band_short_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_band_short(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_band_int_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_band_int(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_band_long_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_band_long(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_band_longlong_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_band_longlong(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_band_uchar_(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_band_uchar(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_band_ushort_(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_band_ushort(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_band_uint_(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_band_uint(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_band_ulong_(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_band_ulong(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_band_ulonglong_(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_band_ulonglong(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_band_bytebool_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_band_bytebool(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_band_shortbool_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_band_shortbool(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_band_intbool_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_band_intbool(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_band_longbool_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_band_longbool(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_band_longlongbool_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_band_longlongbool(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_bor_schar_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_bor_schar(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_bor_short_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_bor_short(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_bor_int_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_bor_int(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_bor_long_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_bor_long(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_bor_longlong_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_bor_longlong(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_bor_uchar_(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_bor_uchar(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_bor_ushort_(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_bor_ushort(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_bor_uint_(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_bor_uint(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_bor_ulong_(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_bor_ulong(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_bor_ulonglong_(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_bor_ulonglong(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_bor_bytebool_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_bor_bytebool(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_bor_shortbool_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_bor_shortbool(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_bor_intbool_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_bor_intbool(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_bor_longbool_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_bor_longbool(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_bor_longlongbool_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_bor_longlongbool(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_bxor_schar_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_bxor_schar(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_bxor_short_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_bxor_short(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_bxor_int_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_bxor_int(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_bxor_long_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_bxor_long(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_bxor_longlong_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_bxor_longlong(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_bxor_uchar_(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_bxor_uchar(volatile unsigned char *, unsigned char);
}
extern "C"
{
  extern void nanos_atomic_bxor_ushort_(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_bxor_ushort(volatile unsigned short int *, unsigned short int);
}
extern "C"
{
  extern void nanos_atomic_bxor_uint_(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_bxor_uint(volatile unsigned int *, unsigned int);
}
extern "C"
{
  extern void nanos_atomic_bxor_ulong_(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_bxor_ulong(volatile unsigned long int *, unsigned long int);
}
extern "C"
{
  extern void nanos_atomic_bxor_ulonglong_(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_bxor_ulonglong(volatile unsigned long long int *, unsigned long long int);
}
extern "C"
{
  extern void nanos_atomic_bxor_bytebool_(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_bxor_bytebool(volatile signed char *, signed char);
}
extern "C"
{
  extern void nanos_atomic_bxor_shortbool_(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_bxor_shortbool(volatile short int *, short int);
}
extern "C"
{
  extern void nanos_atomic_bxor_intbool_(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_bxor_intbool(volatile int *, int);
}
extern "C"
{
  extern void nanos_atomic_bxor_longbool_(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_bxor_longbool(volatile long int *, long int);
}
extern "C"
{
  extern void nanos_atomic_bxor_longlongbool_(volatile long long int *, long long int);
}
extern "C"
{
  extern void nanos_atomic_bxor_longlongbool(volatile long long int *, long long int);
}
enum nanos_omp_sched_t
{
  nanos_omp_sched_static = 1,
  nanos_omp_sched_dynamic = 2,
  nanos_omp_sched_guided = 3,
  nanos_omp_sched_auto = 4
};
extern "C"
{
  extern ::nanos_err_t nanos_omp_single_(bool *);
}
extern "C"
{
  extern ::nanos_err_t nanos_omp_single(bool *);
}
extern "C"
{
  extern ::nanos_err_t nanos_omp_barrier_();
}
extern "C"
{
  extern ::nanos_err_t nanos_omp_barrier();
}
extern "C"
{
  extern ::nanos_err_t nanos_omp_set_implicit_(::nanos_wd_t uwd);
}
extern "C"
{
  extern ::nanos_err_t nanos_omp_set_implicit(::nanos_wd_t uwd);
}
extern "C"
{
  extern int nanos_omp_get_max_threads_();
}
extern "C"
{
  extern int nanos_omp_get_max_threads();
}
extern "C"
{
  extern int nanos_omp_get_num_threads_();
}
extern "C"
{
  extern int nanos_omp_get_num_threads();
}
extern "C"
{
  extern int nanos_omp_get_thread_num_();
}
extern "C"
{
  extern int nanos_omp_get_thread_num();
}
extern "C"
{
  extern int nanos_omp_set_num_threads_(int nthreads);
}
extern "C"
{
  extern int nanos_omp_set_num_threads(int nthreads);
}
extern "C"
{
  extern ::nanos_ws_t nanos_omp_find_worksharing_(::nanos_omp_sched_t kind);
}
extern "C"
{
  extern ::nanos_ws_t nanos_omp_find_worksharing(::nanos_omp_sched_t kind);
}
extern "C"
{
  extern ::nanos_err_t nanos_omp_get_schedule_(::nanos_omp_sched_t *kind, int *modifier);
}
extern "C"
{
  extern ::nanos_err_t nanos_omp_get_schedule(::nanos_omp_sched_t *kind, int *modifier);
}
extern "C"
{
  extern int nanos_omp_get_num_threads_next_parallel_(int threads_requested);
}
extern "C"
{
  extern int nanos_omp_get_num_threads_next_parallel(int threads_requested);
}
extern "C"
{
  extern void nanos_omp_get_process_mask_(::nanos_cpu_set_t cpu_set);
}
extern "C"
{
  extern void nanos_omp_get_process_mask(::nanos_cpu_set_t cpu_set);
}
extern "C"
{
  extern int nanos_omp_set_process_mask_(::const_nanos_cpu_set_t cpu_set);
}
extern "C"
{
  extern int nanos_omp_set_process_mask(::const_nanos_cpu_set_t cpu_set);
}
extern "C"
{
  extern void nanos_omp_add_process_mask_(::const_nanos_cpu_set_t cpu_set);
}
extern "C"
{
  extern void nanos_omp_add_process_mask(::const_nanos_cpu_set_t cpu_set);
}
extern "C"
{
  extern void nanos_omp_get_active_mask_(::nanos_cpu_set_t cpu_set);
}
extern "C"
{
  extern void nanos_omp_get_active_mask(::nanos_cpu_set_t cpu_set);
}
extern "C"
{
  extern int nanos_omp_set_active_mask_(::const_nanos_cpu_set_t cpu_set);
}
extern "C"
{
  extern int nanos_omp_set_active_mask(::const_nanos_cpu_set_t cpu_set);
}
extern "C"
{
  extern void nanos_omp_add_active_mask_(::const_nanos_cpu_set_t cpu_set);
}
extern "C"
{
  extern void nanos_omp_add_active_mask(::const_nanos_cpu_set_t cpu_set);
}
extern "C"
{
  extern int nanos_omp_enable_cpu_(int cpuid);
}
extern "C"
{
  extern int nanos_omp_enable_cpu(int cpuid);
}
extern "C"
{
  extern int nanos_omp_disable_cpu_(int cpuid);
}
extern "C"
{
  extern int nanos_omp_disable_cpu(int cpuid);
}
extern "C"
{
  extern int nanos_omp_get_max_processors_();
}
extern "C"
{
  extern int nanos_omp_get_max_processors();
}
struct  nanos_fpga_args_t
{
    void (*outline)(void *);
    unsigned int type;
};
struct  nanos_find_fpga_args_t
{
    unsigned int type;
    bool check_free;
    bool lock_pe;
};
enum nanos_fpga_memcpy_kind_t
{
  NANOS_COPY_HOST_TO_FPGA = 0,
  NANOS_COPY_FPGA_TO_HOST = 1
};
extern "C"
{
  extern void *nanos_fpga_factory_(void *args);
}
extern "C"
{
  extern void *nanos_fpga_factory(void *args);
}
extern "C"
{
  extern void *nanos_fpga_alloc_dma_mem_(::size_t len);
}
extern "C"
{
  extern void *nanos_fpga_alloc_dma_mem(::size_t len);
}
extern "C"
{
  extern void nanos_fpga_free_dma_mem_(void *address);
}
extern "C"
{
  extern void nanos_fpga_free_dma_mem(void *address);
}
extern "C"
{
  extern ::nanos_err_t nanos_find_fpga_pe_(void *req, ::nanos_pe_t *pe);
}
extern "C"
{
  extern ::nanos_err_t nanos_find_fpga_pe(void *req, ::nanos_pe_t *pe);
}
extern "C"
{
  extern void *nanos_fpga_get_phy_address_(void *address);
}
extern "C"
{
  extern void *nanos_fpga_get_phy_address(void *address);
}
extern "C"
{
  extern ::nanos_err_t nanos_fpga_set_task_arg_(::nanos_wd_t wd, ::size_t argIdx, bool isInput, bool isOutput, ::uint64_t argValue);
}
extern "C"
{
  extern ::nanos_err_t nanos_fpga_set_task_arg(::nanos_wd_t wd, ::size_t argIdx, bool isInput, bool isOutput, ::uint64_t argValue);
}
extern "C"
{
  extern void *nanos_fpga_malloc_(::size_t len);
}
extern "C"
{
  extern void *nanos_fpga_malloc(::size_t len);
}
extern "C"
{
  extern void nanos_fpga_free_(void *fpgaPtr);
}
extern "C"
{
  extern void nanos_fpga_free(void *fpgaPtr);
}
extern "C"
{
  extern void nanos_fpga_memcpy_(void *fpgaPtr, void *hostPtr, ::size_t len, ::nanos_fpga_memcpy_kind_t kind);
}
extern "C"
{
  extern void nanos_fpga_memcpy(void *fpgaPtr, void *hostPtr, ::size_t len, ::nanos_fpga_memcpy_kind_t kind);
}
namespace std __attribute__((__visibility__("default"))) {
  typedef unsigned long int size_t;
  typedef long int ptrdiff_t;
  typedef decltype(nullptr) nullptr_t;
}
typedef unsigned char __u_char;
typedef unsigned short int __u_short;
typedef unsigned int __u_int;
typedef unsigned long int __u_long;
typedef signed char __int8_t;
typedef unsigned char __uint8_t;
typedef short int __int16_t;
typedef unsigned short int __uint16_t;
typedef int __int32_t;
typedef unsigned int __uint32_t;
typedef long int __int64_t;
typedef unsigned long int __uint64_t;
typedef long int __quad_t;
typedef unsigned long int __u_quad_t;
typedef unsigned long int __dev_t;
typedef unsigned int __uid_t;
typedef unsigned int __gid_t;
typedef unsigned long int __ino_t;
typedef unsigned long int __ino64_t;
typedef unsigned int __mode_t;
typedef unsigned int __nlink_t;
typedef long int __off_t;
typedef long int __off64_t;
typedef int __pid_t;
struct  __fsid_t
{
    int __val[2L];
};
typedef long int __clock_t;
typedef unsigned long int __rlim_t;
typedef unsigned long int __rlim64_t;
typedef unsigned int __id_t;
typedef long int __time_t;
typedef unsigned int __useconds_t;
typedef long int __suseconds_t;
typedef int __daddr_t;
typedef int __key_t;
typedef int __clockid_t;
typedef void *__timer_t;
typedef int __blksize_t;
typedef long int __blkcnt_t;
typedef long int __blkcnt64_t;
typedef unsigned long int __fsblkcnt_t;
typedef unsigned long int __fsblkcnt64_t;
typedef unsigned long int __fsfilcnt_t;
typedef unsigned long int __fsfilcnt64_t;
typedef long int __fsword_t;
typedef long int __ssize_t;
typedef long int __syscall_slong_t;
typedef unsigned long int __syscall_ulong_t;
typedef ::__off64_t __loff_t;
typedef ::__quad_t *__qaddr_t;
typedef char *__caddr_t;
typedef long int __intptr_t;
typedef unsigned int __socklen_t;
struct _IO_FILE;
typedef ::_IO_FILE FILE;
typedef ::_IO_FILE __FILE;
struct  __mbstate_t
{
    int __count;
    union  mcc_union_anon_24
    {
        unsigned int __wch;
        char __wchb[4L];
    };
    ::__mbstate_t::mcc_union_anon_24 __value;
};
struct  _G_fpos_t
{
    ::__off_t __pos;
    ::__mbstate_t __state;
};
struct  _G_fpos64_t
{
    ::__off64_t __pos;
    ::__mbstate_t __state;
};
typedef ::__builtin_va_list __gnuc_va_list;
struct _IO_jump_t;
typedef void _IO_lock_t;
struct _IO_marker;
struct  _IO_marker
{
    ::_IO_marker *_next;
    ::_IO_FILE *_sbuf;
    int _pos;
};
enum __codecvt_result
{
  __codecvt_ok = 0,
  __codecvt_partial = 1,
  __codecvt_error = 2,
  __codecvt_noconv = 3
};
struct  _IO_FILE
{
    int _flags;
    char *_IO_read_ptr;
    char *_IO_read_end;
    char *_IO_read_base;
    char *_IO_write_base;
    char *_IO_write_ptr;
    char *_IO_write_end;
    char *_IO_buf_base;
    char *_IO_buf_end;
    char *_IO_save_base;
    char *_IO_backup_base;
    char *_IO_save_end;
    ::_IO_marker *_markers;
    ::_IO_FILE *_chain;
    int _fileno;
    int _flags2;
    ::__off_t _old_offset;
    unsigned short int _cur_column;
    signed char _vtable_offset;
    char _shortbuf[1L];
    ::_IO_lock_t *_lock;
    ::__off64_t _offset;
    void *__pad1;
    void *__pad2;
    void *__pad3;
    void *__pad4;
    ::size_t __pad5;
    int _mode;
    char _unused2[20L];
};
struct _IO_FILE_plus;
extern "C"
{
  extern ::_IO_FILE_plus _IO_2_1_stdin_;
}
extern "C"
{
  extern ::_IO_FILE_plus _IO_2_1_stdout_;
}
extern "C"
{
  extern ::_IO_FILE_plus _IO_2_1_stderr_;
}
typedef ::__ssize_t __io_read_fn(void *, char *, ::size_t);
typedef ::__ssize_t __io_write_fn(void *, const char *, ::size_t);
typedef int __io_seek_fn(void *, ::__off64_t *, int);
typedef int __io_close_fn(void *);
typedef ::__io_read_fn cookie_read_function_t;
typedef ::__io_write_fn cookie_write_function_t;
typedef ::__io_seek_fn cookie_seek_function_t;
typedef ::__io_close_fn cookie_close_function_t;
struct  _IO_cookie_io_functions_t
{
    ::__io_read_fn (*read);
    ::__io_write_fn (*write);
    ::__io_seek_fn (*seek);
    ::__io_close_fn (*close);
};
typedef ::_IO_cookie_io_functions_t cookie_io_functions_t;
struct _IO_cookie_file;
extern "C"
{
  extern void _IO_cookie_init(::_IO_cookie_file *__cfile, int __read_write, void *__cookie, ::_IO_cookie_io_functions_t __fns);
}
extern "C"
{
  extern int __underflow(::_IO_FILE *);
}
extern "C"
{
  extern int __uflow(::_IO_FILE *);
}
extern "C"
{
  extern int __overflow(::_IO_FILE *, int);
}
extern "C"
{
  extern int _IO_getc(::_IO_FILE *__fp);
}
extern "C"
{
  extern int _IO_putc(int __c, ::_IO_FILE *__fp);
}
extern "C"
{
  extern int _IO_feof(::_IO_FILE *__fp) throw();
}
extern "C"
{
  extern int _IO_ferror(::_IO_FILE *__fp) throw();
}
extern "C"
{
  extern int _IO_peekc_locked(::_IO_FILE *__fp);
}
extern "C"
{
  extern void _IO_flockfile(::_IO_FILE *) throw();
}
extern "C"
{
  extern void _IO_funlockfile(::_IO_FILE *) throw();
}
extern "C"
{
  extern int _IO_ftrylockfile(::_IO_FILE *) throw();
}
extern "C"
{
  extern int _IO_vfscanf(::_IO_FILE *__restrict , const char *__restrict , ::__gnuc_va_list, int *__restrict );
}
extern "C"
{
  extern int _IO_vfprintf(::_IO_FILE *__restrict , const char *__restrict , ::__gnuc_va_list);
}
extern "C"
{
  extern ::__ssize_t _IO_padn(::_IO_FILE *, int, ::__ssize_t);
}
extern "C"
{
  extern ::size_t _IO_sgetn(::_IO_FILE *, void *, ::size_t);
}
extern "C"
{
  extern ::__off64_t _IO_seekoff(::_IO_FILE *, ::__off64_t, int, int);
}
extern "C"
{
  extern ::__off64_t _IO_seekpos(::_IO_FILE *, ::__off64_t, int);
}
extern "C"
{
  extern void _IO_free_backup_area(::_IO_FILE *) throw();
}
typedef ::__gnuc_va_list va_list;
typedef ::__off_t off_t;
typedef ::__off64_t off64_t;
typedef ::__ssize_t ssize_t;
typedef ::_G_fpos_t fpos_t;
typedef ::_G_fpos64_t fpos64_t;
extern "C"
{
  extern ::_IO_FILE *stdin;
}
extern "C"
{
  extern ::_IO_FILE *stdout;
}
extern "C"
{
  extern ::_IO_FILE *stderr;
}
extern "C"
{
  extern int remove(const char *__filename) throw();
}
extern "C"
{
  extern int rename(const char *__old, const char *__new) throw();
}
extern "C"
{
  extern int renameat(int __oldfd, const char *__old, int __newfd, const char *__new) throw();
}
extern "C"
{
  extern ::FILE *tmpfile();
}
extern "C"
{
  extern ::FILE *tmpfile64();
}
extern "C"
{
  extern char *tmpnam(char *__s) throw();
}
extern "C"
{
  extern char *tmpnam_r(char *__s) throw();
}
extern "C"
{
  extern char *tempnam(const char *__dir, const char *__pfx) throw() __attribute__((__malloc__));
}
extern "C"
{
  extern int fclose(::FILE *__stream);
}
extern "C"
{
  extern int fflush(::FILE *__stream);
}
extern "C"
{
  extern int fflush_unlocked(::FILE *__stream);
}
extern "C"
{
  extern int fcloseall();
}
extern "C"
{
  extern ::FILE *fopen(const char *__restrict __filename, const char *__restrict __modes);
}
extern "C"
{
  extern ::FILE *freopen(const char *__restrict __filename, const char *__restrict __modes, ::FILE *__restrict __stream);
}
extern "C"
{
  extern ::FILE *fopen64(const char *__restrict __filename, const char *__restrict __modes);
}
extern "C"
{
  extern ::FILE *freopen64(const char *__restrict __filename, const char *__restrict __modes, ::FILE *__restrict __stream);
}
extern "C"
{
  extern ::FILE *fdopen(int __fd, const char *__modes) throw();
}
extern "C"
{
  extern ::FILE *fopencookie(void *__restrict __magic_cookie, const char *__restrict __modes, ::_IO_cookie_io_functions_t __io_funcs) throw();
}
extern "C"
{
  extern ::FILE *fmemopen(void *__s, ::size_t __len, const char *__modes) throw();
}
extern "C"
{
  extern ::FILE *open_memstream(char **__bufloc, ::size_t *__sizeloc) throw();
}
extern "C"
{
  extern void setbuf(::FILE *__restrict __stream, char *__restrict __buf) throw();
}
extern "C"
{
  extern int setvbuf(::FILE *__restrict __stream, char *__restrict __buf, int __modes, ::size_t __n) throw();
}
extern "C"
{
  extern void setbuffer(::FILE *__restrict __stream, char *__restrict __buf, ::size_t __size) throw();
}
extern "C"
{
  extern void setlinebuf(::FILE *__stream) throw();
}
extern "C"
{
  extern int fprintf(::FILE *__restrict __stream, const char *__restrict __format, ...);
}
extern "C"
{
  extern int printf(const char *__restrict __format, ...);
}
extern "C"
{
  extern int sprintf(char *__restrict __s, const char *__restrict __format, ...) throw();
}
extern "C"
{
  extern int vfprintf(::FILE *__restrict __s, const char *__restrict __format, ::__gnuc_va_list __arg);
}
extern "C"
{
  extern inline int vprintf(const char *__restrict __fmt, ::__gnuc_va_list __arg) __attribute__((__gnu_inline__));
}
extern "C"
{
  extern int vsprintf(char *__restrict __s, const char *__restrict __format, ::__gnuc_va_list __arg) throw();
}
extern "C"
{
  extern int snprintf(char *__restrict __s, ::size_t __maxlen, const char *__restrict __format, ...) throw() __attribute__((__format__(__printf__, 3, 4)));
}
extern "C"
{
  extern int vsnprintf(char *__restrict __s, ::size_t __maxlen, const char *__restrict __format, ::__gnuc_va_list __arg) throw() __attribute__((__format__(__printf__, 3, 0)));
}
extern "C"
{
  extern int vasprintf(char **__restrict __ptr, const char *__restrict __f, ::__gnuc_va_list __arg) throw() __attribute__((__format__(__printf__, 2, 0)));
}
extern "C"
{
  extern int __asprintf(char **__restrict __ptr, const char *__restrict __fmt, ...) throw() __attribute__((__format__(__printf__, 2, 3)));
}
extern "C"
{
  extern int asprintf(char **__restrict __ptr, const char *__restrict __fmt, ...) throw() __attribute__((__format__(__printf__, 2, 3)));
}
extern "C"
{
  extern int vdprintf(int __fd, const char *__restrict __fmt, ::__gnuc_va_list __arg) __attribute__((__format__(__printf__, 2, 0)));
}
extern "C"
{
  extern int dprintf(int __fd, const char *__restrict __fmt, ...) __attribute__((__format__(__printf__, 2, 3)));
}
extern "C"
{
  extern int fscanf(::FILE *__restrict __stream, const char *__restrict __format, ...);
}
extern "C"
{
  extern int scanf(const char *__restrict __format, ...);
}
extern "C"
{
  extern int sscanf(const char *__restrict __s, const char *__restrict __format, ...) throw();
}
extern "C"
{
  extern int vfscanf(::FILE *__restrict __s, const char *__restrict __format, ::__gnuc_va_list __arg) __attribute__((__format__(__scanf__, 2, 0)));
}
extern "C"
{
  extern int vscanf(const char *__restrict __format, ::__gnuc_va_list __arg) __attribute__((__format__(__scanf__, 1, 0)));
}
extern "C"
{
  extern int vsscanf(const char *__restrict __s, const char *__restrict __format, ::__gnuc_va_list __arg) throw() __attribute__((__format__(__scanf__, 2, 0)));
}
extern "C"
{
  extern int fgetc(::FILE *__stream);
}
extern "C"
{
  extern int getc(::FILE *__stream);
}
extern "C"
{
  extern inline int getchar() __attribute__((__gnu_inline__));
}
extern "C"
{
  extern inline int getc_unlocked(::FILE *__fp) __attribute__((__gnu_inline__));
}
extern "C"
{
  extern inline int getchar_unlocked() __attribute__((__gnu_inline__));
}
extern "C"
{
  extern inline int fgetc_unlocked(::FILE *__fp) __attribute__((__gnu_inline__));
}
extern "C"
{
  extern int fputc(int __c, ::FILE *__stream);
}
extern "C"
{
  extern int putc(int __c, ::FILE *__stream);
}
extern "C"
{
  extern inline int putchar(int __c) __attribute__((__gnu_inline__));
}
extern "C"
{
  extern inline int fputc_unlocked(int __c, ::FILE *__stream) __attribute__((__gnu_inline__));
}
extern "C"
{
  extern inline int putc_unlocked(int __c, ::FILE *__stream) __attribute__((__gnu_inline__));
}
extern "C"
{
  extern inline int putchar_unlocked(int __c) __attribute__((__gnu_inline__));
}
extern "C"
{
  extern int getw(::FILE *__stream);
}
extern "C"
{
  extern int putw(int __w, ::FILE *__stream);
}
extern "C"
{
  extern char *fgets(char *__restrict __s, int __n, ::FILE *__restrict __stream);
}
extern "C"
{
  extern char *gets(char *__s) __attribute__((__deprecated__));
}
extern "C"
{
  extern char *fgets_unlocked(char *__restrict __s, int __n, ::FILE *__restrict __stream);
}
extern "C"
{
  extern ::__ssize_t __getdelim(char **__restrict __lineptr, ::size_t *__restrict __n, int __delimiter, ::FILE *__restrict __stream);
}
extern "C"
{
  extern ::__ssize_t getdelim(char **__restrict __lineptr, ::size_t *__restrict __n, int __delimiter, ::FILE *__restrict __stream);
}
extern "C"
{
  extern inline ::__ssize_t getline(char **__restrict __lineptr, ::size_t *__restrict __n, ::FILE *__restrict __stream) __attribute__((__gnu_inline__));
}
extern "C"
{
  extern int fputs(const char *__restrict __s, ::FILE *__restrict __stream);
}
extern "C"
{
  extern int puts(const char *__s);
}
extern "C"
{
  extern int ungetc(int __c, ::FILE *__stream);
}
extern "C"
{
  extern ::size_t fread(void *__restrict __ptr, ::size_t __size, ::size_t __n, ::FILE *__restrict __stream);
}
extern "C"
{
  extern ::size_t fwrite(const void *__restrict __ptr, ::size_t __size, ::size_t __n, ::FILE *__restrict __s);
}
extern "C"
{
  extern int fputs_unlocked(const char *__restrict __s, ::FILE *__restrict __stream);
}
extern "C"
{
  extern ::size_t fread_unlocked(void *__restrict __ptr, ::size_t __size, ::size_t __n, ::FILE *__restrict __stream);
}
extern "C"
{
  extern ::size_t fwrite_unlocked(const void *__restrict __ptr, ::size_t __size, ::size_t __n, ::FILE *__restrict __stream);
}
extern "C"
{
  extern int fseek(::FILE *__stream, long int __off, int __whence);
}
extern "C"
{
  extern long int ftell(::FILE *__stream);
}
extern "C"
{
  extern void rewind(::FILE *__stream);
}
extern "C"
{
  extern int fseeko(::FILE *__stream, ::__off_t __off, int __whence);
}
extern "C"
{
  extern ::__off_t ftello(::FILE *__stream);
}
extern "C"
{
  extern int fgetpos(::FILE *__restrict __stream, ::fpos_t *__restrict __pos);
}
extern "C"
{
  extern int fsetpos(::FILE *__stream, const ::fpos_t *__pos);
}
extern "C"
{
  extern int fseeko64(::FILE *__stream, ::__off64_t __off, int __whence);
}
extern "C"
{
  extern ::__off64_t ftello64(::FILE *__stream);
}
extern "C"
{
  extern int fgetpos64(::FILE *__restrict __stream, ::fpos64_t *__restrict __pos);
}
extern "C"
{
  extern int fsetpos64(::FILE *__stream, const ::fpos64_t *__pos);
}
extern "C"
{
  extern void clearerr(::FILE *__stream) throw();
}
extern "C"
{
  extern int feof(::FILE *__stream) throw();
}
extern "C"
{
  extern int ferror(::FILE *__stream) throw();
}
extern "C"
{
  extern void clearerr_unlocked(::FILE *__stream) throw();
}
extern "C"
{
  extern inline int feof_unlocked(::FILE *__stream) throw() __attribute__((__leaf__)) __attribute__((__gnu_inline__));
}
extern "C"
{
  extern inline int ferror_unlocked(::FILE *__stream) throw() __attribute__((__leaf__)) __attribute__((__gnu_inline__));
}
extern "C"
{
  extern void perror(const char *__s);
}
extern "C"
{
  extern int sys_nerr;
}
extern "C"
{
  extern const char *const sys_errlist[];
}
extern "C"
{
  extern int _sys_nerr;
}
extern "C"
{
  extern const char *const _sys_errlist[];
}
extern "C"
{
  extern int fileno(::FILE *__stream) throw();
}
extern "C"
{
  extern int fileno_unlocked(::FILE *__stream) throw();
}
extern "C"
{
  extern ::FILE *popen(const char *__command, const char *__modes);
}
extern "C"
{
  extern int pclose(::FILE *__stream);
}
extern "C"
{
  extern char *ctermid(char *__s) throw();
}
extern "C"
{
  extern char *cuserid(char *__s);
}
struct obstack;
extern "C"
{
  extern int obstack_printf(::obstack *__restrict __obstack, const char *__restrict __format, ...) throw() __attribute__((__format__(__printf__, 2, 3)));
}
extern "C"
{
  extern int obstack_vprintf(::obstack *__restrict __obstack, const char *__restrict __format, ::__gnuc_va_list __args) throw() __attribute__((__format__(__printf__, 2, 0)));
}
extern "C"
{
  extern void flockfile(::FILE *__stream) throw();
}
extern "C"
{
  extern int ftrylockfile(::FILE *__stream) throw();
}
extern "C"
{
  extern void funlockfile(::FILE *__stream) throw();
}
extern "C"
{
  extern inline __attribute__((__gnu_inline__)) int vprintf(const char *__restrict __fmt, ::__gnuc_va_list __arg)
  {
    return ::vfprintf(::stdout, __fmt, __arg);
  }
}
extern "C"
{
  extern inline __attribute__((__gnu_inline__)) int getchar()
  {
    return ::_IO_getc(::stdin);
  }
}
extern "C"
{
  extern inline __attribute__((__gnu_inline__)) int fgetc_unlocked(::FILE *__fp)
  {
    return __builtin_expect((*__fp)._IO_read_ptr >= (*__fp)._IO_read_end, 0) ? ::__uflow(__fp) : *((unsigned char *)(*__fp)._IO_read_ptr++);
  }
}
extern "C"
{
  extern inline __attribute__((__gnu_inline__)) int getc_unlocked(::FILE *__fp)
  {
    return __builtin_expect((*__fp)._IO_read_ptr >= (*__fp)._IO_read_end, 0) ? ::__uflow(__fp) : *((unsigned char *)(*__fp)._IO_read_ptr++);
  }
}
extern "C"
{
  extern inline __attribute__((__gnu_inline__)) int getchar_unlocked()
  {
    return __builtin_expect((*::stdin)._IO_read_ptr >= (*::stdin)._IO_read_end, 0) ? ::__uflow(::stdin) : *((unsigned char *)(*::stdin)._IO_read_ptr++);
  }
}
extern "C"
{
  extern inline __attribute__((__gnu_inline__)) int putchar(int __c)
  {
    return ::_IO_putc(__c, ::stdout);
  }
}
extern "C"
{
  extern inline __attribute__((__gnu_inline__)) int fputc_unlocked(int __c, ::FILE *__stream)
  {
    return __builtin_expect((*__stream)._IO_write_ptr >= (*__stream)._IO_write_end, 0) ? ::__overflow(__stream, (unsigned char)__c) : (unsigned char)(*(*__stream)._IO_write_ptr++ = __c);
  }
}
extern "C"
{
  extern inline __attribute__((__gnu_inline__)) int putc_unlocked(int __c, ::FILE *__stream)
  {
    return __builtin_expect((*__stream)._IO_write_ptr >= (*__stream)._IO_write_end, 0) ? ::__overflow(__stream, (unsigned char)__c) : (unsigned char)(*(*__stream)._IO_write_ptr++ = __c);
  }
}
extern "C"
{
  extern inline __attribute__((__gnu_inline__)) int putchar_unlocked(int __c)
  {
    return __builtin_expect((*::stdout)._IO_write_ptr >= (*::stdout)._IO_write_end, 0) ? ::__overflow(::stdout, (unsigned char)__c) : (unsigned char)(*(*::stdout)._IO_write_ptr++ = __c);
  }
}
extern "C"
{
  extern inline __attribute__((__gnu_inline__)) ::__ssize_t getline(char **__lineptr, ::size_t *__n, ::FILE *__stream)
  {
    return ::__getdelim(__lineptr, __n, '\n', __stream);
  }
}
extern "C"
{
  extern inline __attribute__((__leaf__)) __attribute__((__gnu_inline__)) int feof_unlocked(::FILE *__stream) throw()
  {
    return ((*__stream)._flags & 16) != 0;
  }
}
extern "C"
{
  extern inline __attribute__((__leaf__)) __attribute__((__gnu_inline__)) int ferror_unlocked(::FILE *__stream) throw()
  {
    return ((*__stream)._flags & 32) != 0;
  }
}
namespace std __attribute__((__visibility__("default"))) {
  using ::FILE;
  using ::fpos_t;
  using ::clearerr;
  using ::fclose;
  using ::feof;
  using ::ferror;
  using ::fflush;
  using ::fgetc;
  using ::fgetpos;
  using ::fgets;
  using ::fopen;
  using ::fprintf;
  using ::fputc;
  using ::fputs;
  using ::fread;
  using ::freopen;
  using ::fscanf;
  using ::fseek;
  using ::fsetpos;
  using ::ftell;
  using ::fwrite;
  using ::getc;
  using ::getchar;
  using ::gets;
  using ::perror;
  using ::printf;
  using ::putc;
  using ::putchar;
  using ::puts;
  using ::remove;
  using ::rename;
  using ::rewind;
  using ::scanf;
  using ::setbuf;
  using ::setvbuf;
  using ::sprintf;
  using ::sscanf;
  using ::tmpfile;
  using ::tmpnam;
  using ::ungetc;
  using ::vfprintf;
  using ::vprintf;
  using ::vsprintf;
}
namespace __gnu_cxx {
  using ::snprintf;
  using ::vfscanf;
  using ::vscanf;
  using ::vsnprintf;
  using ::vsscanf;
}
namespace std __attribute__((__visibility__("default"))) {
  using ::__gnu_cxx::snprintf;
  using ::__gnu_cxx::vfscanf;
  using ::__gnu_cxx::vscanf;
  using ::__gnu_cxx::vsnprintf;
  using ::__gnu_cxx::vsscanf;
}
extern "C"
{
  extern void *memcpy(void *__restrict __dest, const void *__restrict __src, ::size_t __n) throw() __attribute__((__nonnull__(1, 2)));
}
extern "C"
{
  extern void *memmove(void *__dest, const void *__src, ::size_t __n) throw() __attribute__((__nonnull__(1, 2)));
}
extern "C"
{
  extern void *memccpy(void *__restrict __dest, const void *__restrict __src, int __c, ::size_t __n) throw() __attribute__((__nonnull__(1, 2)));
}
extern "C"
{
  extern void *memset(void *__s, int __c, ::size_t __n) throw() __attribute__((__nonnull__(1)));
}
extern "C"
{
  extern int memcmp(const void *__s1, const void *__s2, ::size_t __n) throw() __attribute__((__pure__)) __attribute__((__nonnull__(1, 2)));
}
extern inline void *memchr(void *__s, int __c, ::size_t __n) throw() __asm("memchr") __attribute__((__pure__)) __attribute__((__nonnull__(1))) __attribute__((__always_inline__)) __attribute__((__gnu_inline__));
extern inline const void *memchr(const void *__s, int __c, ::size_t __n) throw() __asm("memchr") __attribute__((__pure__)) __attribute__((__nonnull__(1))) __attribute__((__always_inline__)) __attribute__((__gnu_inline__));
extern inline __attribute__((__pure__)) __attribute__((__nonnull__(1))) __attribute__((__always_inline__)) __attribute__((__gnu_inline__)) void *memchr(void *__s, int __c, ::size_t __n) throw()
{
  return __builtin_memchr(__s, __c, __n);
}
extern inline __attribute__((__pure__)) __attribute__((__nonnull__(1))) __attribute__((__always_inline__)) __attribute__((__gnu_inline__)) const void *memchr(const void *__s, int __c, ::size_t __n) throw()
{
  return __builtin_memchr(__s, __c, __n);
}
void *rawmemchr(void *__s, int __c) throw() __asm("rawmemchr") __attribute__((__pure__)) __attribute__((__nonnull__(1)));
const void *rawmemchr(const void *__s, int __c) throw() __asm("rawmemchr") __attribute__((__pure__)) __attribute__((__nonnull__(1)));
void *memrchr(void *__s, int __c, ::size_t __n) throw() __asm("memrchr") __attribute__((__pure__)) __attribute__((__nonnull__(1)));
const void *memrchr(const void *__s, int __c, ::size_t __n) throw() __asm("memrchr") __attribute__((__pure__)) __attribute__((__nonnull__(1)));
extern "C"
{
  extern char *strcpy(char *__restrict __dest, const char *__restrict __src) throw() __attribute__((__nonnull__(1, 2)));
}
extern "C"
{
  extern char *strncpy(char *__restrict __dest, const char *__restrict __src, ::size_t __n) throw() __attribute__((__nonnull__(1, 2)));
}
extern "C"
{
  extern char *strcat(char *__restrict __dest, const char *__restrict __src) throw() __attribute__((__nonnull__(1, 2)));
}
extern "C"
{
  extern char *strncat(char *__restrict __dest, const char *__restrict __src, ::size_t __n) throw() __attribute__((__nonnull__(1, 2)));
}
extern "C"
{
  extern int strcmp(const char *__s1, const char *__s2) throw() __attribute__((__pure__)) __attribute__((__nonnull__(1, 2)));
}
extern "C"
{
  extern int strncmp(const char *__s1, const char *__s2, ::size_t __n) throw() __attribute__((__pure__)) __attribute__((__nonnull__(1, 2)));
}
extern "C"
{
  extern int strcoll(const char *__s1, const char *__s2) throw() __attribute__((__pure__)) __attribute__((__nonnull__(1, 2)));
}
extern "C"
{
  extern ::size_t strxfrm(char *__restrict __dest, const char *__restrict __src, ::size_t __n) throw() __attribute__((__nonnull__(2)));
}
struct __locale_data;
struct  __locale_struct
{
    ::__locale_data *__locales[13L];
    const unsigned short int *__ctype_b;
    const int *__ctype_tolower;
    const int *__ctype_toupper;
    const char *__names[13L];
};
typedef ::__locale_struct *__locale_t;
typedef ::__locale_t locale_t;
extern "C"
{
  extern int strcoll_l(const char *__s1, const char *__s2, ::__locale_t __l) throw() __attribute__((__pure__)) __attribute__((__nonnull__(1, 2, 3)));
}
extern "C"
{
  extern ::size_t strxfrm_l(char *__dest, const char *__src, ::size_t __n, ::__locale_t __l) throw() __attribute__((__nonnull__(2, 4)));
}
extern "C"
{
  extern char *strdup(const char *__s) throw() __attribute__((__malloc__)) __attribute__((__nonnull__(1)));
}
extern "C"
{
  extern char *strndup(const char *__string, ::size_t __n) throw() __attribute__((__malloc__)) __attribute__((__nonnull__(1)));
}
extern inline char *strchr(char *__s, int __c) throw() __asm("strchr") __attribute__((__pure__)) __attribute__((__nonnull__(1))) __attribute__((__always_inline__)) __attribute__((__gnu_inline__));
extern inline const char *strchr(const char *__s, int __c) throw() __asm("strchr") __attribute__((__pure__)) __attribute__((__nonnull__(1))) __attribute__((__always_inline__)) __attribute__((__gnu_inline__));
extern inline __attribute__((__pure__)) __attribute__((__nonnull__(1))) __attribute__((__always_inline__)) __attribute__((__gnu_inline__)) char *strchr(char *__s, int __c) throw()
{
  return __builtin_strchr(__s, __c);
}
extern inline __attribute__((__pure__)) __attribute__((__nonnull__(1))) __attribute__((__always_inline__)) __attribute__((__gnu_inline__)) const char *strchr(const char *__s, int __c) throw()
{
  return __builtin_strchr(__s, __c);
}
extern inline char *strrchr(char *__s, int __c) throw() __asm("strrchr") __attribute__((__pure__)) __attribute__((__nonnull__(1))) __attribute__((__always_inline__)) __attribute__((__gnu_inline__));
extern inline const char *strrchr(const char *__s, int __c) throw() __asm("strrchr") __attribute__((__pure__)) __attribute__((__nonnull__(1))) __attribute__((__always_inline__)) __attribute__((__gnu_inline__));
extern inline __attribute__((__pure__)) __attribute__((__nonnull__(1))) __attribute__((__always_inline__)) __attribute__((__gnu_inline__)) char *strrchr(char *__s, int __c) throw()
{
  return __builtin_strrchr(__s, __c);
}
extern inline __attribute__((__pure__)) __attribute__((__nonnull__(1))) __attribute__((__always_inline__)) __attribute__((__gnu_inline__)) const char *strrchr(const char *__s, int __c) throw()
{
  return __builtin_strrchr(__s, __c);
}
char *strchrnul(char *__s, int __c) throw() __asm("strchrnul") __attribute__((__pure__)) __attribute__((__nonnull__(1)));
const char *strchrnul(const char *__s, int __c) throw() __asm("strchrnul") __attribute__((__pure__)) __attribute__((__nonnull__(1)));
extern "C"
{
  extern ::size_t strcspn(const char *__s, const char *__reject) throw() __attribute__((__pure__)) __attribute__((__nonnull__(1, 2)));
}
extern "C"
{
  extern ::size_t strspn(const char *__s, const char *__accept) throw() __attribute__((__pure__)) __attribute__((__nonnull__(1, 2)));
}
extern inline char *strpbrk(char *__s, const char *__accept) throw() __asm("strpbrk") __attribute__((__pure__)) __attribute__((__nonnull__(1, 2))) __attribute__((__always_inline__)) __attribute__((__gnu_inline__));
extern inline const char *strpbrk(const char *__s, const char *__accept) throw() __asm("strpbrk") __attribute__((__pure__)) __attribute__((__nonnull__(1, 2))) __attribute__((__always_inline__)) __attribute__((__gnu_inline__));
extern inline __attribute__((__pure__)) __attribute__((__nonnull__(1, 2))) __attribute__((__always_inline__)) __attribute__((__gnu_inline__)) char *strpbrk(char *__s, const char *__accept) throw()
{
  return __builtin_strpbrk(__s, __accept);
}
extern inline __attribute__((__pure__)) __attribute__((__nonnull__(1, 2))) __attribute__((__always_inline__)) __attribute__((__gnu_inline__)) const char *strpbrk(const char *__s, const char *__accept) throw()
{
  return __builtin_strpbrk(__s, __accept);
}
extern inline char *strstr(char *__haystack, const char *__needle) throw() __asm("strstr") __attribute__((__pure__)) __attribute__((__nonnull__(1, 2))) __attribute__((__always_inline__)) __attribute__((__gnu_inline__));
extern inline const char *strstr(const char *__haystack, const char *__needle) throw() __asm("strstr") __attribute__((__pure__)) __attribute__((__nonnull__(1, 2))) __attribute__((__always_inline__)) __attribute__((__gnu_inline__));
extern inline __attribute__((__pure__)) __attribute__((__nonnull__(1, 2))) __attribute__((__always_inline__)) __attribute__((__gnu_inline__)) char *strstr(char *__haystack, const char *__needle) throw()
{
  return __builtin_strstr(__haystack, __needle);
}
extern inline __attribute__((__pure__)) __attribute__((__nonnull__(1, 2))) __attribute__((__always_inline__)) __attribute__((__gnu_inline__)) const char *strstr(const char *__haystack, const char *__needle) throw()
{
  return __builtin_strstr(__haystack, __needle);
}
extern "C"
{
  extern char *strtok(char *__restrict __s, const char *__restrict __delim) throw() __attribute__((__nonnull__(2)));
}
extern "C"
{
  extern char *__strtok_r(char *__restrict __s, const char *__restrict __delim, char **__restrict __save_ptr) throw() __attribute__((__nonnull__(2, 3)));
}
extern "C"
{
  extern char *strtok_r(char *__restrict __s, const char *__restrict __delim, char **__restrict __save_ptr) throw() __attribute__((__nonnull__(2, 3)));
}
char *strcasestr(char *__haystack, const char *__needle) throw() __asm("strcasestr") __attribute__((__pure__)) __attribute__((__nonnull__(1, 2)));
const char *strcasestr(const char *__haystack, const char *__needle) throw() __asm("strcasestr") __attribute__((__pure__)) __attribute__((__nonnull__(1, 2)));
extern "C"
{
  extern void *memmem(const void *__haystack, ::size_t __haystacklen, const void *__needle, ::size_t __needlelen) throw() __attribute__((__pure__)) __attribute__((__nonnull__(1, 3)));
}
extern "C"
{
  extern void *__mempcpy(void *__restrict __dest, const void *__restrict __src, ::size_t __n) throw() __attribute__((__nonnull__(1, 2)));
}
extern "C"
{
  extern void *mempcpy(void *__restrict __dest, const void *__restrict __src, ::size_t __n) throw() __attribute__((__nonnull__(1, 2)));
}
extern "C"
{
  extern ::size_t strlen(const char *__s) throw() __attribute__((__pure__)) __attribute__((__nonnull__(1)));
}
extern "C"
{
  extern ::size_t strnlen(const char *__string, ::size_t __maxlen) throw() __attribute__((__pure__)) __attribute__((__nonnull__(1)));
}
extern "C"
{
  extern char *strerror(int __errnum) throw();
}
extern "C"
{
  extern char *strerror_r(int __errnum, char *__buf, ::size_t __buflen) throw() __attribute__((__nonnull__(2)));
}
extern "C"
{
  extern char *strerror_l(int __errnum, ::__locale_t __l) throw();
}
extern "C"
{
  extern void __bzero(void *__s, ::size_t __n) throw() __attribute__((__nonnull__(1)));
}
extern "C"
{
  extern void bcopy(const void *__src, void *__dest, ::size_t __n) throw() __attribute__((__nonnull__(1, 2)));
}
extern "C"
{
  extern void bzero(void *__s, ::size_t __n) throw() __attribute__((__nonnull__(1)));
}
extern "C"
{
  extern int bcmp(const void *__s1, const void *__s2, ::size_t __n) throw() __attribute__((__pure__)) __attribute__((__nonnull__(1, 2)));
}
extern inline char *index(char *__s, int __c) throw() __asm("index") __attribute__((__pure__)) __attribute__((__nonnull__(1))) __attribute__((__always_inline__)) __attribute__((__gnu_inline__));
extern inline const char *index(const char *__s, int __c) throw() __asm("index") __attribute__((__pure__)) __attribute__((__nonnull__(1))) __attribute__((__always_inline__)) __attribute__((__gnu_inline__));
extern inline __attribute__((__pure__)) __attribute__((__nonnull__(1))) __attribute__((__always_inline__)) __attribute__((__gnu_inline__)) char *index(char *__s, int __c) throw()
{
  return __builtin_index(__s, __c);
}
extern inline __attribute__((__pure__)) __attribute__((__nonnull__(1))) __attribute__((__always_inline__)) __attribute__((__gnu_inline__)) const char *index(const char *__s, int __c) throw()
{
  return __builtin_index(__s, __c);
}
extern inline char *rindex(char *__s, int __c) throw() __asm("rindex") __attribute__((__pure__)) __attribute__((__nonnull__(1))) __attribute__((__always_inline__)) __attribute__((__gnu_inline__));
extern inline const char *rindex(const char *__s, int __c) throw() __asm("rindex") __attribute__((__pure__)) __attribute__((__nonnull__(1))) __attribute__((__always_inline__)) __attribute__((__gnu_inline__));
extern inline __attribute__((__pure__)) __attribute__((__nonnull__(1))) __attribute__((__always_inline__)) __attribute__((__gnu_inline__)) char *rindex(char *__s, int __c) throw()
{
  return __builtin_rindex(__s, __c);
}
extern inline __attribute__((__pure__)) __attribute__((__nonnull__(1))) __attribute__((__always_inline__)) __attribute__((__gnu_inline__)) const char *rindex(const char *__s, int __c) throw()
{
  return __builtin_rindex(__s, __c);
}
extern "C"
{
  extern int ffs(int __i) throw() __attribute__((__const__));
}
extern "C"
{
  extern int ffsl(long int __l) throw() __attribute__((__const__));
}
extern "C"
{
  extern int ffsll(long long int __ll) throw() __attribute__((__const__));
}
extern "C"
{
  extern int strcasecmp(const char *__s1, const char *__s2) throw() __attribute__((__pure__)) __attribute__((__nonnull__(1, 2)));
}
extern "C"
{
  extern int strncasecmp(const char *__s1, const char *__s2, ::size_t __n) throw() __attribute__((__pure__)) __attribute__((__nonnull__(1, 2)));
}
extern "C"
{
  extern int strcasecmp_l(const char *__s1, const char *__s2, ::__locale_t __loc) throw() __attribute__((__pure__)) __attribute__((__nonnull__(1, 2, 3)));
}
extern "C"
{
  extern int strncasecmp_l(const char *__s1, const char *__s2, ::size_t __n, ::__locale_t __loc) throw() __attribute__((__pure__)) __attribute__((__nonnull__(1, 2, 4)));
}
extern "C"
{
  extern char *strsep(char **__restrict __stringp, const char *__restrict __delim) throw() __attribute__((__nonnull__(1, 2)));
}
extern "C"
{
  extern char *strsignal(int __sig) throw();
}
extern "C"
{
  extern char *__stpcpy(char *__restrict __dest, const char *__restrict __src) throw() __attribute__((__nonnull__(1, 2)));
}
extern "C"
{
  extern char *stpcpy(char *__restrict __dest, const char *__restrict __src) throw() __attribute__((__nonnull__(1, 2)));
}
extern "C"
{
  extern char *__stpncpy(char *__restrict __dest, const char *__restrict __src, ::size_t __n) throw() __attribute__((__nonnull__(1, 2)));
}
extern "C"
{
  extern char *stpncpy(char *__restrict __dest, const char *__restrict __src, ::size_t __n) throw() __attribute__((__nonnull__(1, 2)));
}
extern "C"
{
  extern int strverscmp(const char *__s1, const char *__s2) throw() __attribute__((__pure__)) __attribute__((__nonnull__(1, 2)));
}
extern "C"
{
  extern char *strfry(char *__string) throw() __attribute__((__nonnull__(1)));
}
extern "C"
{
  extern void *memfrob(void *__s, ::size_t __n) throw() __attribute__((__nonnull__(1)));
}
char *basename(char *__filename) throw() __asm("basename") __attribute__((__nonnull__(1)));
const char *basename(const char *__filename) throw() __asm("basename") __attribute__((__nonnull__(1)));
namespace std __attribute__((__visibility__("default"))) {
  using ::memchr;
  using ::memcmp;
  using ::memcpy;
  using ::memmove;
  using ::memset;
  using ::strcat;
  using ::strcmp;
  using ::strcoll;
  using ::strcpy;
  using ::strcspn;
  using ::strerror;
  using ::strlen;
  using ::strncat;
  using ::strncmp;
  using ::strncpy;
  using ::strspn;
  using ::strtok;
  using ::strxfrm;
  using ::strchr;
  using ::strpbrk;
  using ::strrchr;
  using ::strstr;
}
const int num_proteins(4);
const int num_features(2);
const int num_latent(8);
const int num_samples(4);
const int log_num_samples(2);
typedef float U_base;
typedef float U_type;
typedef float mu_base;
typedef float mu_type;
typedef float B_base;
typedef float B_type;
typedef float F_base;
typedef float F_type;
typedef float P_base;
typedef float P_type;
typedef float L_type;
typedef float S_type;
const float epsilon(5.00000000000000000000000000000000000000000000000000000e-01);
void update_model(::U_base U[4L][4L][8L], ::mu_base mu[4L][8L], ::B_base B[4L][2L][8L]);
void predict_compound(::F_base in[2L], ::P_base out[4L]);
static ::U_base U[4L][4L][8L];
static ::mu_base mu[4L][8L];
static ::B_base B[4L][2L][8L];
void load_model(::U_base (*U_in)[4L][8L], ::mu_base (*mu_in)[8L], ::B_base (*B_in)[2L][8L])
{
  for (int i(0); i < ::num_samples; i++)
    {
      for (int j(0); j < ::num_proteins; j++)
        {
          for (int k(0); k < ::num_latent; k++)
            {
              ::U[i][j][k] = U_in[i][j][k];
            }
        }
      for (int j(0); j < ::num_latent; j++)
        {
          ::mu[i][j] = mu_in[i][j];
        }
      for (int j(0); j < ::num_features; j++)
        {
          for (int k(0); k < ::num_latent; k++)
            {
              ::B[i][j][k] = B_in[i][j][k];
            }
        }
    }
}
void features_loop(const ::F_base *features, ::L_type (*latents)[8L])
{
  for (int d(0); d < ::num_features; d++)
    {
#pragma HLS PIPELINE II = 1
#pragma HLS ARRAY_PARTITION variable = B complete dim = 3
#pragma HLS ARRAY_PARTITION variable = B complete dim = 1
#pragma HLS ARRAY_PARTITION variable = mu complete dim = 2
#pragma HLS ARRAY_PARTITION variable = mu complete dim = 1
#pragma HLS ARRAY_PARTITION variable = latents complete dim = 1
#pragma HLS ARRAY_PARTITION variable = latents complete dim = 2
      const ::F_type feature(features[d]);
      for (int s(0); s < ::num_samples; s++)
        {
          for (int k(0); k < ::num_latent; k++)
            {
              ::L_type v(57005);
              if (d == 0)
                {
                  v = (::mu_type)::mu[s][k];
                }
              else
                {
                  v = latents[s][k];
                }
              ::L_type prod(feature * (::B_type)::B[s][d][k]);
              latents[s][k] = v + prod;
            }
        }
    }
}
void proteins_loop(::P_base *predictions, const ::L_type (*latents)[8L])
{
  for (int d(0); d < ::num_proteins; d++)
    {
#pragma HLS PIPELINE II = 1
#pragma HLS ARRAY_PARTITION variable = U complete dim = 1
#pragma HLS ARRAY_PARTITION variable = U complete dim = 3
      ::S_type sum(0);
      for (int s(0); s < ::num_samples; s++)
        {
          for (int k(0); k < ::num_latent; k++)
            {
              ::S_type prod(latents[s][k] * (::U_type)::U[s][d][k]);
              sum = sum + prod;
            }
        }
      ::P_type aggr(sum / ::num_samples);
      predictions[d] = aggr;
    }
}
void predict(const ::F_base *features, ::P_base *predictions)
{
#pragma HLS DATAFLOW
  ::L_type latents[4L][8L];
  ::features_loop(features, latents);
  ::proteins_loop(predictions, latents);
}
struct  nanos_args_0_t
{
    ::F_base *features;
    ::P_base *predictions;
    bool update_model;
    ::U_base (*U_in)[4L][8L];
    ::mu_base (*mu_in)[8L];
    ::B_base (*B_in)[2L][8L];
};
void fpga_ol_predict_or_update_model_1_unpacked(::F_base *features, ::P_base *predictions, bool update_model, ::U_base (*U_in)[4L][8L], ::mu_base (*mu_in)[8L], ::B_base (*B_in)[2L][8L]) throw();
static void fpga_ol_predict_or_update_model_1(::nanos_args_0_t &args) throw();
static void nanos_xlate_fun_predictcpp_0(::nanos_args_0_t &arg, ::nanos_wd_t wd) throw()
{
  {
    void *device_base_address;
    ::nanos_err_t nanos_err;
    device_base_address = 0;
    nanos_err = ::nanos_get_addr(0, &device_base_address, wd);
    if (nanos_err != ::NANOS_OK)
      {
        ::nanos_handle_error(nanos_err);
      }
    arg.features = (::F_base *)device_base_address;
  }
  {
    void *device_base_address;
    ::nanos_err_t nanos_err;
    device_base_address = 0;
    nanos_err = ::nanos_get_addr(1, &device_base_address, wd);
    if (nanos_err != ::NANOS_OK)
      {
        ::nanos_handle_error(nanos_err);
      }
    arg.predictions = (::P_base *)device_base_address;
  }
  {
    void *device_base_address;
    ::nanos_err_t nanos_err;
    device_base_address = 0;
    nanos_err = ::nanos_get_addr(2, &device_base_address, wd);
    if (nanos_err != ::NANOS_OK)
      {
        ::nanos_handle_error(nanos_err);
      }
    arg.U_in = (::U_base (*)[4L][8L])device_base_address;
  }
  {
    void *device_base_address;
    ::nanos_err_t nanos_err;
    device_base_address = 0;
    nanos_err = ::nanos_get_addr(3, &device_base_address, wd);
    if (nanos_err != ::NANOS_OK)
      {
        ::nanos_handle_error(nanos_err);
      }
    arg.mu_in = (::mu_base (*)[8L])device_base_address;
  }
  {
    void *device_base_address;
    ::nanos_err_t nanos_err;
    device_base_address = 0;
    nanos_err = ::nanos_get_addr(4, &device_base_address, wd);
    if (nanos_err != ::NANOS_OK)
      {
        ::nanos_handle_error(nanos_err);
      }
    arg.B_in = (::B_base (*)[2L][8L])device_base_address;
  }
}
void update_model(::U_base (*U)[4L][8L], ::mu_base (*mu)[8L], ::B_base (*B)[2L][8L])
{
  do
    {
      {
        ::F_base *mcc_arg_0(0);
        ::P_base *mcc_arg_1(0);
        bool mcc_arg_2(true);
        ::U_base (*mcc_arg_3)[4L][8L](U);
        ::mu_base (*mcc_arg_4)[8L](mu);
        ::B_base (*mcc_arg_5)[2L][8L](B);
        {
           /* device argument type */ 
          static ::nanos_fpga_args_t fpga_ol_predict_or_update_model_1_args /* () */ ;
          fpga_ol_predict_or_update_model_1_args.outline = (void (*)(void *))(void (*)(::nanos_args_0_t &))::fpga_ol_predict_or_update_model_1;
          fpga_ol_predict_or_update_model_1_args.type = 1195774825;
          static ::nanos_const_wd_definition_1 nanos_wd_const_data = { /* .::nanos_const_wd_definition_1::base =  */ { /* .::nanos_const_wd_definition_tag::props =  */ { /* .::nanos_wd_props_t::mandatory_creation =  */ 0,  /* .::nanos_wd_props_t::tied =  */ 0,  /* .::nanos_wd_props_t::clear_chunk =  */ 0,  /* .::nanos_wd_props_t::reserved0 =  */ 0,  /* .::nanos_wd_props_t::reserved1 =  */ 0,  /* .::nanos_wd_props_t::reserved2 =  */ 0,  /* .::nanos_wd_props_t::reserved3 =  */ 0,  /* .::nanos_wd_props_t::reserved4 =  */ 0},  /* .::nanos_const_wd_definition_tag::data_alignment =  */ alignof(::nanos_args_0_t),  /* .::nanos_const_wd_definition_tag::num_copies =  */ 5,  /* .::nanos_const_wd_definition_tag::num_devices =  */ 1,  /* .::nanos_const_wd_definition_tag::num_dimensions =  */ 15,  /* .::nanos_const_wd_definition_tag::description =  */ "predict_or_update_model"},  /* .::nanos_const_wd_definition_1::devices =  */ { /* [0] =  */ { /* .::nanos_device_t::factory =  */ ::nanos_fpga_factory,  /* .::nanos_device_t::arg =  */ &fpga_ol_predict_or_update_model_1_args}}};
          ::nanos_wd_dyn_props_t nanos_wd_dyn_props /* () */ ;
          nanos_wd_dyn_props.tie_to = 0;
          nanos_wd_dyn_props.priority = 0;
          nanos_wd_dyn_props.flags.is_final = 0;
          nanos_wd_dyn_props.flags.is_implicit = 0;
          nanos_wd_dyn_props.flags.is_recover = 0;
          ::nanos_args_0_t *ol_args;
          ol_args = (::nanos_args_0_t *)0;
          ::nanos_args_0_t imm_args /* () */ ;
          ::nanos_wd_t nanos_wd_((::nanos_wd_t)0);
          ::nanos_copy_data_t *ol_copy_data((::nanos_copy_data_t *)0);
          ::nanos_region_dimension_internal_t *ol_copy_dimensions((::nanos_region_dimension_internal_t *)0);
          ::nanos_err_t nanos_err;
          nanos_err = ::nanos_create_wd_compact(&nanos_wd_, &nanos_wd_const_data.base, &nanos_wd_dyn_props, sizeof(::nanos_args_0_t &), (void **)&ol_args, ::nanos_current_wd(), &ol_copy_data, &ol_copy_dimensions);
          if (nanos_err != ::NANOS_OK)
            {
              ::nanos_handle_error(nanos_err);
            }
          ::nanos_region_dimension_t dimensions_0[1L];
          ::nanos_region_dimension_t dimensions_1[1L];
          ::nanos_region_dimension_t dimensions_2[5L];
          ::nanos_region_dimension_t dimensions_3[3L];
          ::nanos_region_dimension_t dimensions_4[5L];
          ::nanos_data_access_t dependences[5L];
          dimensions_0[0].size = (((::num_features) - 1L - 0L) + 1L) * sizeof(::F_base);
          dimensions_0[0].lower_bound = (0L - 0L) * sizeof(::F_base);
          dimensions_0[0].accessed_length = ((::num_features) - 1L - 0L - (0L - 0L) + 1) * sizeof(::F_base);
          dependences[0].address = (void *)mcc_arg_0;
          dependences[0].offset = 0L;
          dependences[0].dimensions = dimensions_0;
          dependences[0].flags.input = 1;
          dependences[0].flags.output = 0;
          dependences[0].flags.can_rename = 0;
          dependences[0].flags.concurrent = 0;
          dependences[0].flags.commutative = 0;
          dependences[0].dimension_count = 1;
          dimensions_1[0].size = (((::num_proteins) - 1L - 0L) + 1L) * sizeof(::P_base);
          dimensions_1[0].lower_bound = (0L - 0L) * sizeof(::P_base);
          dimensions_1[0].accessed_length = ((::num_proteins) - 1L - 0L - (0L - 0L) + 1) * sizeof(::P_base);
          dependences[1].address = (void *)mcc_arg_1;
          dependences[1].offset = 0L;
          dependences[1].dimensions = dimensions_1;
          dependences[1].flags.input = 1;
          dependences[1].flags.output = 1;
          dependences[1].flags.can_rename = 0;
          dependences[1].flags.concurrent = 0;
          dependences[1].flags.commutative = 0;
          dependences[1].dimension_count = 1;
          dimensions_2[0].size = 8L * sizeof(::U_base);
          dimensions_2[0].lower_bound = (0L - 0L) * sizeof(::U_base);
          dimensions_2[0].accessed_length = (7L - 0L - (0L - 0L) + 1) * sizeof(::U_base);
          dimensions_2[1].size = 4L;
          dimensions_2[1].lower_bound = 0L - 0L;
          dimensions_2[1].accessed_length = 3L - 0L - (0L - 0L) + 1;
          dimensions_2[2].size = ((::num_latent) - 1L - 0L) + 1L;
          dimensions_2[2].lower_bound = 0L - 0L;
          dimensions_2[2].accessed_length = (::num_latent) - 1L - 0L - (0L - 0L) + 1;
          dimensions_2[3].size = ((::num_proteins) - 1L - 0L) + 1L;
          dimensions_2[3].lower_bound = 0L - 0L;
          dimensions_2[3].accessed_length = (::num_proteins) - 1L - 0L - (0L - 0L) + 1;
          dimensions_2[4].size = ((::num_samples) - 1L - 0L) + 1L;
          dimensions_2[4].lower_bound = 0L - 0L;
          dimensions_2[4].accessed_length = (::num_samples) - 1L - 0L - (0L - 0L) + 1;
          dependences[2].address = (void *)mcc_arg_3;
          dependences[2].offset = 0L;
          dependences[2].dimensions = dimensions_2;
          dependences[2].flags.input = 1;
          dependences[2].flags.output = 0;
          dependences[2].flags.can_rename = 0;
          dependences[2].flags.concurrent = 0;
          dependences[2].flags.commutative = 0;
          dependences[2].dimension_count = 5;
          dimensions_3[0].size = 8L * sizeof(::mu_base);
          dimensions_3[0].lower_bound = (0L - 0L) * sizeof(::mu_base);
          dimensions_3[0].accessed_length = (7L - 0L - (0L - 0L) + 1) * sizeof(::mu_base);
          dimensions_3[1].size = ((::num_latent) - 1L - 0L) + 1L;
          dimensions_3[1].lower_bound = 0L - 0L;
          dimensions_3[1].accessed_length = (::num_latent) - 1L - 0L - (0L - 0L) + 1;
          dimensions_3[2].size = ((::num_samples) - 1L - 0L) + 1L;
          dimensions_3[2].lower_bound = 0L - 0L;
          dimensions_3[2].accessed_length = (::num_samples) - 1L - 0L - (0L - 0L) + 1;
          dependences[3].address = (void *)mcc_arg_4;
          dependences[3].offset = 0L;
          dependences[3].dimensions = dimensions_3;
          dependences[3].flags.input = 1;
          dependences[3].flags.output = 0;
          dependences[3].flags.can_rename = 0;
          dependences[3].flags.concurrent = 0;
          dependences[3].flags.commutative = 0;
          dependences[3].dimension_count = 3;
          dimensions_4[0].size = 8L * sizeof(::B_base);
          dimensions_4[0].lower_bound = (0L - 0L) * sizeof(::B_base);
          dimensions_4[0].accessed_length = (7L - 0L - (0L - 0L) + 1) * sizeof(::B_base);
          dimensions_4[1].size = 2L;
          dimensions_4[1].lower_bound = 0L - 0L;
          dimensions_4[1].accessed_length = 1L - 0L - (0L - 0L) + 1;
          dimensions_4[2].size = ((::num_latent) - 1L - 0L) + 1L;
          dimensions_4[2].lower_bound = 0L - 0L;
          dimensions_4[2].accessed_length = (::num_latent) - 1L - 0L - (0L - 0L) + 1;
          dimensions_4[3].size = ((::num_features) - 1L - 0L) + 1L;
          dimensions_4[3].lower_bound = 0L - 0L;
          dimensions_4[3].accessed_length = (::num_features) - 1L - 0L - (0L - 0L) + 1;
          dimensions_4[4].size = ((::num_samples) - 1L - 0L) + 1L;
          dimensions_4[4].lower_bound = 0L - 0L;
          dimensions_4[4].accessed_length = (::num_samples) - 1L - 0L - (0L - 0L) + 1;
          dependences[4].address = (void *)mcc_arg_5;
          dependences[4].offset = 0L;
          dependences[4].dimensions = dimensions_4;
          dependences[4].flags.input = 1;
          dependences[4].flags.output = 0;
          dependences[4].flags.can_rename = 0;
          dependences[4].flags.concurrent = 0;
          dependences[4].flags.commutative = 0;
          dependences[4].dimension_count = 5;
          if (nanos_wd_ != (::nanos_wd_t)0)
            {
              (*ol_args).features = mcc_arg_0;
              (*ol_args).predictions = mcc_arg_1;
              (*ol_args).update_model = mcc_arg_2;
              (*ol_args).U_in = mcc_arg_3;
              (*ol_args).mu_in = mcc_arg_4;
              (*ol_args).B_in = mcc_arg_5;
              ol_copy_dimensions[0 + 0].size = (((::num_features) - 1L - 0L) + 1L) * sizeof(::F_base);
              ol_copy_dimensions[0 + 0].lower_bound = (0L - 0L) * sizeof(::F_base);
              ol_copy_dimensions[0 + 0].accessed_length = ((::num_features) - 1L - 0L - (0L - 0L) + 1) * sizeof(::F_base);
              ol_copy_data[0].sharing = ::NANOS_SHARED;
              ol_copy_data[0].address = (void *)mcc_arg_0;
              ol_copy_data[0].flags.input = 1;
              ol_copy_data[0].flags.output = 0;
              ol_copy_data[0].dimension_count = (short int)1;
              ol_copy_data[0].dimensions = &ol_copy_dimensions[0];
              ol_copy_data[0].offset = 0L;
              ol_copy_dimensions[1 + 0].size = (((::num_proteins) - 1L - 0L) + 1L) * sizeof(::P_base);
              ol_copy_dimensions[1 + 0].lower_bound = (0L - 0L) * sizeof(::P_base);
              ol_copy_dimensions[1 + 0].accessed_length = ((::num_proteins) - 1L - 0L - (0L - 0L) + 1) * sizeof(::P_base);
              ol_copy_data[1].sharing = ::NANOS_SHARED;
              ol_copy_data[1].address = (void *)mcc_arg_1;
              ol_copy_data[1].flags.input = 1;
              ol_copy_data[1].flags.output = 1;
              ol_copy_data[1].dimension_count = (short int)1;
              ol_copy_data[1].dimensions = &ol_copy_dimensions[1];
              ol_copy_data[1].offset = 0L;
              ol_copy_dimensions[2 + 0].size = 8L * sizeof(::U_base);
              ol_copy_dimensions[2 + 0].lower_bound = (0L - 0L) * sizeof(::U_base);
              ol_copy_dimensions[2 + 0].accessed_length = (7L - 0L - (0L - 0L) + 1) * sizeof(::U_base);
              ol_copy_dimensions[2 + 1].size = 4L;
              ol_copy_dimensions[2 + 1].lower_bound = 0L - 0L;
              ol_copy_dimensions[2 + 1].accessed_length = 3L - 0L - (0L - 0L) + 1;
              ol_copy_dimensions[2 + 2].size = ((::num_latent) - 1L - 0L) + 1L;
              ol_copy_dimensions[2 + 2].lower_bound = 0L - 0L;
              ol_copy_dimensions[2 + 2].accessed_length = (::num_latent) - 1L - 0L - (0L - 0L) + 1;
              ol_copy_dimensions[2 + 3].size = ((::num_proteins) - 1L - 0L) + 1L;
              ol_copy_dimensions[2 + 3].lower_bound = 0L - 0L;
              ol_copy_dimensions[2 + 3].accessed_length = (::num_proteins) - 1L - 0L - (0L - 0L) + 1;
              ol_copy_dimensions[2 + 4].size = ((::num_samples) - 1L - 0L) + 1L;
              ol_copy_dimensions[2 + 4].lower_bound = 0L - 0L;
              ol_copy_dimensions[2 + 4].accessed_length = (::num_samples) - 1L - 0L - (0L - 0L) + 1;
              ol_copy_data[2].sharing = ::NANOS_SHARED;
              ol_copy_data[2].address = (void *)mcc_arg_3;
              ol_copy_data[2].flags.input = 1;
              ol_copy_data[2].flags.output = 0;
              ol_copy_data[2].dimension_count = (short int)5;
              ol_copy_data[2].dimensions = &ol_copy_dimensions[2];
              ol_copy_data[2].offset = 0L;
              ol_copy_dimensions[7 + 0].size = 8L * sizeof(::mu_base);
              ol_copy_dimensions[7 + 0].lower_bound = (0L - 0L) * sizeof(::mu_base);
              ol_copy_dimensions[7 + 0].accessed_length = (7L - 0L - (0L - 0L) + 1) * sizeof(::mu_base);
              ol_copy_dimensions[7 + 1].size = ((::num_latent) - 1L - 0L) + 1L;
              ol_copy_dimensions[7 + 1].lower_bound = 0L - 0L;
              ol_copy_dimensions[7 + 1].accessed_length = (::num_latent) - 1L - 0L - (0L - 0L) + 1;
              ol_copy_dimensions[7 + 2].size = ((::num_samples) - 1L - 0L) + 1L;
              ol_copy_dimensions[7 + 2].lower_bound = 0L - 0L;
              ol_copy_dimensions[7 + 2].accessed_length = (::num_samples) - 1L - 0L - (0L - 0L) + 1;
              ol_copy_data[3].sharing = ::NANOS_SHARED;
              ol_copy_data[3].address = (void *)mcc_arg_4;
              ol_copy_data[3].flags.input = 1;
              ol_copy_data[3].flags.output = 0;
              ol_copy_data[3].dimension_count = (short int)3;
              ol_copy_data[3].dimensions = &ol_copy_dimensions[7];
              ol_copy_data[3].offset = 0L;
              ol_copy_dimensions[10 + 0].size = 8L * sizeof(::B_base);
              ol_copy_dimensions[10 + 0].lower_bound = (0L - 0L) * sizeof(::B_base);
              ol_copy_dimensions[10 + 0].accessed_length = (7L - 0L - (0L - 0L) + 1) * sizeof(::B_base);
              ol_copy_dimensions[10 + 1].size = 2L;
              ol_copy_dimensions[10 + 1].lower_bound = 0L - 0L;
              ol_copy_dimensions[10 + 1].accessed_length = 1L - 0L - (0L - 0L) + 1;
              ol_copy_dimensions[10 + 2].size = ((::num_latent) - 1L - 0L) + 1L;
              ol_copy_dimensions[10 + 2].lower_bound = 0L - 0L;
              ol_copy_dimensions[10 + 2].accessed_length = (::num_latent) - 1L - 0L - (0L - 0L) + 1;
              ol_copy_dimensions[10 + 3].size = ((::num_features) - 1L - 0L) + 1L;
              ol_copy_dimensions[10 + 3].lower_bound = 0L - 0L;
              ol_copy_dimensions[10 + 3].accessed_length = (::num_features) - 1L - 0L - (0L - 0L) + 1;
              ol_copy_dimensions[10 + 4].size = ((::num_samples) - 1L - 0L) + 1L;
              ol_copy_dimensions[10 + 4].lower_bound = 0L - 0L;
              ol_copy_dimensions[10 + 4].accessed_length = (::num_samples) - 1L - 0L - (0L - 0L) + 1;
              ol_copy_data[4].sharing = ::NANOS_SHARED;
              ol_copy_data[4].address = (void *)mcc_arg_5;
              ol_copy_data[4].flags.input = 1;
              ol_copy_data[4].flags.output = 0;
              ol_copy_data[4].dimension_count = (short int)5;
              ol_copy_data[4].dimensions = &ol_copy_dimensions[10];
              ol_copy_data[4].offset = 0L;
              nanos_err = ::nanos_set_translate_function(nanos_wd_, (::nanos_translate_args_t)::nanos_xlate_fun_predictcpp_0);
              if (nanos_err != ::NANOS_OK)
                {
                  ::nanos_handle_error(nanos_err);
                }
              nanos_err = ::nanos_submit(nanos_wd_, 5, &dependences[0], (::nanos_team_t)0);
              if (nanos_err != ::NANOS_OK)
                {
                  ::nanos_handle_error(nanos_err);
                }
            }
          else
            {
              imm_args.features = mcc_arg_0;
              imm_args.predictions = mcc_arg_1;
              imm_args.update_model = mcc_arg_2;
              imm_args.U_in = mcc_arg_3;
              imm_args.mu_in = mcc_arg_4;
              imm_args.B_in = mcc_arg_5;
              ::nanos_copy_data_t imm_copy_data[5L];
              ::nanos_region_dimension_internal_t imm_copy_dimensions[15L];
              imm_copy_dimensions[0 + 0].size = (((::num_features) - 1L - 0L) + 1L) * sizeof(::F_base);
              imm_copy_dimensions[0 + 0].lower_bound = (0L - 0L) * sizeof(::F_base);
              imm_copy_dimensions[0 + 0].accessed_length = ((::num_features) - 1L - 0L - (0L - 0L) + 1) * sizeof(::F_base);
              imm_copy_data[0].sharing = ::NANOS_SHARED;
              imm_copy_data[0].address = (void *)mcc_arg_0;
              imm_copy_data[0].flags.input = 1;
              imm_copy_data[0].flags.output = 0;
              imm_copy_data[0].dimension_count = (short int)1;
              imm_copy_data[0].dimensions = &imm_copy_dimensions[0];
              imm_copy_data[0].offset = 0L;
              imm_copy_dimensions[1 + 0].size = (((::num_proteins) - 1L - 0L) + 1L) * sizeof(::P_base);
              imm_copy_dimensions[1 + 0].lower_bound = (0L - 0L) * sizeof(::P_base);
              imm_copy_dimensions[1 + 0].accessed_length = ((::num_proteins) - 1L - 0L - (0L - 0L) + 1) * sizeof(::P_base);
              imm_copy_data[1].sharing = ::NANOS_SHARED;
              imm_copy_data[1].address = (void *)mcc_arg_1;
              imm_copy_data[1].flags.input = 1;
              imm_copy_data[1].flags.output = 1;
              imm_copy_data[1].dimension_count = (short int)1;
              imm_copy_data[1].dimensions = &imm_copy_dimensions[1];
              imm_copy_data[1].offset = 0L;
              imm_copy_dimensions[2 + 0].size = 8L * sizeof(::U_base);
              imm_copy_dimensions[2 + 0].lower_bound = (0L - 0L) * sizeof(::U_base);
              imm_copy_dimensions[2 + 0].accessed_length = (7L - 0L - (0L - 0L) + 1) * sizeof(::U_base);
              imm_copy_dimensions[2 + 1].size = 4L;
              imm_copy_dimensions[2 + 1].lower_bound = 0L - 0L;
              imm_copy_dimensions[2 + 1].accessed_length = 3L - 0L - (0L - 0L) + 1;
              imm_copy_dimensions[2 + 2].size = ((::num_latent) - 1L - 0L) + 1L;
              imm_copy_dimensions[2 + 2].lower_bound = 0L - 0L;
              imm_copy_dimensions[2 + 2].accessed_length = (::num_latent) - 1L - 0L - (0L - 0L) + 1;
              imm_copy_dimensions[2 + 3].size = ((::num_proteins) - 1L - 0L) + 1L;
              imm_copy_dimensions[2 + 3].lower_bound = 0L - 0L;
              imm_copy_dimensions[2 + 3].accessed_length = (::num_proteins) - 1L - 0L - (0L - 0L) + 1;
              imm_copy_dimensions[2 + 4].size = ((::num_samples) - 1L - 0L) + 1L;
              imm_copy_dimensions[2 + 4].lower_bound = 0L - 0L;
              imm_copy_dimensions[2 + 4].accessed_length = (::num_samples) - 1L - 0L - (0L - 0L) + 1;
              imm_copy_data[2].sharing = ::NANOS_SHARED;
              imm_copy_data[2].address = (void *)mcc_arg_3;
              imm_copy_data[2].flags.input = 1;
              imm_copy_data[2].flags.output = 0;
              imm_copy_data[2].dimension_count = (short int)5;
              imm_copy_data[2].dimensions = &imm_copy_dimensions[2];
              imm_copy_data[2].offset = 0L;
              imm_copy_dimensions[7 + 0].size = 8L * sizeof(::mu_base);
              imm_copy_dimensions[7 + 0].lower_bound = (0L - 0L) * sizeof(::mu_base);
              imm_copy_dimensions[7 + 0].accessed_length = (7L - 0L - (0L - 0L) + 1) * sizeof(::mu_base);
              imm_copy_dimensions[7 + 1].size = ((::num_latent) - 1L - 0L) + 1L;
              imm_copy_dimensions[7 + 1].lower_bound = 0L - 0L;
              imm_copy_dimensions[7 + 1].accessed_length = (::num_latent) - 1L - 0L - (0L - 0L) + 1;
              imm_copy_dimensions[7 + 2].size = ((::num_samples) - 1L - 0L) + 1L;
              imm_copy_dimensions[7 + 2].lower_bound = 0L - 0L;
              imm_copy_dimensions[7 + 2].accessed_length = (::num_samples) - 1L - 0L - (0L - 0L) + 1;
              imm_copy_data[3].sharing = ::NANOS_SHARED;
              imm_copy_data[3].address = (void *)mcc_arg_4;
              imm_copy_data[3].flags.input = 1;
              imm_copy_data[3].flags.output = 0;
              imm_copy_data[3].dimension_count = (short int)3;
              imm_copy_data[3].dimensions = &imm_copy_dimensions[7];
              imm_copy_data[3].offset = 0L;
              imm_copy_dimensions[10 + 0].size = 8L * sizeof(::B_base);
              imm_copy_dimensions[10 + 0].lower_bound = (0L - 0L) * sizeof(::B_base);
              imm_copy_dimensions[10 + 0].accessed_length = (7L - 0L - (0L - 0L) + 1) * sizeof(::B_base);
              imm_copy_dimensions[10 + 1].size = 2L;
              imm_copy_dimensions[10 + 1].lower_bound = 0L - 0L;
              imm_copy_dimensions[10 + 1].accessed_length = 1L - 0L - (0L - 0L) + 1;
              imm_copy_dimensions[10 + 2].size = ((::num_latent) - 1L - 0L) + 1L;
              imm_copy_dimensions[10 + 2].lower_bound = 0L - 0L;
              imm_copy_dimensions[10 + 2].accessed_length = (::num_latent) - 1L - 0L - (0L - 0L) + 1;
              imm_copy_dimensions[10 + 3].size = ((::num_features) - 1L - 0L) + 1L;
              imm_copy_dimensions[10 + 3].lower_bound = 0L - 0L;
              imm_copy_dimensions[10 + 3].accessed_length = (::num_features) - 1L - 0L - (0L - 0L) + 1;
              imm_copy_dimensions[10 + 4].size = ((::num_samples) - 1L - 0L) + 1L;
              imm_copy_dimensions[10 + 4].lower_bound = 0L - 0L;
              imm_copy_dimensions[10 + 4].accessed_length = (::num_samples) - 1L - 0L - (0L - 0L) + 1;
              imm_copy_data[4].sharing = ::NANOS_SHARED;
              imm_copy_data[4].address = (void *)mcc_arg_5;
              imm_copy_data[4].flags.input = 1;
              imm_copy_data[4].flags.output = 0;
              imm_copy_data[4].dimension_count = (short int)5;
              imm_copy_data[4].dimensions = &imm_copy_dimensions[10];
              imm_copy_data[4].offset = 0L;
              nanos_err = ::nanos_create_wd_and_run_compact(&nanos_wd_const_data.base, &nanos_wd_dyn_props, sizeof(::nanos_args_0_t &), &imm_args, 5, &dependences[0], imm_copy_data, imm_copy_dimensions, (::nanos_translate_args_t)::nanos_xlate_fun_predictcpp_0);
              if (nanos_err != ::NANOS_OK)
                {
                  ::nanos_handle_error(nanos_err);
                }
            }
        }
      }
    }
  while (0);
}
struct  nanos_args_1_t
{
    ::F_base *features;
    ::P_base *predictions;
    bool update_model;
    ::U_base (*U_in)[4L][8L];
    ::mu_base (*mu_in)[8L];
    ::B_base (*B_in)[2L][8L];
};
void fpga_ol_predict_or_update_model_3_unpacked(::F_base *features, ::P_base *predictions, bool update_model, ::U_base (*U_in)[4L][8L], ::mu_base (*mu_in)[8L], ::B_base (*B_in)[2L][8L]) throw();
static void fpga_ol_predict_or_update_model_3(::nanos_args_1_t &args) throw();
static void nanos_xlate_fun_predictcpp_1(::nanos_args_1_t &arg, ::nanos_wd_t wd) throw()
{
  {
    void *device_base_address;
    ::nanos_err_t nanos_err;
    device_base_address = 0;
    nanos_err = ::nanos_get_addr(0, &device_base_address, wd);
    if (nanos_err != ::NANOS_OK)
      {
        ::nanos_handle_error(nanos_err);
      }
    arg.features = (::F_base *)device_base_address;
  }
  {
    void *device_base_address;
    ::nanos_err_t nanos_err;
    device_base_address = 0;
    nanos_err = ::nanos_get_addr(1, &device_base_address, wd);
    if (nanos_err != ::NANOS_OK)
      {
        ::nanos_handle_error(nanos_err);
      }
    arg.predictions = (::P_base *)device_base_address;
  }
  {
    void *device_base_address;
    ::nanos_err_t nanos_err;
    device_base_address = 0;
    nanos_err = ::nanos_get_addr(2, &device_base_address, wd);
    if (nanos_err != ::NANOS_OK)
      {
        ::nanos_handle_error(nanos_err);
      }
    arg.U_in = (::U_base (*)[4L][8L])device_base_address;
  }
  {
    void *device_base_address;
    ::nanos_err_t nanos_err;
    device_base_address = 0;
    nanos_err = ::nanos_get_addr(3, &device_base_address, wd);
    if (nanos_err != ::NANOS_OK)
      {
        ::nanos_handle_error(nanos_err);
      }
    arg.mu_in = (::mu_base (*)[8L])device_base_address;
  }
  {
    void *device_base_address;
    ::nanos_err_t nanos_err;
    device_base_address = 0;
    nanos_err = ::nanos_get_addr(4, &device_base_address, wd);
    if (nanos_err != ::NANOS_OK)
      {
        ::nanos_handle_error(nanos_err);
      }
    arg.B_in = (::B_base (*)[2L][8L])device_base_address;
  }
}
void predict_compound(::F_base *in, ::P_base *out)
{
  do
    {
      {
        ::F_base *mcc_arg_8(in);
        ::P_base *mcc_arg_9(out);
        bool mcc_arg_10(false);
        ::U_base (*mcc_arg_11)[4L][8L](0);
        ::mu_base (*mcc_arg_12)[8L](0);
        ::B_base (*mcc_arg_13)[2L][8L](0);
        {
           /* device argument type */ 
          static ::nanos_fpga_args_t fpga_ol_predict_or_update_model_3_args /* () */ ;
          fpga_ol_predict_or_update_model_3_args.outline = (void (*)(void *))(void (*)(::nanos_args_1_t &))::fpga_ol_predict_or_update_model_3;
          fpga_ol_predict_or_update_model_3_args.type = 1195774825;
          static ::nanos_const_wd_definition_1 nanos_wd_const_data = { /* .::nanos_const_wd_definition_1::base =  */ { /* .::nanos_const_wd_definition_tag::props =  */ { /* .::nanos_wd_props_t::mandatory_creation =  */ 0,  /* .::nanos_wd_props_t::tied =  */ 0,  /* .::nanos_wd_props_t::clear_chunk =  */ 0,  /* .::nanos_wd_props_t::reserved0 =  */ 0,  /* .::nanos_wd_props_t::reserved1 =  */ 0,  /* .::nanos_wd_props_t::reserved2 =  */ 0,  /* .::nanos_wd_props_t::reserved3 =  */ 0,  /* .::nanos_wd_props_t::reserved4 =  */ 0},  /* .::nanos_const_wd_definition_tag::data_alignment =  */ alignof(::nanos_args_1_t),  /* .::nanos_const_wd_definition_tag::num_copies =  */ 5,  /* .::nanos_const_wd_definition_tag::num_devices =  */ 1,  /* .::nanos_const_wd_definition_tag::num_dimensions =  */ 15,  /* .::nanos_const_wd_definition_tag::description =  */ "predict_or_update_model"},  /* .::nanos_const_wd_definition_1::devices =  */ { /* [0] =  */ { /* .::nanos_device_t::factory =  */ ::nanos_fpga_factory,  /* .::nanos_device_t::arg =  */ &fpga_ol_predict_or_update_model_3_args}}};
          ::nanos_wd_dyn_props_t nanos_wd_dyn_props /* () */ ;
          nanos_wd_dyn_props.tie_to = 0;
          nanos_wd_dyn_props.priority = 0;
          nanos_wd_dyn_props.flags.is_final = 0;
          nanos_wd_dyn_props.flags.is_implicit = 0;
          nanos_wd_dyn_props.flags.is_recover = 0;
          ::nanos_args_1_t *ol_args;
          ol_args = (::nanos_args_1_t *)0;
          ::nanos_args_1_t imm_args /* () */ ;
          ::nanos_wd_t nanos_wd_((::nanos_wd_t)0);
          ::nanos_copy_data_t *ol_copy_data((::nanos_copy_data_t *)0);
          ::nanos_region_dimension_internal_t *ol_copy_dimensions((::nanos_region_dimension_internal_t *)0);
          ::nanos_err_t nanos_err;
          nanos_err = ::nanos_create_wd_compact(&nanos_wd_, &nanos_wd_const_data.base, &nanos_wd_dyn_props, sizeof(::nanos_args_1_t &), (void **)&ol_args, ::nanos_current_wd(), &ol_copy_data, &ol_copy_dimensions);
          if (nanos_err != ::NANOS_OK)
            {
              ::nanos_handle_error(nanos_err);
            }
          ::nanos_region_dimension_t dimensions_5[1L];
          ::nanos_region_dimension_t dimensions_6[1L];
          ::nanos_region_dimension_t dimensions_7[5L];
          ::nanos_region_dimension_t dimensions_8[3L];
          ::nanos_region_dimension_t dimensions_9[5L];
          ::nanos_data_access_t dependences[5L];
          dimensions_5[0].size = (((::num_features) - 1L - 0L) + 1L) * sizeof(::F_base);
          dimensions_5[0].lower_bound = (0L - 0L) * sizeof(::F_base);
          dimensions_5[0].accessed_length = ((::num_features) - 1L - 0L - (0L - 0L) + 1) * sizeof(::F_base);
          dependences[0].address = (void *)mcc_arg_8;
          dependences[0].offset = 0L;
          dependences[0].dimensions = dimensions_5;
          dependences[0].flags.input = 1;
          dependences[0].flags.output = 0;
          dependences[0].flags.can_rename = 0;
          dependences[0].flags.concurrent = 0;
          dependences[0].flags.commutative = 0;
          dependences[0].dimension_count = 1;
          dimensions_6[0].size = (((::num_proteins) - 1L - 0L) + 1L) * sizeof(::P_base);
          dimensions_6[0].lower_bound = (0L - 0L) * sizeof(::P_base);
          dimensions_6[0].accessed_length = ((::num_proteins) - 1L - 0L - (0L - 0L) + 1) * sizeof(::P_base);
          dependences[1].address = (void *)mcc_arg_9;
          dependences[1].offset = 0L;
          dependences[1].dimensions = dimensions_6;
          dependences[1].flags.input = 1;
          dependences[1].flags.output = 1;
          dependences[1].flags.can_rename = 0;
          dependences[1].flags.concurrent = 0;
          dependences[1].flags.commutative = 0;
          dependences[1].dimension_count = 1;
          dimensions_7[0].size = 8L * sizeof(::U_base);
          dimensions_7[0].lower_bound = (0L - 0L) * sizeof(::U_base);
          dimensions_7[0].accessed_length = (7L - 0L - (0L - 0L) + 1) * sizeof(::U_base);
          dimensions_7[1].size = 4L;
          dimensions_7[1].lower_bound = 0L - 0L;
          dimensions_7[1].accessed_length = 3L - 0L - (0L - 0L) + 1;
          dimensions_7[2].size = ((::num_latent) - 1L - 0L) + 1L;
          dimensions_7[2].lower_bound = 0L - 0L;
          dimensions_7[2].accessed_length = (::num_latent) - 1L - 0L - (0L - 0L) + 1;
          dimensions_7[3].size = ((::num_proteins) - 1L - 0L) + 1L;
          dimensions_7[3].lower_bound = 0L - 0L;
          dimensions_7[3].accessed_length = (::num_proteins) - 1L - 0L - (0L - 0L) + 1;
          dimensions_7[4].size = ((::num_samples) - 1L - 0L) + 1L;
          dimensions_7[4].lower_bound = 0L - 0L;
          dimensions_7[4].accessed_length = (::num_samples) - 1L - 0L - (0L - 0L) + 1;
          dependences[2].address = (void *)mcc_arg_11;
          dependences[2].offset = 0L;
          dependences[2].dimensions = dimensions_7;
          dependences[2].flags.input = 1;
          dependences[2].flags.output = 0;
          dependences[2].flags.can_rename = 0;
          dependences[2].flags.concurrent = 0;
          dependences[2].flags.commutative = 0;
          dependences[2].dimension_count = 5;
          dimensions_8[0].size = 8L * sizeof(::mu_base);
          dimensions_8[0].lower_bound = (0L - 0L) * sizeof(::mu_base);
          dimensions_8[0].accessed_length = (7L - 0L - (0L - 0L) + 1) * sizeof(::mu_base);
          dimensions_8[1].size = ((::num_latent) - 1L - 0L) + 1L;
          dimensions_8[1].lower_bound = 0L - 0L;
          dimensions_8[1].accessed_length = (::num_latent) - 1L - 0L - (0L - 0L) + 1;
          dimensions_8[2].size = ((::num_samples) - 1L - 0L) + 1L;
          dimensions_8[2].lower_bound = 0L - 0L;
          dimensions_8[2].accessed_length = (::num_samples) - 1L - 0L - (0L - 0L) + 1;
          dependences[3].address = (void *)mcc_arg_12;
          dependences[3].offset = 0L;
          dependences[3].dimensions = dimensions_8;
          dependences[3].flags.input = 1;
          dependences[3].flags.output = 0;
          dependences[3].flags.can_rename = 0;
          dependences[3].flags.concurrent = 0;
          dependences[3].flags.commutative = 0;
          dependences[3].dimension_count = 3;
          dimensions_9[0].size = 8L * sizeof(::B_base);
          dimensions_9[0].lower_bound = (0L - 0L) * sizeof(::B_base);
          dimensions_9[0].accessed_length = (7L - 0L - (0L - 0L) + 1) * sizeof(::B_base);
          dimensions_9[1].size = 2L;
          dimensions_9[1].lower_bound = 0L - 0L;
          dimensions_9[1].accessed_length = 1L - 0L - (0L - 0L) + 1;
          dimensions_9[2].size = ((::num_latent) - 1L - 0L) + 1L;
          dimensions_9[2].lower_bound = 0L - 0L;
          dimensions_9[2].accessed_length = (::num_latent) - 1L - 0L - (0L - 0L) + 1;
          dimensions_9[3].size = ((::num_features) - 1L - 0L) + 1L;
          dimensions_9[3].lower_bound = 0L - 0L;
          dimensions_9[3].accessed_length = (::num_features) - 1L - 0L - (0L - 0L) + 1;
          dimensions_9[4].size = ((::num_samples) - 1L - 0L) + 1L;
          dimensions_9[4].lower_bound = 0L - 0L;
          dimensions_9[4].accessed_length = (::num_samples) - 1L - 0L - (0L - 0L) + 1;
          dependences[4].address = (void *)mcc_arg_13;
          dependences[4].offset = 0L;
          dependences[4].dimensions = dimensions_9;
          dependences[4].flags.input = 1;
          dependences[4].flags.output = 0;
          dependences[4].flags.can_rename = 0;
          dependences[4].flags.concurrent = 0;
          dependences[4].flags.commutative = 0;
          dependences[4].dimension_count = 5;
          if (nanos_wd_ != (::nanos_wd_t)0)
            {
              (*ol_args).features = mcc_arg_8;
              (*ol_args).predictions = mcc_arg_9;
              (*ol_args).update_model = mcc_arg_10;
              (*ol_args).U_in = mcc_arg_11;
              (*ol_args).mu_in = mcc_arg_12;
              (*ol_args).B_in = mcc_arg_13;
              ol_copy_dimensions[0 + 0].size = (((::num_features) - 1L - 0L) + 1L) * sizeof(::F_base);
              ol_copy_dimensions[0 + 0].lower_bound = (0L - 0L) * sizeof(::F_base);
              ol_copy_dimensions[0 + 0].accessed_length = ((::num_features) - 1L - 0L - (0L - 0L) + 1) * sizeof(::F_base);
              ol_copy_data[0].sharing = ::NANOS_SHARED;
              ol_copy_data[0].address = (void *)mcc_arg_8;
              ol_copy_data[0].flags.input = 1;
              ol_copy_data[0].flags.output = 0;
              ol_copy_data[0].dimension_count = (short int)1;
              ol_copy_data[0].dimensions = &ol_copy_dimensions[0];
              ol_copy_data[0].offset = 0L;
              ol_copy_dimensions[1 + 0].size = (((::num_proteins) - 1L - 0L) + 1L) * sizeof(::P_base);
              ol_copy_dimensions[1 + 0].lower_bound = (0L - 0L) * sizeof(::P_base);
              ol_copy_dimensions[1 + 0].accessed_length = ((::num_proteins) - 1L - 0L - (0L - 0L) + 1) * sizeof(::P_base);
              ol_copy_data[1].sharing = ::NANOS_SHARED;
              ol_copy_data[1].address = (void *)mcc_arg_9;
              ol_copy_data[1].flags.input = 1;
              ol_copy_data[1].flags.output = 1;
              ol_copy_data[1].dimension_count = (short int)1;
              ol_copy_data[1].dimensions = &ol_copy_dimensions[1];
              ol_copy_data[1].offset = 0L;
              ol_copy_dimensions[2 + 0].size = 8L * sizeof(::U_base);
              ol_copy_dimensions[2 + 0].lower_bound = (0L - 0L) * sizeof(::U_base);
              ol_copy_dimensions[2 + 0].accessed_length = (7L - 0L - (0L - 0L) + 1) * sizeof(::U_base);
              ol_copy_dimensions[2 + 1].size = 4L;
              ol_copy_dimensions[2 + 1].lower_bound = 0L - 0L;
              ol_copy_dimensions[2 + 1].accessed_length = 3L - 0L - (0L - 0L) + 1;
              ol_copy_dimensions[2 + 2].size = ((::num_latent) - 1L - 0L) + 1L;
              ol_copy_dimensions[2 + 2].lower_bound = 0L - 0L;
              ol_copy_dimensions[2 + 2].accessed_length = (::num_latent) - 1L - 0L - (0L - 0L) + 1;
              ol_copy_dimensions[2 + 3].size = ((::num_proteins) - 1L - 0L) + 1L;
              ol_copy_dimensions[2 + 3].lower_bound = 0L - 0L;
              ol_copy_dimensions[2 + 3].accessed_length = (::num_proteins) - 1L - 0L - (0L - 0L) + 1;
              ol_copy_dimensions[2 + 4].size = ((::num_samples) - 1L - 0L) + 1L;
              ol_copy_dimensions[2 + 4].lower_bound = 0L - 0L;
              ol_copy_dimensions[2 + 4].accessed_length = (::num_samples) - 1L - 0L - (0L - 0L) + 1;
              ol_copy_data[2].sharing = ::NANOS_SHARED;
              ol_copy_data[2].address = (void *)mcc_arg_11;
              ol_copy_data[2].flags.input = 1;
              ol_copy_data[2].flags.output = 0;
              ol_copy_data[2].dimension_count = (short int)5;
              ol_copy_data[2].dimensions = &ol_copy_dimensions[2];
              ol_copy_data[2].offset = 0L;
              ol_copy_dimensions[7 + 0].size = 8L * sizeof(::mu_base);
              ol_copy_dimensions[7 + 0].lower_bound = (0L - 0L) * sizeof(::mu_base);
              ol_copy_dimensions[7 + 0].accessed_length = (7L - 0L - (0L - 0L) + 1) * sizeof(::mu_base);
              ol_copy_dimensions[7 + 1].size = ((::num_latent) - 1L - 0L) + 1L;
              ol_copy_dimensions[7 + 1].lower_bound = 0L - 0L;
              ol_copy_dimensions[7 + 1].accessed_length = (::num_latent) - 1L - 0L - (0L - 0L) + 1;
              ol_copy_dimensions[7 + 2].size = ((::num_samples) - 1L - 0L) + 1L;
              ol_copy_dimensions[7 + 2].lower_bound = 0L - 0L;
              ol_copy_dimensions[7 + 2].accessed_length = (::num_samples) - 1L - 0L - (0L - 0L) + 1;
              ol_copy_data[3].sharing = ::NANOS_SHARED;
              ol_copy_data[3].address = (void *)mcc_arg_12;
              ol_copy_data[3].flags.input = 1;
              ol_copy_data[3].flags.output = 0;
              ol_copy_data[3].dimension_count = (short int)3;
              ol_copy_data[3].dimensions = &ol_copy_dimensions[7];
              ol_copy_data[3].offset = 0L;
              ol_copy_dimensions[10 + 0].size = 8L * sizeof(::B_base);
              ol_copy_dimensions[10 + 0].lower_bound = (0L - 0L) * sizeof(::B_base);
              ol_copy_dimensions[10 + 0].accessed_length = (7L - 0L - (0L - 0L) + 1) * sizeof(::B_base);
              ol_copy_dimensions[10 + 1].size = 2L;
              ol_copy_dimensions[10 + 1].lower_bound = 0L - 0L;
              ol_copy_dimensions[10 + 1].accessed_length = 1L - 0L - (0L - 0L) + 1;
              ol_copy_dimensions[10 + 2].size = ((::num_latent) - 1L - 0L) + 1L;
              ol_copy_dimensions[10 + 2].lower_bound = 0L - 0L;
              ol_copy_dimensions[10 + 2].accessed_length = (::num_latent) - 1L - 0L - (0L - 0L) + 1;
              ol_copy_dimensions[10 + 3].size = ((::num_features) - 1L - 0L) + 1L;
              ol_copy_dimensions[10 + 3].lower_bound = 0L - 0L;
              ol_copy_dimensions[10 + 3].accessed_length = (::num_features) - 1L - 0L - (0L - 0L) + 1;
              ol_copy_dimensions[10 + 4].size = ((::num_samples) - 1L - 0L) + 1L;
              ol_copy_dimensions[10 + 4].lower_bound = 0L - 0L;
              ol_copy_dimensions[10 + 4].accessed_length = (::num_samples) - 1L - 0L - (0L - 0L) + 1;
              ol_copy_data[4].sharing = ::NANOS_SHARED;
              ol_copy_data[4].address = (void *)mcc_arg_13;
              ol_copy_data[4].flags.input = 1;
              ol_copy_data[4].flags.output = 0;
              ol_copy_data[4].dimension_count = (short int)5;
              ol_copy_data[4].dimensions = &ol_copy_dimensions[10];
              ol_copy_data[4].offset = 0L;
              nanos_err = ::nanos_set_translate_function(nanos_wd_, (::nanos_translate_args_t)::nanos_xlate_fun_predictcpp_1);
              if (nanos_err != ::NANOS_OK)
                {
                  ::nanos_handle_error(nanos_err);
                }
              nanos_err = ::nanos_submit(nanos_wd_, 5, &dependences[0], (::nanos_team_t)0);
              if (nanos_err != ::NANOS_OK)
                {
                  ::nanos_handle_error(nanos_err);
                }
            }
          else
            {
              imm_args.features = mcc_arg_8;
              imm_args.predictions = mcc_arg_9;
              imm_args.update_model = mcc_arg_10;
              imm_args.U_in = mcc_arg_11;
              imm_args.mu_in = mcc_arg_12;
              imm_args.B_in = mcc_arg_13;
              ::nanos_copy_data_t imm_copy_data[5L];
              ::nanos_region_dimension_internal_t imm_copy_dimensions[15L];
              imm_copy_dimensions[0 + 0].size = (((::num_features) - 1L - 0L) + 1L) * sizeof(::F_base);
              imm_copy_dimensions[0 + 0].lower_bound = (0L - 0L) * sizeof(::F_base);
              imm_copy_dimensions[0 + 0].accessed_length = ((::num_features) - 1L - 0L - (0L - 0L) + 1) * sizeof(::F_base);
              imm_copy_data[0].sharing = ::NANOS_SHARED;
              imm_copy_data[0].address = (void *)mcc_arg_8;
              imm_copy_data[0].flags.input = 1;
              imm_copy_data[0].flags.output = 0;
              imm_copy_data[0].dimension_count = (short int)1;
              imm_copy_data[0].dimensions = &imm_copy_dimensions[0];
              imm_copy_data[0].offset = 0L;
              imm_copy_dimensions[1 + 0].size = (((::num_proteins) - 1L - 0L) + 1L) * sizeof(::P_base);
              imm_copy_dimensions[1 + 0].lower_bound = (0L - 0L) * sizeof(::P_base);
              imm_copy_dimensions[1 + 0].accessed_length = ((::num_proteins) - 1L - 0L - (0L - 0L) + 1) * sizeof(::P_base);
              imm_copy_data[1].sharing = ::NANOS_SHARED;
              imm_copy_data[1].address = (void *)mcc_arg_9;
              imm_copy_data[1].flags.input = 1;
              imm_copy_data[1].flags.output = 1;
              imm_copy_data[1].dimension_count = (short int)1;
              imm_copy_data[1].dimensions = &imm_copy_dimensions[1];
              imm_copy_data[1].offset = 0L;
              imm_copy_dimensions[2 + 0].size = 8L * sizeof(::U_base);
              imm_copy_dimensions[2 + 0].lower_bound = (0L - 0L) * sizeof(::U_base);
              imm_copy_dimensions[2 + 0].accessed_length = (7L - 0L - (0L - 0L) + 1) * sizeof(::U_base);
              imm_copy_dimensions[2 + 1].size = 4L;
              imm_copy_dimensions[2 + 1].lower_bound = 0L - 0L;
              imm_copy_dimensions[2 + 1].accessed_length = 3L - 0L - (0L - 0L) + 1;
              imm_copy_dimensions[2 + 2].size = ((::num_latent) - 1L - 0L) + 1L;
              imm_copy_dimensions[2 + 2].lower_bound = 0L - 0L;
              imm_copy_dimensions[2 + 2].accessed_length = (::num_latent) - 1L - 0L - (0L - 0L) + 1;
              imm_copy_dimensions[2 + 3].size = ((::num_proteins) - 1L - 0L) + 1L;
              imm_copy_dimensions[2 + 3].lower_bound = 0L - 0L;
              imm_copy_dimensions[2 + 3].accessed_length = (::num_proteins) - 1L - 0L - (0L - 0L) + 1;
              imm_copy_dimensions[2 + 4].size = ((::num_samples) - 1L - 0L) + 1L;
              imm_copy_dimensions[2 + 4].lower_bound = 0L - 0L;
              imm_copy_dimensions[2 + 4].accessed_length = (::num_samples) - 1L - 0L - (0L - 0L) + 1;
              imm_copy_data[2].sharing = ::NANOS_SHARED;
              imm_copy_data[2].address = (void *)mcc_arg_11;
              imm_copy_data[2].flags.input = 1;
              imm_copy_data[2].flags.output = 0;
              imm_copy_data[2].dimension_count = (short int)5;
              imm_copy_data[2].dimensions = &imm_copy_dimensions[2];
              imm_copy_data[2].offset = 0L;
              imm_copy_dimensions[7 + 0].size = 8L * sizeof(::mu_base);
              imm_copy_dimensions[7 + 0].lower_bound = (0L - 0L) * sizeof(::mu_base);
              imm_copy_dimensions[7 + 0].accessed_length = (7L - 0L - (0L - 0L) + 1) * sizeof(::mu_base);
              imm_copy_dimensions[7 + 1].size = ((::num_latent) - 1L - 0L) + 1L;
              imm_copy_dimensions[7 + 1].lower_bound = 0L - 0L;
              imm_copy_dimensions[7 + 1].accessed_length = (::num_latent) - 1L - 0L - (0L - 0L) + 1;
              imm_copy_dimensions[7 + 2].size = ((::num_samples) - 1L - 0L) + 1L;
              imm_copy_dimensions[7 + 2].lower_bound = 0L - 0L;
              imm_copy_dimensions[7 + 2].accessed_length = (::num_samples) - 1L - 0L - (0L - 0L) + 1;
              imm_copy_data[3].sharing = ::NANOS_SHARED;
              imm_copy_data[3].address = (void *)mcc_arg_12;
              imm_copy_data[3].flags.input = 1;
              imm_copy_data[3].flags.output = 0;
              imm_copy_data[3].dimension_count = (short int)3;
              imm_copy_data[3].dimensions = &imm_copy_dimensions[7];
              imm_copy_data[3].offset = 0L;
              imm_copy_dimensions[10 + 0].size = 8L * sizeof(::B_base);
              imm_copy_dimensions[10 + 0].lower_bound = (0L - 0L) * sizeof(::B_base);
              imm_copy_dimensions[10 + 0].accessed_length = (7L - 0L - (0L - 0L) + 1) * sizeof(::B_base);
              imm_copy_dimensions[10 + 1].size = 2L;
              imm_copy_dimensions[10 + 1].lower_bound = 0L - 0L;
              imm_copy_dimensions[10 + 1].accessed_length = 1L - 0L - (0L - 0L) + 1;
              imm_copy_dimensions[10 + 2].size = ((::num_latent) - 1L - 0L) + 1L;
              imm_copy_dimensions[10 + 2].lower_bound = 0L - 0L;
              imm_copy_dimensions[10 + 2].accessed_length = (::num_latent) - 1L - 0L - (0L - 0L) + 1;
              imm_copy_dimensions[10 + 3].size = ((::num_features) - 1L - 0L) + 1L;
              imm_copy_dimensions[10 + 3].lower_bound = 0L - 0L;
              imm_copy_dimensions[10 + 3].accessed_length = (::num_features) - 1L - 0L - (0L - 0L) + 1;
              imm_copy_dimensions[10 + 4].size = ((::num_samples) - 1L - 0L) + 1L;
              imm_copy_dimensions[10 + 4].lower_bound = 0L - 0L;
              imm_copy_dimensions[10 + 4].accessed_length = (::num_samples) - 1L - 0L - (0L - 0L) + 1;
              imm_copy_data[4].sharing = ::NANOS_SHARED;
              imm_copy_data[4].address = (void *)mcc_arg_13;
              imm_copy_data[4].flags.input = 1;
              imm_copy_data[4].flags.output = 0;
              imm_copy_data[4].dimension_count = (short int)5;
              imm_copy_data[4].dimensions = &imm_copy_dimensions[10];
              imm_copy_data[4].offset = 0L;
              nanos_err = ::nanos_create_wd_and_run_compact(&nanos_wd_const_data.base, &nanos_wd_dyn_props, sizeof(::nanos_args_1_t &), &imm_args, 5, &dependences[0], imm_copy_data, imm_copy_dimensions, (::nanos_translate_args_t)::nanos_xlate_fun_predictcpp_1);
              if (nanos_err != ::NANOS_OK)
                {
                  ::nanos_handle_error(nanos_err);
                }
            }
        }
      }
    }
  while (0);
}
void fpga_ol_predict_or_update_model_1_unpacked(::F_base *features, ::P_base *predictions, bool update_model, ::U_base (*U_in)[4L][8L], ::mu_base (*mu_in)[8L], ::B_base (*B_in)[2L][8L]) throw()
{
  union  mcc_union_anon_28
  {
      ::F_base *features;
      ::uint64_t features_task_arg;
  };
  mcc_union_anon_28 mcc_arg_0 /* () */ ;
  mcc_arg_0.features = (::F_base *)::nanos_fpga_get_phy_address((void *)features);
  ::nanos_fpga_set_task_arg(::nanos_current_wd(), 0, 1, 1, mcc_arg_0.features_task_arg);
  union  mcc_union_anon_29
  {
      ::P_base *predictions;
      ::uint64_t predictions_task_arg;
  };
  mcc_union_anon_29 mcc_arg_1 /* () */ ;
  mcc_arg_1.predictions = (::P_base *)::nanos_fpga_get_phy_address((void *)predictions);
  ::nanos_fpga_set_task_arg(::nanos_current_wd(), 1, 1, 1, mcc_arg_1.predictions_task_arg);
  union  mcc_union_anon_30
  {
      bool update_model;
      ::uint64_t update_model_task_arg;
  };
  mcc_union_anon_30 mcc_arg_2 /* () */ ;
  mcc_arg_2.update_model = update_model;
  ::nanos_fpga_set_task_arg(::nanos_current_wd(), 2, 1, 1, mcc_arg_2.update_model_task_arg);
  union  mcc_union_anon_31
  {
      ::U_base (*U_in)[4L][8L];
      ::uint64_t U_in_task_arg;
  };
  mcc_union_anon_31 mcc_arg_3 /* () */ ;
  mcc_arg_3.U_in = (::U_base (*)[4L][8L])::nanos_fpga_get_phy_address((void *)U_in);
  ::nanos_fpga_set_task_arg(::nanos_current_wd(), 3, 1, 1, mcc_arg_3.U_in_task_arg);
  union  mcc_union_anon_32
  {
      ::mu_base (*mu_in)[8L];
      ::uint64_t mu_in_task_arg;
  };
  mcc_union_anon_32 mcc_arg_4 /* () */ ;
  mcc_arg_4.mu_in = (::mu_base (*)[8L])::nanos_fpga_get_phy_address((void *)mu_in);
  ::nanos_fpga_set_task_arg(::nanos_current_wd(), 4, 1, 1, mcc_arg_4.mu_in_task_arg);
  union  mcc_union_anon_33
  {
      ::B_base (*B_in)[2L][8L];
      ::uint64_t B_in_task_arg;
  };
  mcc_union_anon_33 mcc_arg_5 /* () */ ;
  mcc_arg_5.B_in = (::B_base (*)[2L][8L])::nanos_fpga_get_phy_address((void *)B_in);
  ::nanos_fpga_set_task_arg(::nanos_current_wd(), 5, 1, 1, mcc_arg_5.B_in_task_arg);
  ;
}
static void fpga_ol_predict_or_update_model_1(::nanos_args_0_t &args) throw()
{
  {
    static int nanos_funct_id_init(0);
    static ::nanos_event_key_t nanos_instr_uf_location_key(0);
    ::nanos_err_t nanos_err;
    if (nanos_funct_id_init == 0)
      {
        nanos_err = ::nanos_instrument_get_key("user-funct-location", &nanos_instr_uf_location_key);
        if (nanos_err != ::NANOS_OK)
          {
            ::nanos_handle_error(nanos_err);
          }
        nanos_err = ::nanos_instrument_register_value_with_val((::nanos_event_value_t)(void (*)(void *))(void (*)(::nanos_args_0_t &))::fpga_ol_predict_or_update_model_1, "user-funct-location", "fpga_ol_predict_or_update_model_1@predict.cpp@132@FUNCTION", "void ::predict_or_update_model(::F_base [2L], ::P_base [4L], bool, ::U_base [4L][4L][8L], ::mu_base [4L][8L], ::B_base [4L][2L][8L])@predict.cpp@132@FUNCTION", 0);
        if (nanos_err != ::NANOS_OK)
          {
            ::nanos_handle_error(nanos_err);
          }
        nanos_funct_id_init = 1;
      }
    ::nanos_event_t event /* () */ ;
    event.type = ::NANOS_BURST_START;
    event.key = nanos_instr_uf_location_key;
    event.value = (::nanos_event_value_t)(void (*)(void *))(void (*)(::nanos_args_0_t &))::fpga_ol_predict_or_update_model_1;
    nanos_err = ::nanos_instrument_events(1, &event);
    ::fpga_ol_predict_or_update_model_1_unpacked(args.features, args.predictions, args.update_model, args.U_in, args.mu_in, args.B_in);
    event.type = ::NANOS_BURST_END;
    event.key = nanos_instr_uf_location_key;
    event.value = (::nanos_event_value_t)(void (*)(void *))(void (*)(::nanos_args_0_t &))::fpga_ol_predict_or_update_model_1;
    nanos_err = ::nanos_instrument_events(1, &event);
  }
}
void fpga_ol_predict_or_update_model_3_unpacked(::F_base *features, ::P_base *predictions, bool update_model, ::U_base (*U_in)[4L][8L], ::mu_base (*mu_in)[8L], ::B_base (*B_in)[2L][8L]) throw()
{
  union  mcc_union_anon_34
  {
      ::F_base *features;
      ::uint64_t features_task_arg;
  };
  mcc_union_anon_34 mcc_arg_8 /* () */ ;
  mcc_arg_8.features = (::F_base *)::nanos_fpga_get_phy_address((void *)features);
  ::nanos_fpga_set_task_arg(::nanos_current_wd(), 0, 1, 1, mcc_arg_8.features_task_arg);
  union  mcc_union_anon_35
  {
      ::P_base *predictions;
      ::uint64_t predictions_task_arg;
  };
  mcc_union_anon_35 mcc_arg_9 /* () */ ;
  mcc_arg_9.predictions = (::P_base *)::nanos_fpga_get_phy_address((void *)predictions);
  ::nanos_fpga_set_task_arg(::nanos_current_wd(), 1, 1, 1, mcc_arg_9.predictions_task_arg);
  union  mcc_union_anon_36
  {
      bool update_model;
      ::uint64_t update_model_task_arg;
  };
  mcc_union_anon_36 mcc_arg_10 /* () */ ;
  mcc_arg_10.update_model = update_model;
  ::nanos_fpga_set_task_arg(::nanos_current_wd(), 2, 1, 1, mcc_arg_10.update_model_task_arg);
  union  mcc_union_anon_37
  {
      ::U_base (*U_in)[4L][8L];
      ::uint64_t U_in_task_arg;
  };
  mcc_union_anon_37 mcc_arg_11 /* () */ ;
  mcc_arg_11.U_in = (::U_base (*)[4L][8L])::nanos_fpga_get_phy_address((void *)U_in);
  ::nanos_fpga_set_task_arg(::nanos_current_wd(), 3, 1, 1, mcc_arg_11.U_in_task_arg);
  union  mcc_union_anon_38
  {
      ::mu_base (*mu_in)[8L];
      ::uint64_t mu_in_task_arg;
  };
  mcc_union_anon_38 mcc_arg_12 /* () */ ;
  mcc_arg_12.mu_in = (::mu_base (*)[8L])::nanos_fpga_get_phy_address((void *)mu_in);
  ::nanos_fpga_set_task_arg(::nanos_current_wd(), 4, 1, 1, mcc_arg_12.mu_in_task_arg);
  union  mcc_union_anon_39
  {
      ::B_base (*B_in)[2L][8L];
      ::uint64_t B_in_task_arg;
  };
  mcc_union_anon_39 mcc_arg_13 /* () */ ;
  mcc_arg_13.B_in = (::B_base (*)[2L][8L])::nanos_fpga_get_phy_address((void *)B_in);
  ::nanos_fpga_set_task_arg(::nanos_current_wd(), 5, 1, 1, mcc_arg_13.B_in_task_arg);
  ;
}
static void fpga_ol_predict_or_update_model_3(::nanos_args_1_t &args) throw()
{
  {
    static int nanos_funct_id_init(0);
    static ::nanos_event_key_t nanos_instr_uf_location_key(0);
    ::nanos_err_t nanos_err;
    if (nanos_funct_id_init == 0)
      {
        nanos_err = ::nanos_instrument_get_key("user-funct-location", &nanos_instr_uf_location_key);
        if (nanos_err != ::NANOS_OK)
          {
            ::nanos_handle_error(nanos_err);
          }
        nanos_err = ::nanos_instrument_register_value_with_val((::nanos_event_value_t)(void (*)(void *))(void (*)(::nanos_args_1_t &))::fpga_ol_predict_or_update_model_3, "user-funct-location", "fpga_ol_predict_or_update_model_3@predict.cpp@142@FUNCTION", "void ::predict_or_update_model(::F_base [2L], ::P_base [4L], bool, ::U_base [4L][4L][8L], ::mu_base [4L][8L], ::B_base [4L][2L][8L])@predict.cpp@142@FUNCTION", 0);
        if (nanos_err != ::NANOS_OK)
          {
            ::nanos_handle_error(nanos_err);
          }
        nanos_funct_id_init = 1;
      }
    ::nanos_event_t event /* () */ ;
    event.type = ::NANOS_BURST_START;
    event.key = nanos_instr_uf_location_key;
    event.value = (::nanos_event_value_t)(void (*)(void *))(void (*)(::nanos_args_1_t &))::fpga_ol_predict_or_update_model_3;
    nanos_err = ::nanos_instrument_events(1, &event);
    ::fpga_ol_predict_or_update_model_3_unpacked(args.features, args.predictions, args.update_model, args.U_in, args.mu_in, args.B_in);
    event.type = ::NANOS_BURST_END;
    event.key = nanos_instr_uf_location_key;
    event.value = (::nanos_event_value_t)(void (*)(void *))(void (*)(::nanos_args_1_t &))::fpga_ol_predict_or_update_model_3;
    nanos_err = ::nanos_instrument_events(1, &event);
  }
}
extern "C"
{
  __attribute__((weak)) void nanos_needs_fpga_fun()
  {
  }
}
