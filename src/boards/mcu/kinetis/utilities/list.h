/**
 * \file list.h
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
#ifndef __LIST_H_
#define __LIST_H_

/*******************************************************************************
 * TYPE DEFINITIONS
 ******************************************************************************/
/* forward_list container element */
typedef struct ForwardListNode_s {
    void *data;
    struct ForwardListNode_s *next;
} ForwardListNode_t;

/* list container element */
typedef struct ListNode_s {
    struct ListNode_s *prev;
    void *data;
    struct ListNode_s *next;
} ListNode_t;

/*******************************************************************************
 * SINGLY LINKED LIST FUNCTION PROTOTYPES (PUBLIC)
 ******************************************************************************/
/*!
 * Creates a new forward_list element with specified data and returns a pointer to
 * the newly created element.
 *
 * \param [IN] data Value to be copied to the inserted elements.
 * \retval Pointer to the newly created forward_list node.
 */
ForwardListNode_t *forward_list_create( void* data );

/*!
 * Inserts a new element at the beginning of the forward_list, right before its current first element.
 *
 * \param [IN] list forward_list pointer.
 * \param [IN] data Value to be copied to the inserted elements.
 */
ForwardListNode_t *forward_list_push_front( ForwardListNode_t *list, void *data );

/*!
 * Removes the first element in the forward_list container, effectively reducing its size by one.
 *
 * \param [IN] list Forward list pointer.
 * \retval Pointer to the newly first element of the forward_list containers.
 */
ForwardListNode_t *forward_list_pop_front( ForwardListNode_t *list );

/*!
 * The container is extended by inserting new elements after the element at position.
 *
 * \param [IN] list forward_list pointer.
 * \param [IN] position Position after which the element will be removed.
 * \param [IN] data Value to be copied to the inserted elements.
 */
ForwardListNode_t *forward_list_insert_after( ForwardListNode_t *list, uint32_t position,
        void *data );

/*!
 * Removes from the forward_list container a single element (the one after position).
 *
 * \param [IN] list forward_list pointer.
 * \param [IN] position Position after which the element will be removed.
 */
void forward_list_erase_after( ForwardListNode_t *list, uint32_t position );

/*!
 * Removes from the forward_list container the specified node.
 *
 * \param [IN] list forward_list pointer.
 * \param [IN] node forward_list node to be removed.
 *
 * \retval      ERR_OK      : Element successfully removed.
 *              ERR_FAILED  : Specified node could not be found/removed.
 */
uint8_t forward_list_remove( ForwardListNode_t *list, ForwardListNode_t *node );

/*!
 * Apply the specified function to each element of the forward_list containers.
 *
 * \param [IN] list forward_list pointer.
 * \param [IN] func Function to be applied to all forward_list container elements.
 *
 * \retval      ERR_OK      : All elements successfully traversed.
 *              ERR_FAILED  : Some element failed to execute specified function.
 */
uint8_t forward_list_foreach( ForwardListNode_t *node, int (*func)( void* ) );

/*!
 * Find a node that fulfills a specified criteria (function pointer) and return pointer to
 * the element if found.
 *
 * \param [IN] list forward_list pointer.
 * \param [IN] func Function that specifies the search criteria.
 * \param [IN] data Value to be found.
 */
ForwardListNode_t *forward_list_find( ForwardListNode_t *node, int (*func)( void*, void* ),
        void *data );

/*!
 * Returns the size of the specified forward_list container.
 *
 * \param [IN] list forward_list pointer.
 * \retval Size of the forward_list container.
 */
uint32_t forward_list_size( ForwardListNode_t *list );
/*******************************************************************************
 * DOUBLY LINKED LIST FUNCTION PROTOTYPES (PUBLIC)
 ******************************************************************************/
/*!
 * Creates a new list element with specified data and returns a pointer to
 * the newly created element.
 *
 * \param [IN] data Value to be copied to the inserted elements.
 * \retval Pointer to the newly created list node.
 */
ListNode_t *list_create( void* data );

/*!
 * Inserts a new element at the beginning of the list, right before its current first element.
 *
 * \param [IN] list List pointer.
 * \param [IN] data Value to be copied to the inserted elements.
 */
ListNode_t *list_push_front( ListNode_t *list, void *data );

/*!
 * Removes the first element in the list container, effectively reducing its size by one.
 *
 * \param [IN] list List pointer.
 * \retval Pointer to the newly first element of the list containers.
 */
ListNode_t *list_pop_front( ListNode_t *list );

/*!
 * Inserts a new element at the end of the list, right after its current last element.
 *
 * \param [IN] list List pointer.
 * \param [IN] data Value to be copied to the inserted elements.
 */
ListNode_t *list_push_back( ListNode_t *list, void *data );

/*!
 * Removes the last element in the list container, effectively reducing its size by one.
 *
 * \param [IN] list List pointer.
 * \retval Pointer to the newly last element of the list containers.
 */
ListNode_t *list_pop_back( ListNode_t *list );

/*!
 * The container is extended by inserting new elements at the specified position.
 *
 * \param [IN] list forward_list pointer.
 * \param [IN] position Position after which the element will be removed.
 * \param [IN] data Value to be copied to the inserted elements.
 */
ListNode_t *list_insert( ListNode_t *list, uint32_t position, void *data );

/*!
 * Removes from the list container a single element at the specified position.
 *
 * \param [IN] list Forward list pointer.
 * \param [IN] position Position from which the element will be removed.
 */
void list_erase( ListNode_t *list, uint32_t position );

/*!
 * Removes from the forward_list container the specified node.
 *
 * \param [IN] list Forward list pointer.
 * \param [IN] node Forward list node to be removed.
 */
uint8_t list_remove( ListNode_t *list, ListNode_t *node );

/*!
 * Apply the specified function to each element of the list containers.
 *
 * \param [IN] list list pointer.
 * \param [IN] func Function to be applied to all list container elements.
 *
 * \retval      ERR_OK      : All elements successfully traversed.
 *              ERR_FAILED  : Some element failed to execute specified function.
 */
uint8_t list_foreach( ListNode_t *node, int (*func)( void* ) );

/*!
 * Find a node that fulfills a specified criteria (function pointer) and return pointer to
 * the element if found.
 *
 * \param [IN] list List pointer.
 * \param [IN] func Function that specifies the search criteria.
 * \param [IN] data Value to be found.
 */
ListNode_t *list_find( ListNode_t *node, int (*func)( void*, void* ), void *data );

/*!
 * Returns the size of the specified list container.
 *
 * \param [IN] list list pointer.
 * \retval Size of the list container.
 */
uint32_t list_size( ListNode_t *list );
/*******************************************************************************
 * END OF CODE
 ******************************************************************************/

#endif /* __LIST_H_ */
