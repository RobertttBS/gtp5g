#include <linux/version.h>
#include <linux/udp.h>
#include <linux/skbuff.h>
#include <linux/inetdevice.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/udp_tunnel.h>
#include <linux/kernel.h>

#include "qos_meter.h"

#define MTU 1500
// #define NSEC_PER_SEC 1000000000

int trtcm_param_config(struct trtcm_param *parameter, uint32_t gbr_high, uint8_t gbr_low, uint32_t mbr_high, uint8_t mbr_low) {
    if(parameter == NULL)
        return -1;
    
    parameter->cir = ((((uint64_t) gbr_high) << 8) + gbr_low); // Convert to bits per second
    parameter->pir = ((((uint64_t) mbr_high) << 8) + mbr_low);
    parameter->cbs = MTU * 20;
    parameter->pbs = MTU * 20;
    return 0;
}

int trtcm_runtime_config(struct trtcm_param *param, struct trtcm_runtime *runtime){

    if((param == NULL) || (runtime == NULL))
        return -1;
    
    runtime->l_time = ktime_get_ns();
    runtime->tc = param->cbs; // Initialize token bucket
    runtime->tp = param->pbs; // Initialize token bucket
    runtime->remainder_c = 0;
    runtime->remainder_bits_c = 0;
    runtime->remainder_p = 0;
    runtime->remainder_bits_p = 0;
    return 0;
}

/* Using / and % operator to calculate*/
char trtcm_color_blind_check(struct trtcm_param *param, struct trtcm_runtime *runtime, uint64_t time_cur, uint64_t pkt_len){
    uint64_t time_diff;
    uint64_t p_rate_high, p_rate_low, p_bits;
    uint64_t c_rate_high, c_rate_low, c_bits;
    /* Compute time diff and update last update time. */
    time_diff = time_cur - runtime->l_time;
    runtime->l_time = time_cur; // Update latest update time
    
    /* Compute c bucket */
    c_rate_low = param->cir % NSEC_PER_SEC;
    c_rate_high = param->cir / NSEC_PER_SEC;

    runtime->remainder_c += time_diff * c_rate_low;
    c_bits = time_diff * c_rate_high + runtime->remainder_c / NSEC_PER_SEC + runtime->remainder_bits_c;
    runtime->remainder_c = runtime->remainder_c % NSEC_PER_SEC;
    runtime->remainder_bits_c = c_bits & 7;
    
    runtime->tc += (c_bits >> 3);
    if(runtime->tc > param->cbs)
        runtime->tc = param->cbs;

    /* Compute p bucket */
    p_rate_high = param->pir / NSEC_PER_SEC;
    p_rate_low = param->pir % NSEC_PER_SEC;

    runtime->remainder_p += time_diff * p_rate_low;
    p_bits = time_diff * p_rate_high + runtime->remainder_p / NSEC_PER_SEC + runtime->remainder_bits_p;
    runtime->remainder_p = runtime->remainder_p % NSEC_PER_SEC;
    runtime->remainder_bits_p = p_bits & 7;
    
    runtime->tp += (p_bits >> 3);
    if(runtime->tp > param->pbs)
        runtime->tp = param->pbs;

    // printk("cir: %llu, pir: %llu\n", param->cir, param->pir);
    // printk("time diff: %llu, pkt_len: %llu bytes, tc_cur: %llu, tp_cur: %llu, c_bits: %llu, p_bits: %llu\n",\
    //         time_diff, pkt_len, tc_cur, tp_cur, c_bits, p_bits);
    
    // seems runtime->tp is not volatile

    /* Color marking, first check c bucket */ 
    if (runtime->tc >= pkt_len) {
        runtime->tc -= pkt_len;
        return 'G';
    } else if (runtime->tp >= pkt_len) {
        runtime->tp -= pkt_len;
        return 'Y';
    } else
        return 'R';
}

/* Using bitwise operator to speedup calculation, but there will be 7% error*/
char trtcm_color_blind_check_bitwise(struct trtcm_param *param, struct trtcm_runtime *runtime, uint64_t time_cur, uint64_t pkt_len){
    uint64_t time_diff, tp_cur, p_rate_high, p_rate_low, p_bits;
    // uint64_t tc_cur, c_rate_high, c_rate_low, c_bits;
    // Update token bucket
    time_diff = time_cur - runtime->l_time;
    runtime->l_time = time_cur; // Update latest update time
    // printk("cir: %llu pir: %llu\n", param->cir, param->pir);
    
    // Abort the cir, since we just need to implement MBR
    // c_rate_low = param->cir % NSEC_PER_SEC;
    // c_rate_high = param->cir / NSEC_PER_SEC;
    // c_bits = time_diff * c_rate_high + (time_diff * c_rate_low) / NSEC_PER_SEC;
    // tc_cur = runtime->tc + (c_bits >> 3);
    // if(tc_cur > param->cbs)
    //     tc_cur = param->cbs;

    p_rate_high = param->pir >> 30;
    p_rate_low = param->pir & ((1ULL << 30) - 1);
    
    runtime->remainder_p += time_diff * p_rate_low;
    p_bits = time_diff * p_rate_high + (runtime->remainder_p >> 30) + runtime->remainder_bits_p;
    runtime->remainder_p = runtime->remainder_p & ((1ULL << 30) - 1);
    runtime->remainder_bits_p = p_bits & 7;
    tp_cur = runtime->tp + (p_bits >> 3);
    if(tp_cur > param->pbs)
        tp_cur = param->pbs;
    printk("rate: %llu, time_diff: %llu, p_bits: %llu, tp: %llu, tp_cur: %llu, pkt_len: %lld\n", param->pir, time_diff, p_bits, runtime->tp, tp_cur, pkt_len);

    // Color marking
    if(tp_cur < pkt_len){
        // runtime->tc = tc_cur;
        runtime->tp = tp_cur;
        // printk("Red\n");
        return 'R';
    }
    // if(tc_cur < pkt_len){
    //     runtime->tc = tc_cur;
    //     runtime->tp = tp_cur - pkt_len;
    //     // printk("Yellow\n");
    //     return 'Y';
    // }
    // runtime->tc = tc_cur - pkt_len;
    runtime->tp = tp_cur - pkt_len;
    // printk("Green\n");
    return 'G';
}