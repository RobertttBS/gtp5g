#ifndef __WRED_H__
#define __WRED_H__

//#include <linux/ktime.h>
//#include <linux/types.h>
#include <linux/random.h>

#include "dynamic_queue.h"
#include "clk_freq.h"

struct wred_profile {
    struct dynamic_queue d_queue;
    int *low_limit;               // low_limit[0] for green packets, low_limit[1] for yellow packets
    int *high_limit;              // high_limit[0] for green packets, high_limit[1] for yellow packets
    int *max_drop_prob;           // max_drop_prob[0] for green packets, max_drop_prob[1] for yellow packets
    int *count;                    // count of packets since last probabilistic drop
    int avg_size;
    int wq;                       // weight factor
    uint8_t qfi;
    uint32_t d_queue_id;
    uint64_t last_update_time;
};

extern int wred_processing(struct wred_profile *, struct dynamic_queue *, struct sk_buff *, struct gtp5g_pktinfo *);

#endif // __WRED_H__