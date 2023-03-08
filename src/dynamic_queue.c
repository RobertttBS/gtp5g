#include "dynamic_queue.h"

void create_d_queue(struct dynamic_queue *d_queue){
    d_queue->queue_head = (struct d_queue_data *)kmalloc(sizeof(struct d_queue_data), GFP_KERNEL);
    d_queue->queue_head->next = NULL;
    d_queue->queue_tail = NULL;
    d_queue->size = 0;
    /*
    INIT_HLIST_HEAD(&(d_queue->queue_head));
    d_queue->front = 1;
    d_queue->rear = 0;
    d_queue->size = 0;
    */
}

void d_queue_push(struct dynamic_queue *d_queue, struct sk_buff *skb, struct gtp5g_pktinfo *pktinfo){
    struct d_queue_data *new_node;
    new_node = (struct d_queue_data *)kmalloc(sizeof(struct d_queue_data), GFP_KERNEL);
    new_node->skb = *skb;
    new_node->pktinfo = *pktinfo;
    new_node->next = NULL;

    if(d_queue->size == 0)
        d_queue->queue_head->next = new_node;
    else
        d_queue->queue_tail->next = new_node;
    
    d_queue->queue_tail = new_node;
    d_queue->size++;
    /*
    hlist_add_head(&(q_data->queue_node), &(d_queue->queue_head));
    d_queue->rear++;
    d_queue->size++;
    */
}

void d_queue_pop(struct dynamic_queue *d_queue){
    struct d_queue_data *del_node;
    if(d_queue->size == 0){
        printk("This dynamic queue is empty!\n");
        return;
    }
    if(d_queue->size == 1)
        d_queue->queue_tail = NULL;

    del_node = d_queue->queue_head->next;
    d_queue->queue_head->next = del_node->next;
    kfree(del_node);
    d_queue->size--;
    /*
    int i = 0;
    struct d_queue_data *del_data;
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
    */
}

struct d_queue_data *d_queue_get_front(struct dynamic_queue *d_queue){
    if(d_queue->size == 0){
        printk("This dynamic queue is empty!\n");
        return NULL;
    }
    return d_queue->queue_head->next;
    /*
    int i = 0;
    struct d_queue_data *res_data;
    struct hlist_node *tmp_node;
    hlist_for_each_entry_safe(res_data, tmp_node, &(d_queue->queue_head), queue_node){
        i++;
        if(i == 1)
            break;
    }
    return res_data;
    */
}

struct d_queue_data *d_queue_get_rear(struct dynamic_queue *d_queue){
    if(d_queue->size == 0){
        printk("This dynamic queue is empty!\n");
        return NULL;
    }
    return d_queue->queue_tail;
    /*
    int i = 0;
    struct d_queue_data *res_data;
    struct hlist_node *tmp_node;
    hlist_for_each_entry_safe(res_data, tmp_node, &(d_queue->queue_head), queue_node){
        i++;
        if(i == d_queue->size)
            break;
    }
    return res_data;
    */
}

void d_queue_display(struct dynamic_queue *d_queue){
    struct d_queue_data *cur;
    if(d_queue->size == 0){
        printk("This dynamic queue is empty!\n");
        return;
    }
    cur = d_queue->queue_head;
    while(cur){
        printk("debug");
        cur = cur->next;
    }
    /*
    int i = 0;
    struct d_queue_data *res_data;
    struct hlist_node *tmp_node;
    hlist_for_each_entry_safe(res_data, tmp_node, &(d_queue->queue_head), queue_node){
        i++;
        printk("debug");
    }
    */
}