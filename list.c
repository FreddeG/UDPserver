#include "list.h"
#include "Generic.h"


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

bool findSeq(List *list, uint64_t seqToFind)
{
    if(list->head == NULL) return false;

    Node* curr = list->head;

    while (curr != NULL)
    {
        if(curr->data.seq == seqToFind)
            return true;

        curr = curr->Next;
    }
    return false;
}


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
void printData(List *mylist)
{
    Node *iterator = mylist->head;
    if (numberOfNodes(mylist) == 0)
    {
        printf("\nThis list is empty you scrub");
    }
    else
    {
        printf("\nData: ");
        do
        {
            //  printf("%d   ", iterator->data); fix printing
            printf("%c", iterator->data.data);
            iterator = iterator->Next;
        } while (iterator != NULL);
    }


}

void sortBySeq(List *list) // should not be used
{

    // Don't try to sort empty or single-node lists
    if (list->head == NULL || list->head->Next == NULL) return;
    Node *current;
    Node *next;
    Node *previous;

    int num_nodes = numberOfNodes(list);
    int i;

    for (i = 0; i < num_nodes; i++)
    {
        current = list->head;
        next = current->Next;
        previous = NULL;

        while (next != NULL)
        {
            if (current->data.seq > next->data.seq) // if next is smaller than current, wrong order
            {
                if (current == list->head) // we need to change the list->head as it is not enough to move them around, we need to change the head as well
                {
                    list->head = next;
                }
                else
                {
                    previous->Next = next; // here we don't need to change the head, just make sure previous connects instead
                }

                current->Next = next->Next;
                next->Next = current;

                previous = next;
                next = current->Next;
            }
            else // keep in mind, we just switched our previous and next, this the only place where we progress in the list
            {
                previous = current;
                current = current->Next;
                next = current->Next;
            }

        }
    }
}

// use list.head = removebySEQRecursive(list.head, SEQ)
// a) checks if to be removed
// b) writes ack to client
// c) writes package to database
// d)
void jail(List *jailList, Package pack, int sock, struct sockaddr_in serverAddr, bool genError)
{
    uint8_t trigger = rand() % 100;
    if(genError == false)
    {
        trigger = 0;
    }
    if(trigger < ERRORPROBABILITY)
    {
        uint8_t errorType = rand() % 2;

        //destroy checksum
        if (errorType == 0)
        {

            addNodeLast(jailList, pack);
            pack.checkSum = pack.checkSum + 1;
            if (sendto(sock, &pack, sizeof(Package), 0, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) == -1)
            {
                die("sendto()");
            }
            printTime();
            printf("\n!!Destroying checksum!!\n");
            printPackage(pack);

        }
        else if (errorType == 1)
        {
            printTime();
            printf("\n!!GOES TO JAIL!!");
            printPackage(pack);
            addNodeLast(jailList, pack);
        }
        else
        {
            //wont happen :)
        }
    }
    else
    {
        if (sendto(sock, &pack, sizeof(Package), 0, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) == -1)
        {
            die("sendto()");
        }
        printTime();
        printf("\n-=Output package=-");
        printPackage(pack);
    }
}
Node* freeFromJail(List*list)
{
    Node *curr = list->head;
    Node *prev = curr;
    int numNodes = numberOfNodes(list);

    int count = 0;
    //somerandom numbbetween0andlength - 1

    if(numNodes == 0)
    {
        return NULL;
    }
    int targetNode = rand() % numberOfNodes(list); // must be at least 2 elements in list
    if(targetNode == 0)
    {
        list->head = curr->Next;
        return curr;
    }

    while (count < targetNode)  // vi har 2 nodes, targetnode = 1,
    {
        prev = curr;
        curr = curr->Next;
        count++;
    }

    prev->Next = curr->Next;
    return curr;
}



bool ackBySEQ(List *list, uint64_t lowestSEQ, int sock, struct sockaddr_in clientInfo, List* database, uint64_t *outSeq) // this function will pass its original current node back to itself if it is not the ID we are looking for
{
    Node* curr = list->head;
    Node* prev = curr;



    while(curr != NULL)
    {
        if(curr->data.seq == lowestSEQ)
        {
            Package outputBuf;
            emptyPackage(&outputBuf);

            //outputBuf.seq

            outputBuf.ack = lowestSEQ;
            outputBuf.seq = *outSeq;
            *outSeq = *outSeq + 1;
            outputBuf.checkSum = 0;
            outputBuf.checkSum = checksum(outputBuf);

            if (sendto(sock, &outputBuf, sizeof(Package), 0, (struct sockaddr*) &clientInfo,  sizeof(clientInfo)) == -1)
            {
                die("sendto()");
            }
            printTime();
            printf("\n sending ack from ackbyseq! lowestSeq is %lu\n", lowestSEQ);
            printPackage(outputBuf);

            addNodeLast(database, curr->data); // save to file

            if(list->head == curr)
            {
                list->head = curr->Next;
            }
            else
            {
                prev->Next = curr->Next;
            }


            free(curr);
            return true;
        }
        prev = curr;
        curr = curr->Next;


    }
    return false;


}




