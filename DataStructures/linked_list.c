#include "linked_list.h"

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