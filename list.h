//
// Created by fredde on 2017-05-12.
//

#ifndef UDPSERVER_LIST_H
#define UDPSERVER_LIST_H


#include "Generic.h"

//typedef int Data;

/* 1. Struct-definitioner ********************************/
/*                                                       */
/* Lägg era struct-definitioner nedan                    */
/* Ni kan göra en enkellänkad eller dubbellänkad         */
/* lista. Ni får också välja hur ni vill representera    */
/* listan (nodpekare eller strukt).                      */
/*********************************************************/
typedef struct node
{
    Package data;
    struct node *Next;
} Node;

/* 2. Typdefinition **************************************/
/* Ersätt ordet 'int' nedan med den typ ni valt          */
/* T.ex:											     */
/* typedef struct node* List;                            */
/* eller											     */
/* typedef struct list List;                             */
/* *******************************************************/


typedef struct list
{
    Node *head;
}List;

typedef struct iter
{
    Node *current;
    Node *previous;
}Iter;

/* 3. Interface ****************************************************/
/* Här lägger ni era funktionsdeklarationer för er länkade lista   */
/* Läs labbinstruktioner för vilka som ska vara med                */
/*																   */
/* OBS:															   */
/* ALLA funktioner i interfacet måste returnera eller ta ett       */
/* argument av typen List eller List*                              */
/* *****************************************************************/
List *listCreate(void); // does not compile?
void removeList(List* myList);
void removeFirst(List* myList);
Node *createNode(const Package pack);
void addNodeLast(List *list, Package dataInput);
void addNodeFirst(List *list, Package dataInput);
int numberOfNodes(List *list);
int IsListEmpty(List *list);
void printList(List *mylist);
Node *ackBySEQRecursive(Node *current, uint64_t *lowestSEQ, int sock, struct sockaddr_in clientInfo, List* database);


void printPackage(Package pack);

#endif //UDPCLIENT_LIST_H
