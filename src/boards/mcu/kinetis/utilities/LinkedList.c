/**
 * \file LinkedList.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 15.11.2015
 * \brief Doubly linked list implementation
 *
 */
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "LinkedList.h"

/*!
 * \brief Create new list node element.
 *
 * \param data Data pointer to be added to new element.
 * \return ListNodePointer_t Pointer to created list node element.
 */
ListNodePointer_t CreateListNode(void* data)
{
    ListNodePointer_t newNode = (ListNodePointer_t) malloc(sizeof(ListNode_t));
    newNode->data = data;
    newNode->prev = NULL;
    newNode->next = NULL;

    return newNode;
}

/*!
 * \brief Delete list node element.
 *
 * \param node Pointer to the node to be deleted.
 * \return void* Pointer to data of the deleted element.
 */
void* DeleteListNode(ListNodePointer_t node)
{
    void* tempData = node->data;
    free(node);
    return tempData;
}

ListPointer_t ListCreate(void)
{
    ListPointer_t newList = (ListPointer_t) malloc(sizeof(List_t));
    newList->count = 0;
    newList->head = NULL;
    newList->tail = NULL;

    return newList;
}

ListPointer_t ListCreateAndPopulate(void* dataArray, uint32_t size)
{
    ListPointer_t newList = ListCreate();

    for (uint32_t i = 0; i < size; i++) {
        ListPushFront(newList, dataArray + i);
    }

    return newList;
}

void ListDelete(ListPointer_t list)
{
    /* Delete all remaining elements */
    ListClear(list);
    /* De-allocated list */
    free(list);
}

ListNodePointer_t ListPushFront(ListPointer_t list, void* data)
{
    ListNodePointer_t tempNode = CreateListNode(data);

    if (list->count == 0) {
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

void* ListPopFront(ListPointer_t list)
{
    void* tempData;

    if (list->count == 0) {
        /* List is empty */
        return NULL;
    } else if (list->count == 1) {
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

ListNodePointer_t ListPushBack(ListPointer_t list, void* data)
{
    ListNodePointer_t tempNode = CreateListNode(data);

    if (list->count == 0) {
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

void* ListPopBack(ListPointer_t list)
{
    void* tempData;

    if (list->count == 0) {
        /* List is empty */
        return NULL;
    } else if (list->count == 1) {
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

ListNodePointer_t ListInsert(ListPointer_t list, void* data, uint32_t position)
{
    if (position > list->count) {
        return NULL;
    }

    if (position == 0) {
        /* Add to head */
        ListPushFront(list, data);
    }

    if (position == (list->count - 1)) {
        /* Add to tail */
        ListPushBack(list, data);
    }

    uint32_t cnt;
    ListNodePointer_t tempNode;
    ListNodePointer_t newNode = CreateListNode(data);

    if (position < (list->count / 2)) {
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
    newNode->next = tempNode;
    newNode->prev = tempNode->prev;
    tempNode->prev = newNode;
    list->count++;

    return newNode;
}

void ListRemove(ListPointer_t list, void* data)
{
    if (list->count == 0) return;

    ListNodePointer_t tempNode = ListFind(list, data);

    if (tempNode == NULL) return;

    if (tempNode == list->tail) {
        /* Check if found node is tail */
        ListPopBack(list);
    } else if (tempNode == list->head) {
        /* Check if found node is head */
        ListPopFront(list);
    } else {
        tempNode->next->prev = tempNode->prev;
        tempNode->prev->next = tempNode->next;
        DeleteListNode(tempNode);
    }
    list->count--;
}

ListNodePointer_t ListFind(ListPointer_t list, void* data)
{
    ListNodePointer_t tempNode = list->head;
    uint32_t cnt = 0;

    while (cnt < list->count) {
        if (tempNode->data == data) return tempNode;
        if (tempNode->next == NULL) break;
        tempNode = tempNode->next;
        cnt++;
    }
    return NULL;
}

void ListClear(ListPointer_t list)
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
