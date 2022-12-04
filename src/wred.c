#include "wred.h"

void wred_profile_config(struct wred_profile *queue_profile, uint8_t qos_flow_id, uint32_t queue_idx, uint64_t config_time){
    
    create_d_queue(&(queue_profile->d_queue));
    queue_profile->qfi = qos_flow_id;
    queue_profile->d_queue_id = queue_idx;
    queue_profile->last_update_time = config_time;
}

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