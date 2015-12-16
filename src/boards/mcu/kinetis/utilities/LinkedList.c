/**
 * \file LinkedList.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 15.11.2015
 * \brief Doubly linked list implementation
 *
 */
/*******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "LinkedList.h"
#include "utilities.h"

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES (STATIC)
 ******************************************************************************/
/*! \brief Create new list node element. */
ListNodePointer_t CreateListNode( void* data );

/*! \brief Find list node element. */
ListNodePointer_t FindListNode( ListPointer_t list, void* data );

/*! \brief Delete list node element. */
void* DeleteListNode( ListNodePointer_t node );

/*******************************************************************************
 * MODULE FUNCTIONS (PUBLIC)
 ******************************************************************************/
ListPointer_t ListCreate( void )
{
    ListPointer_t newList = (ListPointer_t) custom_malloc(sizeof(LinkedList_t));
    newList->count = 0;
    newList->head = NULL;
    newList->tail = NULL;

    return newList;
}

ListPointer_t ListCreateAndPopulate( void* dataArray, uint32_t size )
{
    ListPointer_t newList = ListCreate();

    for ( uint32_t i = 0; i < size; i++ ) {
        ListPushFront(newList, dataArray + i);
    }

    return newList;
}

void ListDelete( ListPointer_t list )
{
    /* Delete all remaining elements */
    ListClear(list);
    /* De-allocated list */
    custom_free(list);
}

ListNodePointer_t ListPushFront( ListPointer_t list, void* data )
{
    ListNodePointer_t tempNode = CreateListNode(data);

    if ( tempNode == NULL ) return NULL;

    if ( list->count == 0 ) {
        /* List is empty */
        list->head = tempNode;
        list->tail = tempNode;
    } else {
        list->head->prev = tempNode;
        tempNode->next = list->head;
        list->head = tempNode;
    }
    list->count++;

    return tempNode;
}

void* ListPopFront( ListPointer_t list )
{
    void* tempData;

    if ( list->count == 0 ) {
        /* List is empty */
        return NULL;
    } else if ( list->count == 1 ) {
        /* Last element */
        tempData = DeleteListNode(list->head);
        list->head = NULL;
        list->tail = NULL;
    } else {
        ListNodePointer_t tempNode = list->head->next;
        tempNode->prev = NULL;
        tempData = DeleteListNode(list->head);
        list->head = tempNode;
    }
    list->count--;

    return tempData;
}

ListNodePointer_t ListPushBack( ListPointer_t list, void* data )
{
    ListNodePointer_t tempNode = CreateListNode(data);

    if ( list->count == 0 ) {
        /* List is empty */
        list->head = tempNode;
        list->tail = tempNode;
    } else {
        list->tail->next = tempNode;
        tempNode->prev = list->tail;
        list->tail = tempNode;
    }
    list->count++;

    return tempNode;
}

void* ListPopBack( ListPointer_t list )
{
    void* tempData;

    if ( list->count == 0 ) {
        /* List is empty */
        return NULL;
    } else if ( list->count == 1 ) {
        /* Last element */
        tempData = DeleteListNode(list->tail);
        list->head = NULL;
        list->tail = NULL;
    } else {
        ListNodePointer_t tempNode = list->tail->prev;
        tempNode->next = NULL;
        tempData = DeleteListNode(list->tail);
        list->tail = tempNode;
    }
    list->count--;

    return tempData;
}

