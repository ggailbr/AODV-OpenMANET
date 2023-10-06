#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdlib.h>

typedef struct linked_entry_p{
    uint32_t data;
    struct linked_entry_p *next;
    struct linked_entry_p *prev;
}linked_entry;

typedef struct{
    linked_entry *first;
    linked_entry *last;
}linked_list;

/**
 * \brief Frees the linked list and all entries
 * 
 * @param list The list to free
 */
void free_linked_list(linked_list * list);

#endif