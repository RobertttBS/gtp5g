#include "clk_freq.h"

uint64_t get_tsc(void){ // TSC == Time Stamp Counter
#ifdef __i386__
    uint64_t x;
    __asm__ volatile("rdtsc" : "=A"(x));
    return x;
#elif defined(__amd64__) || defined(__x86_64__)
    uint64_t a, d;
    __asm__ volatile("rdtsc" : "=a"(a), "=d"(d));
    return (d << 32) | a;
#else // ARM CPU
    uint32_t cc = 0;
    __asm__ volatile ("mrc p15, 0, %0, c9, c13, 0":"=r" (cc));
    return (uint64_t)cc; 
#endif
}

uint32_t get_ms_cnt(void){
    struct timespec64 t_spec;
    ktime_get_real_ts64(&t_spec);

    return (uint32_t) (t_spec.tv_sec * 1000 + t_spec.tv_nsec / 1000000);
}

uint64_t get_cpu_freq(void){
    uint64_t clks = get_tsc(), clks_now;
    uint32_t msec = get_ms_cnt(), msec_diff;
    int last_res = 0;
    for(;;){
        int n = 1000000;
        while(--n > 0){}
        clks_now = get_tsc();
        msec_diff = get_ms_cnt() - msec;
        if(msec_diff > 80){
            const int new_res = (int)((clks_now - clks) / msec_diff / 1000);
            if(msec_diff > 500 || (last_res == new_res && new_res > 100))
                return new_res;
            last_res = new_res;
        }
    }
}