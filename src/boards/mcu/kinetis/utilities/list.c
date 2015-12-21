/**
 * \file list.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 21.12.2015
 * \version 1.0
 *
 * \brief Implementation of singly and doubly linked lists.
 *
 *******************************************************************************
 *  Change log:
 *      [1.0]   21.12.2015      Alexander Winiger (alexander.winiger@hslu.ch)
 *          - created
 *******************************************************************************
 */

/*******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/
#include "utilities.h"
#include "list.h"

/*******************************************************************************
 * SINGLY LINKED LIST FUNCTIONS (PUBLIC)
 ******************************************************************************/
ForwardListNode_t *forward_list_create( void* data )
{
    ForwardListNode_t *node;
    if ( !(node = custom_malloc(sizeof(ForwardListNode_t))) ) return NULL;
    node->data = data;
    node->next = NULL;
    return node;
}

ForwardListNode_t *forward_list_push_front( ForwardListNode_t *list, void *data )
{
    ForwardListNode_t *newNode;
    newNode = forward_list_create(data);
    newNode->next = list;
    return newNode;
}

ForwardListNode_t *forward_list_pop_front( ForwardListNode_t *list )
{
    ForwardListNode_t *newHead;
    newHead = list->next;
    /* free memory */
    custom_free(list);
    return newHead;
}

ForwardListNode_t *forward_list_insert_after( ForwardListNode_t *list, uint32_t position,
        void *data )
{
    ForwardListNode_t *newNode;
    newNode = forward_list_create(data);

    while ( position > 0 ) {
        list = list->next;
        position--;
    }
    newNode->next = list->next;
    list->next = newNode;

    return newNode;
}

void forward_list_erase_after( ForwardListNode_t *list, uint32_t position )
{
    while ( position > 0 ) {
        list = list->next;
        position--;
    }
    list->next = list->next->next;
    custom_free(list->next);
}

uint8_t forward_list_remove( ForwardListNode_t *list, ForwardListNode_t *node )
{
    while ( list->next && list->next != node )
        list = list->next;
    if ( list->next ) {
        list->next = node->next;
        custom_free(node);
        return ERR_OK;
    } else
        return ERR_FAILED;
}

uint8_t forward_list_foreach( ForwardListNode_t *node, int (*func)( void* ) )
{
    while ( node ) {
        if ( func(node->data) != ERR_OK ) return ERR_FAILED;
        node = node->next;
    }
    return ERR_OK;
}

ForwardListNode_t *forward_list_find( ForwardListNode_t *node, int (*func)( void*, void* ),
        void *data )
{
    while ( node ) {
        if ( func(node->data, data) > 0 ) return node;
        node = node->next;
    }
    return NULL;
}

uint32_t forward_list_size( ForwardListNode_t *list )
{
    uint32_t size = 0;

    while ( list != NULL ) {
        list = list->next;
        size++;
    }
    return size;
}

/*******************************************************************************
 * DOUBLY LINKED LIST FUNCTIONS (PUBLIC)
 ******************************************************************************/
ListNode_t *list_create( void* data )
{
    ListNode_t *node;
    if ( !(node = custom_malloc(sizeof(ListNode_t))) ) return NULL;
    node->prev = NULL;
    node->data = data;
    node->next = NULL;
    return node;
}

ListNode_t *list_push_front( ListNode_t *list, void *data )
{
    ListNode_t *newNode;
    newNode = list_create(data);
    newNode->next = list;
    list->prev = newNode;
    return newNode;
}

ListNode_t *list_pop_front( ListNode_t *list )
{
    ListNode_t *newHead;
    newHead = list->next;
    newHead->prev = NULL;
    custom_free(list);
    return newHead;
}

ListNode_t *list_push_back( ListNode_t *list, void *data )
{
    ListNode_t *newNode;
    newNode = list_create(data);

    while ( list->next != NULL )
        list = list->next;

    list->next = newNode;
    newNode->prev = list;

    return newNode;
}

ListNode_t * list_pop_back( ListNode_t *list )
{
    ListNode_t *newTail;

    while ( list->next != NULL )
        list = list->next;

    newTail = list->prev;
    newTail->next = NULL;
    custom_free(list);

    return newTail;
}

ListNode_t *list_insert( ListNode_t *list, uint32_t position, void *data )
{
    ListNode_t *newNode;
    newNode = list_create(data);

    while ( position > 0 ) {
        list = list->next;
        position--;
    }

    newNode->next = list;
    newNode->prev = list->prev;
    list->prev = newNode;

    return newNode;
}

void list_erase( ListNode_t *list, uint32_t position )
{
    while ( position > 0 ) {
        list = list->next;
        position--;
    }

    list->prev->next = list->next;
    list->next->prev = list->prev;
    custom_free(list);
}

uint8_t list_remove( ListNode_t *list, ListNode_t *node )
{
    while ( list != node )
        list = list->next;
    if ( list ) {
        list->prev->next = list->next;
        list->next->prev = list->prev;
        custom_free(node);
        return ERR_OK;
    } else
        return ERR_FAILED;
}

uint8_t list_foreach( ListNode_t *node, int (*func)( void* ) )
{
    while ( node ) {
        if ( func(node->data) != ERR_OK ) return ERR_FAILED;
        node = node->next;
    }
    return ERR_OK;
}

ListNode_t *list_find( ListNode_t *node, int (*func)( void*, void* ), void *data )
{
    while ( node ) {
        if ( func(node->data, data) > 0 ) return node;
        node = node->next;
    }
    return NULL;
}

uint32_t list_size( ListNode_t *list )
{
    uint32_t size = 0;

    while ( list != NULL ) {
        list = list->next;
        size++;
    }
    return size;
}
/*******************************************************************************
 * END OF CODE
 ******************************************************************************/
