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
    uint64_t cir;    // Committed Information Rate, bits per nano second
    uint64_t cbs;    // Committed Burst Size
    uint64_t pir;    // Peak Information Rate, bits per nano second
    uint64_t pbs;    // Peak Burst Size
};

struct trtcm_runtime{
    uint64_t tc;         // Number of bytes available in C token bucket now
    uint64_t tp;         // Number of bytes available in P token bucket now
    uint64_t l_time;    // Latest update time of token buckets
    uint64_t remainder; // Remainder of the last update(`/ 1000000000` or `>> 30`), for precision
};

extern int trtcm_param_config(struct trtcm_param *, uint32_t, uint8_t, uint32_t, uint8_t);
extern int trtcm_runtime_config(struct trtcm_param *, struct trtcm_runtime *);
extern char trtcm_color_blind_check(struct trtcm_param *, struct trtcm_runtime *, uint64_t, uint64_t);
#endif // __QOS_METER_H__