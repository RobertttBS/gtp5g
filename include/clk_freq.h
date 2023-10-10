#ifndef __CLK_FREQ_H__
#define __CLK_FREQ_H__

#include <linux/ktime.h>
#include <linux/types.h>

extern uint64_t get_tsc(void);       // Count how many CPU clks have passed begin from boot
extern uint32_t get_ms_cnt(void);    // Count how many miliseconds have passed begin from boot
extern uint64_t get_cpu_freq(void);       // Calculate CPU frequency

#endif // __CLK_FREQ_H__