#include "wred.h"

void wred_profile_config(struct wred_profile *queue_profile, int queue_idx, uint64_t config_time){
    
    create_queue(&(queue_profile->s_queue), 1024);
    queue_profile->queue_id = queue_idx;
    queue_profile->last_update_time = config_time;
}

void wred_processing(struct wred_profile *queue_profile, struct static_queue *s_queue, struct sk_buff *skb, struct gtp5g_pktinfo *pktinfo){
    uint64_t cur_time, idle_time;
    int old_avg_size;
    cur_time = get_tsc();
    idle_time = cur_time - queue_profile->last_update_time;
    old_avg_size = queue_profile->avg_size;
    if(s_queue->size == 0){
        queue_profile->avg_size = ((100 - queue_profile->wq) * old_avg_size);
        queue_profile->avg_size /= 100;
        queue_profile->last_update_time = cur_time;
    }
    else{
        queue_profile->avg_size = ((100 - queue_profile->wq) * old_avg_size) + (queue_profile->wq * s_queue->size);
        queue_profile->avg_size /= 100;
        queue_profile->last_update_time = cur_time;
    }
}