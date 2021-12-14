
#include <pmu.h>

char const * const pmu_event_descr[] = {
    [L1D_CACHE_REFILL] = "L1$D refill/miss",
    [L1D_CACHE] = "L1$D access",
    [L2D_CACHE_REFILL] = "L2$D refill/miss",
    [L2D_CACHE] = "L2$D access",
    [L1D_CACHE_WB] = "L1$D writebacks",
    [L2D_CACHE_WB] = "L2$D writebacks",
    [L1D_CACHE_ALLOCATE] = "L1$D allocates",
    [L2D_CACHE_ALLOCATE] = "L2$D allocates",
    [LL_CACHE] = "LLC accesses",
    [LL_CACHE_MISS] = "LLC misses",
    [LINEFILL_PREFETCH] = "Linefill because of prefetch",
    [MEM_ACCESS] = "Memory accesses",
    [BUS_ACCESS] = "Bus access"
};

size_t pmu_num_counters() {
    return (size_t) bit_extract(MRS(PMCR_EL0), PMCR_N_OFF, PMCR_N_LEN);
}

static inline void pmu_counter_enable(int counter, bool en){
    uint64_t val = (1ULL << counter);
    if(en){
        MSR(PMCNTENSET_EL0, val);
    } else {
        MSR(PMCNTENCLR_EL0, val);
    }
}

static inline bool pmu_counter_enabled(int counter){
    uint64_t val = 0;
    MRS(val, PMCNTENCLR_EL0);
    return (1ULL << counter) & val;
}

static inline void pmu_counter_set_event(int counter, enum PMU_EVENT event){
    MSR(PMSELR_EL0, counter);
    //barrier?
    MSR(PMXEVTYPER_EL0, event);
}

static inline enum PMU_EVENT pmu_counter_get_event(int counter){
    MSR(PMSELR_EL0, counter);
    uint64_t event = 0;
    MRS(event, PMXEVTYPER_EL0);
    return event;
}

static inline uint64_t pmu_counter_get(int counter){
    MSR(PMSELR_EL0, counter);
    //barrier?
    uint64_t val = 0;
    MRS(val, PMXEVCNTR_EL0);
    return val;
}

static inline void pmu_cycle_enable(bool en){
    uint64_t val = (1ULL << 31);
    if(en){
        MSR(PMCNTENSET_EL0, val);
    } else {
        MSR(PMCNTENCLR_EL0, val);
    }
}

static inline uint64_t pmu_cycle_get(){
    uint64_t val = 0;
    MRS(val, PMCCNTR_EL0);
    return val;
}

void pmu_setup(int n, uint32_t events[]){
    for(int i = 0; i < n && i < 31; i++){
        pmu_counter_set_event(i, events[i]);
        pmu_counter_enable(i, true);
    }
    pmu_cycle_enable(true);
}

// void pmu_print_results(){
//     for(int i = 0; i < 31; i++){
//         if(pmu_counter_enabled(i)){
//             uint64_t count = pmu_counter_get(i);
//             uint32_t event = pmu_counter_get_event(i);
//             char const * descr =  pmu_event_descr[event]; 
//             descr = descr ? descr : "unknown event";
//             printf("%s (0x%x): %d \n", descr, event, count);
//         }
//     }
//     printf("cycles: %d\n\n", pmu_cycle_get());
// }
