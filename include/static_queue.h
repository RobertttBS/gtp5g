#ifndef __STATIC_QUEUE_H__
#define __STATIC_QUEUE_H__

//#include <linux/ktime.h>
//#include <linux/types.h>
#include <linux/slab.h>

#include "pktinfo.h"

struct queue_data {
    struct sk_buff *skb;
    struct gtp5g_pktinfo pktinfo;
};

struct static_queue {
    struct queue_data *data;
    int front;
    int rear;
    int size;
    int capacity;
};

extern void create_queue(struct static_queue *, int);
extern void queue_push(struct static_queue *, struct queue_data);
extern void queue_pop(struct static_queue *);
extern struct queue_data get_front(struct static_queue *);
extern struct queue_data get_rear(struct static_queue *);
extern void display(struct static_queue *);

#endif // __STATIC_QUEUE_H__