#include "priority_linked_list.h"

#include "linked_list.h"

#include <stdio.h>
#include <assert.h>

void PriorityList_init(PriorityListHead* head){
    assert(head && "");
    for(int i=0; i<PRIORITY_LEVELS; i++)
        List_init(&(head->priority_level_list[i]));
    head->size = 0;
    return;
}

PriorityListItem* PriorityList_insert(PriorityListHead* head, PriorityListItem* item){
    assert(head && "");
    assert(item && "");
    assert(item->priority >= 0 && "");

    assert(item->priority < PRIORITY_LEVELS && "");
    item->list_item.prev = item->list_item.next = 0;
    item = (PriorityListItem*) List_insert(&(head->priority_level_list[item->priority]), (head->priority_level_list[item->priority]).last, (ListItem*) item);
    assert(item && "Problemi di inserimento");
    head->size++;
    return item;
}

PriorityListItem* PriorityList_detach(PriorityListHead* head){
    int max_priority_list_not_empty = -1;
    for(int i=0; i<PRIORITY_LEVELS && max_priority_list_not_empty<0; i++){
        if(head->priority_level_list[i].size>0 && max_priority_list_not_empty < 0)
            max_priority_list_not_empty = i;
    }
    if(max_priority_list_not_empty<0) return NULL;
    PriorityListItem* item = (PriorityListItem*) List_popFront(&(head->priority_level_list[max_priority_list_not_empty]));
    assert(item && "");
    head->size--;
    return item;
}
