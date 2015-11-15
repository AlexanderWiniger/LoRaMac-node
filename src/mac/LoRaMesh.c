/**
 * \file LoRaMesh.c
 * \author Alexander Winiger (alexander.winiger@hslu.ch)
 * \date 11.11.2015
 * \brief Mesh LoRa network implementation
 *
 */

#include "board.h"
#include "LoRaMesh.h"

#define LOG_LEVEL_TRACE
#include "debug.h"

/*!
 * Child node list.
 */
static ListPointer_t ChildNodeList;

/*!
 * Address of the parent node.
 */
static uint32_t ParentAddr;

/*!
 * Address of the coordinator node.
 */
static uint32_t CoodrinatorAddr;

/*!
 * Advertising slot information.
 */
static LoRaMeshSlotInfo_t AdvertisingSlot;

/*!
 * \brief Create new child node with given data
 *
 * \param devAddr Device address of the child node
 * \param nwkSKey Network session key
 * \param appSKey Application session key
 * \param uplinkSlot Uplink slot information
 */
LoRaMeshChildNodeInfo_t* CreateChildNode(uint32_t devAddr, uint8_t* nwkSKey, uint8_t* appSKey,
        LoRaMeshSlotInfo_t* uplinkSlot);

/*!
 * \brief Print out child node information.
 *
 * \param childNode Child node to be printed out.
 */
void PrintChildNode(LoRaMeshChildNodeInfo_t* childNode);

void LoRaMeshInit(LoRaMeshCallbacks_t *callabcks)
{
    LoRaMeshChildNodeInfo_t* newNode;
    LoRaMeshSlotInfo_t* UplinkSlot;
    uint32_t DevAddr, remAddr;
    uint8_t LoRaMeshNwkSKey[] = { 0x76, 0x73, 0xb7, 0x0a, 0x7a, 0x70, 0x7c, 0x83, 0x43, 0x9c, 0xc8,
            0x6e, 0xdf, 0xf2, 0x3f, 0x9a };
    uint8_t LoRaMeshAppSKey[] = { 0x7b, 0x95, 0xf3, 0x0a, 0x35, 0x0a, 0x42, 0x1e, 0x17, 0x75, 0x02,
            0x2d, 0xf2, 0xb7, 0x17, 0xb2 };

    /* Create empty linked list */
    ChildNodeList = ListCreate();

    UplinkSlot = (LoRaMeshSlotInfo_t*) malloc(sizeof(LoRaMeshSlotInfo_t));
    UplinkSlot->Frequency = 868100000;
    UplinkSlot->Periodicity = 5;
    UplinkSlot->Duration = 50;

    for (uint8_t i = 0; i < 10; i++) {
        DevAddr = randr(0, 0x01FFFFFF);
        if (i == 5) remAddr = DevAddr;
        newNode = CreateChildNode(DevAddr, (uint8_t*) &LoRaMeshNwkSKey, (uint8_t*) &LoRaMeshAppSKey,
                UplinkSlot);
        AddChildNode(newNode);
    }
    /* Test print list */
    PrintChildNodeList(false);

    /* Test remove node */
    RemoveChildNode(remAddr);

    /* Test print list in reverse order */
    PrintChildNodeList(true);
}

LoRaMeshChildNodeInfo_t* CreateChildNode(uint32_t devAddr, uint8_t* nwkSKey, uint8_t* appSKey,
        LoRaMeshSlotInfo_t* uplinkSlot)
{
    LoRaMeshChildNodeInfo_t* newNode = (LoRaMeshChildNodeInfo_t*) malloc(
            sizeof(LoRaMeshChildNodeInfo_t));
    newNode->Address = devAddr;
    memcpy(newNode->NwkSKey, nwkSKey, 16);
    memcpy(newNode->AppSKey, appSKey, 16);
    newNode->UpLinkCounter = 0;
    memcpy(&(newNode->UplinkSlot), uplinkSlot, sizeof(LoRaMeshSlotInfo_t));

    return newNode;
}

LoRaMeshChildNodeInfo_t* RemoveChildNode(uint32_t devAddr)
{
    LoRaMeshChildNodeInfo_t* tempNode = FindChildNode(devAddr);

    if (tempNode != NULL) {
        ListRemove(ChildNodeList, (void*) tempNode);
    }

    return tempNode;
}

void AddChildNode(LoRaMeshChildNodeInfo_t* childNode)
{
    ListPushBack(ChildNodeList, (void*) childNode);
}

LoRaMeshChildNodeInfo_t* FindChildNode(uint32_t devAddr)
{
    LoRaMeshChildNodeInfo_t* tempNode;
    ListNodePointer_t listNode = ChildNodeList->head;

    while (listNode->next != NULL) {
        tempNode = (LoRaMeshChildNodeInfo_t*) listNode->data;
        if (tempNode->Address == devAddr) return tempNode;
        listNode = listNode->next;
    }

    return NULL;
}

void PrintChildNodeList(bool reverseOrder)
{
    ListNodePointer_t tempNode;
    uint8_t i;
    if (reverseOrder)
        tempNode = (ListNodePointer_t) ChildNodeList->tail;
    else
        tempNode = (ListNodePointer_t) ChildNodeList->head;

    for (i = 0; i < ChildNodeList->count; i++) {
        PRINTF("%u. ---------------------------------------------\r\n", (i + 1));
        PrintChildNode((LoRaMeshChildNodeInfo_t*) tempNode->data);
        if (reverseOrder)
            tempNode = (ListNodePointer_t) tempNode->prev;
        else
            tempNode = (ListNodePointer_t) tempNode->next;
    }
}

void PrintChildNode(LoRaMeshChildNodeInfo_t* childNode)
{
    uint8_t j;
    PRINTF("%-15s: 0x%08x\r\n", "Address", childNode->Address);
    PRINTF("%-15s: ", "NwkSKey");
    for (j = 0; j < 16; j++)
        PRINTF("0x%02x ", childNode->NwkSKey[j]);
    PRINTF("\r\n%-15s: ", "AppSKey");
    for (j = 0; j < 16; j++)
        PRINTF("0x%02x ", childNode->AppSKey[j]);
    PRINTF("%-15s: 0x%08x\r\n", "UpLinkCounter", childNode->UpLinkCounter);
    PRINTF("\r\n---- %-15s ----\r\n", "Uplink Slot Info");
    PRINTF("%-15s: %u\r\n", "Frequency", childNode->UplinkSlot.Frequency);
    PRINTF("%-15s: %u\r\n", "Periodicity", childNode->UplinkSlot.Periodicity);
    PRINTF("%-15s: %u\r\n", "Duration", childNode->UplinkSlot.Duration);
}
