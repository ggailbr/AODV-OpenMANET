#ifndef LINKED_LIST_H
#define LINKED_LIST_H


typedef struct{
    void *data;
    linked_entry *next;
    linked_entry *prev;
}linked_entry;

typedef struct{
    linked_entry *first;
    linked_entry *last;
}linked_list;

#endif