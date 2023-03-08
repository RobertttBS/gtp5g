#include "static_queue.h"

void create_s_queue(struct static_queue *s_queue, int id, int max_element){
    s_queue->data = (struct s_queue_data *)kmalloc(sizeof(struct s_queue_data) * max_element, GFP_KERNEL);
    s_queue->s_queue_id = id;
    s_queue->front = 0;
    s_queue->rear = -1;
    s_queue->size = 0;
    s_queue->capacity = max_element;
}

void s_queue_push(struct static_queue *s_queue, struct sk_buff *skb, struct gtp5g_pktinfo *pktinfo){
    struct s_queue_data new_data;
    if(s_queue->size == s_queue->capacity){
        printk("This static queue is full!\n");
        return;
    }
    s_queue->size++;
    s_queue->rear++;

    if(s_queue->rear == s_queue->capacity)
        s_queue->rear = 0;
    
    new_data.skb = *skb;
    new_data.pktinfo = *pktinfo;
    s_queue->data[s_queue->rear] = new_data;
}

void s_queue_pop(struct static_queue *s_queue){
    if(s_queue->size == 0){
        printk("This static queue is empty!\n");
        return;
    }
    s_queue->size--;
    s_queue->front++;

    if(s_queue->front == s_queue->capacity)
        s_queue->front = 0;
}

struct s_queue_data s_queue_get_front(struct static_queue *s_queue){
    if(s_queue->size == 0){
        struct s_queue_data tmp;
        printk("This static queue is empty!\n");
        return tmp;
    }
    return s_queue->data[s_queue->front];
}

struct s_queue_data s_queue_get_rear(struct static_queue *s_queue){
    if(s_queue->size == 0){
        struct s_queue_data tmp;
        printk("This static queue is empty!\n");
        return tmp;
    }
    return s_queue->data[s_queue->rear];
}

void s_queue_display(struct static_queue *s_queue){
    int i;
    if(s_queue->size == 0){
        printk("This static queue is empty!\n");
        return;
    }
    if(s_queue->front > s_queue->rear){
        for(i = s_queue->front; i < s_queue->capacity; i++)
            printk("debug");
        for(i = 0; i <= s_queue->rear; i++)
            printk("debug");

        printk("\n");
    }
    else{
        for(i = s_queue->front; i <= s_queue->rear; i++)
            printk("debug");
        
        printk("\n");
    }
}