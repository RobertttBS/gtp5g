#include "wred.h"

int wred_processing(struct wred_profile *queue_profile, struct dynamic_queue *d_queue, struct sk_buff *skb, struct gtp5g_pktinfo *pktinfo){
    uint64_t cur_time, idle_time;
    int old_avg_size, tmp_avg_size, random_num, mark_prob, drop_prob;
    int green_low_limit = queue_profile->low_limit[0], green_high_limit = queue_profile->high_limit[0], green_max_drop_prob = queue_profile->max_drop_prob[0];
    int yellow_low_limit = queue_profile->low_limit[1], yellow_high_limit = queue_profile->high_limit[1], yellow_max_drop_prob = queue_profile->max_drop_prob[1];
    
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

    tmp_avg_size = queue_profile->avg_size;
    if(pktinfo->color == 'G'){ // green packets
        // low limit < average queue size <= high limit, prob buf or prob drop
        if(green_low_limit < tmp_avg_size && tmp_avg_size <= green_high_limit){
            queue_profile->count[0] += 1;
            drop_prob = (green_max_drop_prob * (tmp_avg_size - green_low_limit)) / (green_high_limit - green_low_limit);
            if(queue_profile->count[0] * drop_prob < 100)
                mark_prob = (100 * drop_prob) / (100 - (queue_profile->count[0] * drop_prob));
            else
                mark_prob = drop_prob;
            get_random_bytes(&random_num, sizeof(random_num));
            if(random_num < mark_prob){ // drop
                queue_profile->count[0] = 0;
                return 0;
            }
            else{ // buf
                struct d_queue_data tmp_queue_data;
                tmp_queue_data.skb = *skb;
                tmp_queue_data.pktinfo = *pktinfo;
                d_queue_push(&(queue_profile->d_queue), &tmp_queue_data);
                return 1;
            }
        }
        // average queue size <= low limit, buf
        else if(tmp_avg_size <= green_low_limit){
            struct d_queue_data tmp_queue_data;
            tmp_queue_data.skb = *skb;
            tmp_queue_data.pktinfo = *pktinfo;
            d_queue_push(&(queue_profile->d_queue), &tmp_queue_data);
            queue_profile->count[0] = -1;
            return 1;
        }
        // average queue size > high limit, drop
        else{
            queue_profile->count[0] = 0;
            return 0;
        }
    }
    else{ // yellow packets
        // low limit <= average queue size < high limit, prob buf or prob drop
        if(yellow_low_limit < tmp_avg_size && tmp_avg_size <= yellow_high_limit){
            queue_profile->count[1] += 1;
            drop_prob = (yellow_max_drop_prob * (tmp_avg_size - yellow_low_limit)) / (yellow_high_limit - yellow_low_limit);
            if(queue_profile->count[1] * drop_prob < 100)
                mark_prob = (100 * drop_prob) / (100 - (queue_profile->count[1] * drop_prob));
            else
                mark_prob = drop_prob;
            get_random_bytes(&random_num, sizeof(random_num));
            if(random_num < mark_prob){ // drop
                queue_profile->count[1] = 0;
                return 0;
            }
            else{ // buf
                struct d_queue_data tmp_queue_data;
                tmp_queue_data.skb = *skb;
                tmp_queue_data.pktinfo = *pktinfo;
                d_queue_push(&(queue_profile->d_queue), &tmp_queue_data);
                return 1;
            }
        }
        // average queue size < low limit, buf
        else if(tmp_avg_size <= yellow_low_limit){
            struct d_queue_data tmp_queue_data;
            tmp_queue_data.skb = *skb;
            tmp_queue_data.pktinfo = *pktinfo;
            d_queue_push(&(queue_profile->d_queue), &tmp_queue_data);
            queue_profile->count[1] = -1;
            return 1;
        }
        // average queue size >= high limit, drop
        else{
            queue_profile->count[1] = 0;
            return 0;
        }
    }
}