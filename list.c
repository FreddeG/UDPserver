#include "list.h"


/* Skriv era funktionsimplementationer för interfacet till er länkade lista här 
   Det går också bra att skriva statiska hjälpfunktioner i denna fil            */




/* Skriv era funktionsimplementationer för interfacet till er länkade lista här
Det går också bra att skriva statiska hjälpfunktioner i denna fil            */

List *createList()
{
    List *newList = (List*)malloc(sizeof(List));

    if (newList == NULL)
    {
        printf("\nYou done goofed");
        return NULL;
    }
    newList->head = NULL;
    printf("List created!");
    return newList;
}

void removeList(List* myList)
{
    Node *current = myList->head;
    Node *previous = current;
    while (current != NULL)
    {
        previous = current;
        current = current->Next;
        free(previous);
    }
}

void removeFirst(List* myList)
{
    Node *current = myList->head;
    Node *previous = current;
    if (current != NULL)
    {
        current = current->Next;
        free(previous);
        previous = NULL;
        myList->head = current;
    }
}

Node *createNode(const Package pack)
{
    Node *newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL)
    {
        printf("Memory for new node could not be allocated");
        return NULL;
    }
    newNode->data = pack; // create empty pack?

    newNode->Next = NULL;
    return newNode;
}

void addNodeLast(List *list, Package dataInput)
{
    if (list->head == NULL)
    {
        list->head = createNode(dataInput);
        return;
    }
    Node *current = list->head;
    Node *previous = current;

    while (current != NULL)
    {
        previous = current;
        current = current->Next;
    }

    previous->Next = createNode(dataInput);

}

void addNodeFirst(List *list, Package dataInput) // will work on head being NULL as it will just overwrite the nodes->Next NULL with NULL
{
    Node *temp = list->head;
    Node *iter;

    list->head = createNode(dataInput);
    iter = list->head;
    iter->Next = temp;
}

int numberOfNodes(List *list)
{
    int x = 0;
    Node *iter = list->head;

    while (iter != NULL)
    {
        x++;
        iter = iter->Next;
    }
    return x;
}
/* not fixed
int dataSearch(List *list, Package x)
{
    Node *current = list->head;
    int flag = 0;

    while (current != NULL)
    {
        if (current->data == x) flag = 1;
        current = current->Next;
    }

    return flag;
}
*/

int IsListEmpty(List *list)
{

    if (numberOfNodes(list) == 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void printList(List *mylist)
{
    Node *iterator = mylist->head;
    if (numberOfNodes(mylist) == 0)
    {
        printf("\nThis list is empty you scrub");
    }
    else
    {
        printf("\nThelist: ");
        do
        {
            //  printf("%d   ", iterator->data); fix printing
            printPackage(iterator->data);
            iterator = iterator->Next;
        } while (iterator != NULL);
    }


}

void swap(Node )

void sortListBySeq(List *mylist)
{
    void bubbleSort(struct node *start)
    {
        int swapped, i;
        struct node *ptr1;
        struct node *lptr = NULL;

        /* Checking for empty list */
        if (ptr1 == NULL)
            return;

        do
        {
            swapped = 0;
            ptr1 = start;

            while (ptr1->next != lptr)
            {
                if (ptr1->data > ptr1->next->data)
                {
                    swap(ptr1, ptr1->next);
                    swapped = 1;
                }
                ptr1 = ptr1->next;
            }
            lptr = ptr1;
        }
        while (swapped);
    }

}