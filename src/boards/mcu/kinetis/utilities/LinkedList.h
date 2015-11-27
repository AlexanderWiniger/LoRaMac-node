/**
 * \file LinkedList.h
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 15.11.2015
 * \brief Doubly linked list implementation
 *
 */

#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

/*!
 * List node element definition.
 */
typedef struct ListNode_s {
    struct ListNode_s* prev;
    void* data;
    struct ListNode_s* next;
} ListNode_t;

/*!
 * List node pointer type.
 */
typedef ListNode_t* ListNodePointer_t;

/*!
 * List struct definition.
 */
typedef struct List_s {
    uint32_t count;
    ListNodePointer_t head;
    ListNodePointer_t tail;
} LinkedList_t;

/*!
 * List pointer type.
 */
typedef LinkedList_t* ListPointer_t;

/*!
 * \brief Create empty list.
 *
 * \return ListPointer_t Pointer to created list.
 */
ListPointer_t ListCreate( void );

/*!
 * \brief Create list with passed elements.
 *
 * \param dataArray Pointer to array with data to be added to new list.
 * \param size Number of elements contained in dataArray.
 * \return ListPointer_t Pointer to the created list.
 */
ListPointer_t ListCreateAndPopulate( void* dataArray, uint32_t size );

/*!
 * \brief Delete list.
 *
 * \param list Pointer to the list to be deleted.
 */
void ListDelete( ListPointer_t list );

/*!
 * \brief Insert element at beginning.
 *
 * \param list Pointer to the list the element will be insert.
 * \param data Data pointer that will be added to the element.
 * \return ListNodePointer_t Pointer to the added list node.
 */
ListNodePointer_t ListPushFront( ListPointer_t list, void* data );

/*!
 * \brief Delete first element.
 *
 * \param list Pointer to the list the element will be insert.
 * \return void* Pointer to the data from the deleted list node.
 */
void* ListPopFront( ListPointer_t list );

/*!
 * \brief Add element at the end.
 *
 * \param list Pointer to the list the element will be insert.
 * \param data Data pointer that will be added to the element.
 * \return ListNodePointer_t Pointer to the added list node.
 */
ListNodePointer_t ListPushBack( ListPointer_t list, void* data );

/*!
 * \brief Delete last element.
 *
 * \param list Pointer to the list the element will be insert.
 * \return void* Pointer to the data from the deleted list node.
 */
void* ListPopBack( ListPointer_t list );

/*!
 * \brief Insert element at position.
 *
 * \param list Pointer to the list the element will be insert.
 * \param data Data pointer that will be added to the element.
 * \param position Position of the element to be added.
 */
ListNodePointer_t ListInsert( ListPointer_t list, void* data, uint32_t position );

/*!
 * \brief Remove element with specific value.
 *
 * \param list Pointer to the list the element will be insert.
 */
void ListRemove( ListPointer_t list, void* data );

/*!
 * \brief Find element in the list.
 *
 * \param list Pointer to the list the element will be insert.
 * \param data Data pointer that will be added to the element.
 * \return ListNodePointer_t Pointer to the list node element if found, else NULL.
 */
ListNodePointer_t ListFind( ListPointer_t list, void* data );

/*!
 * \brief Clear list.
 *
 * \param list Pointer to the list the element will be insert.
 */
void ListClear( ListPointer_t list );

#endif /* __LINKEDLIST_H__ */
