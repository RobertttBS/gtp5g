#include <linux/version.h>
#include <linux/udp.h>
#include <linux/skbuff.h>
#include <linux/inetdevice.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/udp_tunnel.h>
#include <linux/kernel.h>


#include "clk_freq.h"
#include "qos_meter.h"

#define TB_MIN_PERIOD 400000
#define MTU 1500

static void get_tb_parameters(uint64_t cpu_freq, uint64_t rate, uint64_t *tb_period, uint64_t *tb_bytes_per_period){
    uint64_t period;
    if(rate == 0){
        *tb_period = TB_MIN_PERIOD;
        *tb_bytes_per_period = 0;
        return;
    }
    // Remember, the unit of cpu frequency is MHz
    period = ((cpu_freq * 1000) / rate) * 1000;
    if(period >= TB_MIN_PERIOD){
        *tb_period = period;
        *tb_bytes_per_period = 1;
    }
    else{
        *tb_bytes_per_period = DIV_ROUND_UP(TB_MIN_PERIOD, period);
        *tb_period = ((cpu_freq * 1000 * (*tb_bytes_per_period)) / rate) * 1000;
    }
}

int trtcm_param_config(struct trtcm_param *parameter, uint32_t gbr, uint32_t mbr){
    if(parameter == NULL)
        return -1;
    
    parameter->cir = (uint64_t) (gbr * 1000 / 8);
    parameter->pir = (uint64_t) (mbr * 1000 / 8);
    parameter->cbs = MTU * 10;
    parameter->pbs = MTU * 10;
    return 0;
}

int trtcm_profile_config(struct trtcm_param *parameter, struct trtcm_profile *profile){
    // The unit of cpu frequency is MHz
    uint64_t cpu_freq = get_cpu_freq();
    
    if((profile == NULL) || (parameter->cir == 0) || (parameter->pir == 0) ||
        (parameter->pir < parameter->cir) || (parameter->cbs == 0) || (parameter->pbs == 0))
        return -1;
    
    profile->cbs = parameter->cbs;
    profile->pbs = parameter->pbs;
    get_tb_parameters(cpu_freq, parameter->cir, &profile->cir_period, &profile->cir_bytes_per_period);
    get_tb_parameters(cpu_freq, parameter->pir, &profile->pir_period, &profile->pir_bytes_per_period);
    return 0;
}

int trtcm_runtime_config(struct trtcm_profile *profile, struct trtcm_runtime *runtime){

    if((profile == NULL) || (runtime == NULL))
        return -1;
    
    runtime->time_tc = runtime->time_tp = get_tsc();
    runtime->tc = profile->cbs;
    runtime->tp = profile->pbs;
    return 0;
}

char trtcm_color_blind_check(struct trtcm_profile *profile, struct trtcm_runtime *runtime, uint64_t time_cur, uint64_t pkt_len){
    uint64_t tc_time_diff, tp_time_diff, tc_period_diff, tp_period_diff, tc_cur, tp_cur;
    
    // Update token bucket
    tc_time_diff = time_cur - runtime->time_tc;
    tp_time_diff = time_cur - runtime->time_tp; 
    tc_period_diff = tc_time_diff / profile->cir_period;
    tp_period_diff = tp_time_diff / profile->pir_period;
    runtime->time_tc += tc_period_diff * profile->cir_period;
    runtime->time_tp += tp_period_diff * profile->pir_period;

    tc_cur = runtime->tc + tc_period_diff * profile->cir_bytes_per_period;
    if(tc_cur > profile->cbs)
        tc_cur = profile->cbs;
    printk("After adding token, tc: %llu\n", tc_cur);


    tp_cur = runtime->tp + tp_period_diff * profile->pir_bytes_per_period;
    if(tp_cur > profile->pbs)
        tp_cur = profile->pbs;
    printk("After adding token, tp: %llu\n", tp_cur);

    // Color marking
    if(tp_cur < pkt_len){
        runtime->tc = tc_cur;
        runtime->tp = tp_cur;
        printk("Red\n");
        return 'R';
    }
    if(tc_cur < pkt_len){
        runtime->tc = tc_cur;
        runtime->tp = tp_cur - pkt_len;
        printk("Yellow\n");
        return 'Y';
    }
    runtime->tc = tc_cur - pkt_len;
    runtime->tp = tp_cur - pkt_len;
    printk("Green\n");
    return 'G';
}