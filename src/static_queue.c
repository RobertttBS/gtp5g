#include "static_queue.h"

void create_queue(struct static_queue *s_queue, int max_element){
    s_queue->data = (struct queue_data *)kmalloc(sizeof(struct queue_data) * max_element, GFP_KERNEL);
    s_queue->front = 0;
    s_queue->rear = -1;
    s_queue->size = 0;
    s_queue->capacity = max_element;
}

void queue_push(struct static_queue *s_queue, struct queue_data q_data){
    if(s_queue->size == s_queue->capacity){
        printk("This static queue is full!\n");
        return;
    }
    s_queue->size++;
    s_queue->rear++;

    if(s_queue->rear == s_queue->capacity)
        s_queue->rear = 0;
    
    s_queue->data[s_queue->rear] = q_data;
}

void queue_pop(struct static_queue *s_queue){
    if(s_queue->size == 0){
        printk("This static queue is empty!\n");
        return;
    }
    s_queue->size--;
    s_queue->front++;

    if(s_queue->front == s_queue->capacity)
        s_queue->front = 0;
}

struct queue_data get_front(struct static_queue *s_queue){
    if(s_queue->size == 0){
        struct queue_data tmp;
        printk("This static queue is empty!\n");
        return tmp;
    }
    return s_queue->data[s_queue->front];
}

struct queue_data get_rear(struct static_queue *s_queue){
    if(s_queue->size == 0){
        struct queue_data tmp;
        printk("This static queue is empty!\n");
        return tmp;
    }
    return s_queue->data[s_queue->rear];
}

void display(struct static_queue *s_queue){
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