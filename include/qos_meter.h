#ifndef __QOS_METER_H__
#define __QOS_METER_H__

#include <linux/netdevice.h>
#include <linux/socket.h>
#include <linux/kernel.h>
#include <linux/net.h>
#include <linux/rculist.h>
#include <linux/range.h>
#include <linux/skbuff.h>
#include <linux/un.h>
#include <net/ip.h>

struct trtcm_param{
    uint64_t cir;    // Committed Information Rate, bytes per second
    uint64_t cbs;    // Committed Burst Size, bytes
    uint64_t pir;    // Peak Information Rate, bytes per second
    uint64_t pbs;    // Peak Burst Size, bytes
};

struct trtcm_profile{
    uint64_t cbs;                     // Max size of C token bucket
    uint64_t pbs;                     // Max size of P token bucket
    uint64_t cir_period;              // Number of CPU clks for each updtae of C token bucket
    uint64_t cir_bytes_per_period;    // Number of bytes add to C token buckets on each update
    uint64_t pir_period;              // Number of CPU clks for each updtae of P token bucket
    uint64_t pir_bytes_per_period;    // Number of bytes add to P token buckets on each update
};

struct trtcm_runtime{
    uint64_t tc;         // Number of bytes available in C token bucket now
    uint64_t time_tc;    // Latest update time of C token bucket
    uint64_t tp;         // Number of bytes available in P token bucket now
    uint64_t time_tp;    // Latest update time of P token bucket
};

extern int trtcm_param_config(struct trtcm_param *, uint32_t, uint32_t);
extern int trtcm_profile_config(struct trtcm_param *, struct trtcm_profile *);
extern int trtcm_runtime_config(struct trtcm_profile *, struct trtcm_runtime *);
extern char trtcm_color_blind_check(struct trtcm_profile *, struct trtcm_runtime *, uint64_t, uint64_t);
#endif // __QOS_METER_H__