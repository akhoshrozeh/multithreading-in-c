// NAME: Anthony Khoshrozeh
// EMAIL: akhoshrozeh@ucla.edu
// ID: 305366728

#include "SortedList.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>


void SortedList_insert(SortedList_t *list, SortedListElement_t *element) {
	// invalid input
	if(list == NULL || element == NULL)
		return;

	SortedListElement_t *curr = list;

	if (opt_yield & INSERT_YIELD){
        sched_yield();
    }

    while(curr != list){
        if (strcmp(element->key, curr->key) <= 0)
            break;

        curr = curr->next;
    }

    element->prev = curr->prev;
    element->next = curr;
    curr->prev->next = element;
    curr->prev = element;
}


int SortedList_delete( SortedListElement_t *element) {

	if (element->prev->next != element || element->next->prev != element || !element->key)
		return 1;

	if (opt_yield & DELETE_YIELD)
		sched_yield();
	
	element->prev->next = element->next;
	element->next->prev = element->prev;

	return 0;
}


SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key) {
	if(!list || !key)
		return NULL;

	SortedListElement_t * temp = list->next;

	if(opt_yield & LOOKUP_YIELD) {
        	sched_yield();
	    }

    while(temp != list) {
   
        if(!strcmp(temp->key, key)) {
            return temp;
        }

        temp = temp->next;
    }

    return NULL;
}


int SortedList_length(SortedList_t *list) {

	SortedListElement_t* temp = list->next;

	if (temp->prev != list)
		return -1;

	int length = 0;
	
	if (opt_yield & LOOKUP_YIELD)
		sched_yield();
	
	while (temp != list)
	{
		if (temp->next->prev != temp)
			return -1;

		length++;
		temp = temp->next;
	}

	return length;
}