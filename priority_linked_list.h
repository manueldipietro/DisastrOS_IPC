#pragma once
#include "linked_list.h"

#define PRIORITY_LEVELS 5

typedef struct PriorityListHead{
    ListHead priority_level_list[PRIORITY_LEVELS];
    int size;
} PriorityListHead;

typedef struct PriorityListItem{
    ListItem list_item;
    int priority;
} PriorityListItem;

void PriorityList_init(PriorityListHead* head);
PriorityListItem* PriorityList_insert(PriorityListHead* head, PriorityListItem* item);
PriorityListItem* PriorityList_detach(PriorityListHead* head);

