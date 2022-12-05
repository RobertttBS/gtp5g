#include "wred.h"

void wred_processing(struct wred_profile *queue_profile, struct dynamic_queue *d_queue, struct sk_buff *skb, struct gtp5g_pktinfo *pktinfo){
    uint64_t cur_time, idle_time;
    int old_avg_size;
    cur_time = get_tsc();
    idle_time = cur_time - queue_profile->last_update_time;
    old_avg_size = queue_profile->avg_size;
    if(d_queue->size == 0){
        queue_profile->avg_size = ((100 - queue_profile->wq) * old_avg_size);
        queue_profile->avg_size /= 100;
        queue_profile->last_update_time = cur_time;
    }
    else{
        queue_profile->avg_size = ((100 - queue_profile->wq) * old_avg_size) + (queue_profile->wq * d_queue->size);
        queue_profile->avg_size /= 100;
        queue_profile->last_update_time = cur_time;
    }
}