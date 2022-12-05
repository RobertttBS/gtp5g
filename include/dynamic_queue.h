#ifndef __DYNAMIC_QUEUE_H__
#define __DYNAMIC_QUEUE_H__

//#include <linux/ktime.h>
#include<linux/list.h>
#include <linux/types.h>
#include <linux/slab.h>

#include "pktinfo.h"

struct d_queue_data {
    struct hlist_node queue_node;
    struct sk_buff *skb;
    struct gtp5g_pktinfo pktinfo;
};

struct dynamic_queue {
    struct hlist_head queue_head;
    int front;
    int rear;
    int size;
};

extern void create_d_queue(struct dynamic_queue *);
extern void d_queue_push(struct dynamic_queue *, struct d_queue_data *);
extern void d_queue_pop(struct dynamic_queue *);
extern struct d_queue_data *d_queue_get_front(struct dynamic_queue *);
extern struct d_queue_data *d_queue_get_rear(struct dynamic_queue *);
extern void d_queue_display(struct dynamic_queue *);

#endif // __DYNAMIC_QUEUE_H__