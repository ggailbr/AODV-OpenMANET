#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdlib.h>
#include <stdint.h>
#include "debug.h"
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

/**
 * \brief Adds an entry to the end of the linked list
 * 
 * @param list The list to add the entry
 * @param data The data to store
 */
void add_entry_to_list(linked_list * list, uint32_t data);
/**
 * \brief Removes an entry to the end of the linked list
 * 
 * @param list The list to remove the entry from
 * @param data The data to remove
 */
void remove_entry_from_list(linked_list * list, uint32_t data);

/**
 * \brief Returns all data in the linked list as an array
 * @param list The list to pull all data from
 * @param[out] size Is replaced with the number of entries in the array
 * @return A pointer to the array containing all data
 */
uint32_t * get_all_entries(linked_list * list, uint32_t *size);

#endif