ListNodePointer_t ListInsert( ListPointer_t list, void* data, uint32_t position )
{
    ListNodePointer_t tempNode, newNode;
    uint32_t cnt;

    if ( position > list->count ) {
        return NULL;
    }

    if ( position == 0 ) {
        /* Add to head */
        return ListPushFront(list, data);
    }

    if ( position == (list->count) ) {
        /* Add to tail */
        return ListPushBack(list, data);
    }

    newNode = CreateListNode(data);

    if ( position < (list->count / 2) ) {
        /* Start at the head */
        cnt = 0;
        tempNode = list->head;

        while (cnt < position) {
            tempNode = tempNode->next;
            cnt++;
        }

        newNode->next = tempNode;
        newNode->prev = tempNode->prev;
        tempNode->prev->next = newNode;
        tempNode->prev = newNode;
    } else {
        /* Start at the tail */
        cnt = position;
        tempNode = list->tail;

        while (cnt > 0) {
            tempNode = tempNode->prev;
            cnt--;
        }

        newNode->prev = tempNode;
        newNode->next = tempNode->next;
        tempNode->next->prev = newNode;
        tempNode->next = newNode;
    }

    list->count++;

    return newNode;
}

void ListRemove( ListPointer_t list, void* data )
{
    if ( list->count == 0 ) return;

    ListNodePointer_t tempNode = FindListNode(list, data);

    if ( tempNode == NULL ) return;

    if ( tempNode == list->tail ) {
        /* Check if found node is tail */
        (void*) ListPopBack(list);
    } else if ( tempNode == list->head ) {
        /* Check if found node is head */
        (void*) ListPopFront(list);
    } else {
        tempNode->next->prev = tempNode->prev;
        tempNode->prev->next = tempNode->next;
        DeleteListNode(tempNode);
    }
    list->count--;
}

void ListRemoveAt( ListPointer_t list, uint32_t position )
{
    ListNodePointer_t tempNode;
    uint32_t cnt;

    if ( position > list->count ) {
        return;
    }

    if ( position == 0 ) {
        /* Add to head */
        (void*) ListPopFront(list);
    }

    if ( position == (list->count) ) {
        /* Add to tail */
        (void*) ListPopBack(list);
    }

    if ( position < (list->count / 2) ) {
        /* Start at the head */
        cnt = 0;
        tempNode = list->head;

        while (cnt < position) {
            tempNode = tempNode->next;
            cnt++;
        }
    } else {
        /* Start at the tail */
        cnt = position;
        tempNode = list->tail;

        while (cnt > 0) {
            tempNode = tempNode->prev;
            cnt--;
        }
    }
    /* Plug hole in list */
    tempNode->prev->next = tempNode->next;
    tempNode->next->prev = tempNode->prev;
    DeleteListNode(tempNode);
    list->count--;
}

void ListClear( ListPointer_t list )
{
    ListNodePointer_t curNode, nextNode;
    curNode = list->head;

    while (curNode->next != NULL) {
        nextNode = curNode->next;
        DeleteListNode(curNode);
        curNode = nextNode;
    }

    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}
/*******************************************************************************
 * PRIVATE FUNCTIONS (STATIC)
 ******************************************************************************/
/*!
 * Create new list node element.
 *
 * \param data Data pointer to be added to new element.
 * \return ListNodePointer_t Pointer to created list node element.
 */
ListNodePointer_t CreateListNode( void* data )
{
    ListNodePointer_t newNode = (ListNodePointer_t) custom_malloc(sizeof(ListNode_t));
    if ( newNode != NULL ) {
        newNode->data = data;
        newNode->prev = NULL;
        newNode->next = NULL;
    }
    return newNode;
}

/*!
 * \brief Find element in the list.
 *
 * \param list Pointer to the list the element will be insert.
 * \param data Data pointer that will be added to the element.
 * \return ListNodePointer_t Pointer to the list node element if found, else NULL.
 */
ListNodePointer_t FindListNode( ListPointer_t list, void* data )
{
    ListNodePointer_t tempNode = list->head;

    while (tempNode != NULL) {
        if ( tempNode->data == data ) return tempNode;
        tempNode = tempNode->next;
    }
    return NULL;
}

/*!
 * Delete list node element.
 *
 * \param node Pointer to the node to be deleted.
 * \return void* Pointer to data of the deleted element.
 */
void* DeleteListNode( ListNodePointer_t node )
{
    void* tempData = node->data;
    custom_free(node);
    return tempData;
}
/*******************************************************************************
 * END OF CODE
 ******************************************************************************/
