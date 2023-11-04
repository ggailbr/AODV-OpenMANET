#include "linked_list.h"
#include <stdint.h>

void free_linked_list(linked_list * list){
    if(list == NULL){
        return;
    }
    linked_entry *entry = list->first;
    linked_entry *next = NULL;
    if(entry == NULL){
        return;
    }
    else{
        while(entry != NULL){
            next = entry->next;
            free(entry);
            entry = next;
        }
        free(list);
    }
}

void add_entry_to_list(linked_list * list, uint32_t data){
    if(list == NULL){
        debprintf("[ERROR] : Linked List not Initialized\n");
        return;
    }
    linked_entry * new_entry = (linked_entry *)malloc(sizeof(linked_entry));
    new_entry->data = data;
    new_entry->next = NULL;
    new_entry->prev = NULL;
    // The only entry
    if(list->first == NULL){
        list->first = (list->last = new_entry);
    }
    else{
        list->last->next = new_entry;
        new_entry->prev = list->last;
        list->last = new_entry;
    }
}

void remove_entry_from_list(linked_list * list, uint32_t data){
    if(list == NULL){
        debprintf("[ERROR] : Linked List not Initialized\n");
        return;
    }
    linked_entry* test_entry = list->first;
    // If it is the first entry in the list
    if(test_entry->data == data){
        // If it is the only entry in the list
        if(test_entry->next == NULL){
            free(test_entry);
            list->first = (list->last = NULL);
            return;
        }
        list->first = test_entry->next;
        list->first->prev = NULL;
        free(test_entry);
        return;
    }
    test_entry = test_entry->next;
    while(test_entry != NULL){
        if(test_entry->data == data){
            // If it is the last entry in the list
            if(test_entry->next == NULL){
                list->last = test_entry->prev;
                list->last->next = NULL;
                free(test_entry);
                return;
            }
            test_entry->prev->next = test_entry->next;
            test_entry->next->prev = test_entry->prev;
            free(test_entry);
            return;
        }
        test_entry = test_entry->next;
    }
}

uint32_t * get_all_entries(linked_list * list, uint32_t *size){
    if(list == NULL){
        debprintf("[ERROR] : Linked List not Initialized\n");
        return;
    }
    linked_entry *cur_entry = list->first;
    // First total number of entries
    *size = 0;
    while(cur_entry != NULL){
        (*size)++;
        cur_entry = cur_entry->next;
    }
    cur_entry = list->first;
    uint32_t * ret_array = (uint32_t *) malloc(sizeof(uint32_t) * (*size));
    uint32_t index = 0;
    while(cur_entry != NULL){
        ret_array[index] = cur_entry->data;
        cur_entry = cur_entry->next;
        index++;
    }
    return ret_array;
}

uint8_t is_in_list(linked_list * list, uint32_t data){
    linked_entry *current_entry = list->first;
    while(current_entry != NULL){
        if(current_entry->data == data)
            return 1;
        current_entry = current_entry->next;
    }
    return 0;
}