#ifndef __WRED_H__
#define __WRED_H__

//#include <linux/ktime.h>
//#include <linux/types.h>
#include <linux/random.h>

#include "dynamic_queue.h"
#include "clk_freq.h"

struct wred_profile {
    struct dynamic_queue d_queue;
    int low_limit;
    int high_limit;
    int drop_prob;
    int max_drop_prob;
    int avg_size;
    int wq;                       // weight factor
    uint8_t qfi;
    uint32_t d_queue_id;
    uint64_t last_update_time;
};

extern void wred_profile_config(struct wred_profile *, uint8_t, uint32_t, uint64_t);
extern void wred_processing(struct wred_profile *, struct dynamic_queue *, struct sk_buff *, struct gtp5g_pktinfo *);

#endif // __WRED_H__