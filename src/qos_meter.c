#include <linux/version.h>
#include <linux/udp.h>
#include <linux/skbuff.h>
#include <linux/inetdevice.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/udp_tunnel.h>
#include <linux/kernel.h>

#include "qos_meter.h"

#define TB_MIN_PERIOD 400000
#define MTU 1500

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
    
    runtime->l_time = ktime_get();
    runtime->tc = param->cbs; // Initialize token bucket
    runtime->tp = param->pbs; // Initialize token bucket
    runtime->remainder = 0;
    return 0;
}

char trtcm_color_blind_check(struct trtcm_param *param, struct trtcm_runtime *runtime, uint64_t time_cur, uint64_t pkt_len){
    uint64_t time_diff, tp_cur, p_rate_high, p_rate_low, p_bits;
    // uint64_t tc_cur, c_rate_high, c_rate_low, c_bits;
    // Update token bucket
    time_diff = time_cur - runtime->l_time;
    runtime->l_time = time_cur; // Update latest update time
    // printk("cir: %llu pir: %llu\n", param->cir, param->pir);
    
    // Abort the cir, since we just need to implement MBR
    // c_rate_low = param->cir % 1000000000;
    // c_rate_high = param->cir / 1000000000;
    // c_bits = time_diff * c_rate_high + (time_diff * c_rate_low) / 1000000000;
    // tc_cur = runtime->tc + (c_bits >> 3);
    // if(tc_cur > param->cbs)
    //     tc_cur = param->cbs;

    p_rate_high = param->pir / 1000000000;
    p_rate_low = param->pir % 1000000000;
    
    runtime->remainder += time_diff * p_rate_low;
    p_bits = time_diff * p_rate_high + runtime->remainder / 1000000000;
    runtime->remainder = runtime->remainder % 1000000000;
    tp_cur = runtime->tp + (p_bits >> 3);
    if(tp_cur > param->pbs)
        tp_cur = param->pbs;
    // printk("time_diff: %llu, c_bits: %llu, tc_cur: %llu, p_bits: %llu, tp_cur: %llu\n", time_diff, c_bits, tc_cur, p_bits, tp_cur);
    

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