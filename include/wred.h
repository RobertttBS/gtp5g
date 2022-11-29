#ifndef __WRED_H__
#define __WRED_H__

//#include <linux/ktime.h>
//#include <linux/types.h>
#include <linux/random.h>

#include "static_queue.h"
#include "clk_freq.h"

struct wred_profile {
    struct static_queue s_queue;
    int queue_id;
    int low_limit;
    int high_limit;
    int drop_prob;
    int max_drop_prob;
    int avg_size;
    int wq;                       // weight factor
    uint64_t last_update_time;
};

extern void wred_profile_config(struct wred_profile *, int, uint64_t);
extern void wred_processing(struct wred_profile *, struct static_queue *, struct sk_buff *, struct gtp5g_pktinfo *);

#endif // __WRED_H__