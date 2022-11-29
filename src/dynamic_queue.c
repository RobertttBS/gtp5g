#include "dynamic_queue.h"

void create_d_queue(struct dynamic_queue *d_queue){
    INIT_HLIST_HEAD(&(d_queue->queue_head));
    d_queue->front = 1;
    d_queue->rear = 0;
    d_queue->size = 0;
}

void d_queue_push(struct dynamic_queue *d_queue, struct queue_data *q_data){
    hlist_add_head(&(q_data->queue_node), &(d_queue->queue_head));
    d_queue->rear++;
    d_queue->size++;
}

void d_queue_pop(struct dynamic_queue *d_queue){
    int i = 0;
    struct queue_data *del_data;
    struct hlist_node *tmp_node;
    hlist_for_each_entry_safe(del_data, tmp_node, &(d_queue->queue_head), queue_node){
        i++;
        if(i == 1){
            hlist_del(&(del_data->queue_node));
            break;
        }
    }
    d_queue->rear--;
    d_queue->size--;
}

struct queue_data *d_queue_get_front(struct dynamic_queue *d_queue){
    int i = 0;
    struct queue_data *res_data;
    struct hlist_node *tmp_node;
    hlist_for_each_entry_safe(res_data, tmp_node, &(d_queue->queue_head), queue_node){
        i++;
        if(i == 1)
            break;
    }
    return res_data;
}

struct queue_data *d_queue_get_rear(struct dynamic_queue *d_queue){
    int i = 0;
    struct queue_data *res_data;
    struct hlist_node *tmp_node;
    hlist_for_each_entry_safe(res_data, tmp_node, &(d_queue->queue_head), queue_node){
        i++;
        if(i == d_queue->size)
            break;
    }
    return res_data;
}

void d_queue_display(struct dynamic_queue *d_queue){
    int i = 0;
    struct queue_data *res_data;
    struct hlist_node *tmp_node;
    hlist_for_each_entry_safe(res_data, tmp_node, &(d_queue->queue_head), queue_node){
        i++;
        printk("debug");
    }
}