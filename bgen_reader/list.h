#ifndef LIST_H
#define LIST_H

#include "byte.h"

struct node
{
    void        *data;
    struct node *next;
};

inline static void push(struct node **head_ref, void *new_data, size_t data_size)
{
    struct node *new_node = (struct node *)malloc(sizeof(struct node));

    new_node->data = new_data;
    new_node->next = (*head_ref);

    (*head_ref) = new_node;
}

#endif /* end of include guard: LIST_H */
