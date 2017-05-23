/*
    Simple udp server
*/
/*
#include<stdio.h> //printf
#include<stdint.h>
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/select.h>
*/

#include "list.h"
#include "Generic.h"


#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data

#define MAXDATA 200
#define INITCONNECT 0
#define WAITINITCONNECT 1
#define WAITINGCONFIRMCONNECT 2
#define CONNECTED 3
#define INITCLOSE 4
#define WAITINGCLOSE 5
#define CLOSED 6

#define WINDOWSIZE 10



int main(void)
{
    struct sockaddr_in serverInfo, clientInfo, AcceptedHost;
    Package outputBuf, inputBuf;
    fd_set activeFdSet, readFdSet;
    struct timeval timeout_t;
    uint64_t incLowestSeq;
    uint64_t outSeq;
    List list;
    list.head = NULL;

    uint16_t  count = 0;
    Node *ptr = NULL;

    uint16_t  packageType = 0;

    List database;
    database.head = NULL;
    List jailList;
    jailList.head = NULL;

    bool genError = false;


    FILE *fp = NULL;
    uint16_t trigger = 0;

    srand(time(0)); // seed our rand

    int sock, i, slen = sizeof(clientInfo);
    //char buf[BUFLEN];

    //create a UDP socket
    if ((sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) // initialize socket with UPD setting
    {
        die("socket");
    }

    // zero out the structure
    memset((char *) &serverInfo, 0, sizeof(serverInfo));

    serverInfo.sin_family = AF_INET;
    serverInfo.sin_port = htons(PORT);
    serverInfo.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind socket to port
    if( bind(sock , (struct sockaddr*)&serverInfo, sizeof(serverInfo) ) == -1)
    {
        die("bind");
    }

    /* Initialize the set of active sockets */
    FD_ZERO(&activeFdSet); // initializes an empty FDset
    FD_SET(sock, &activeFdSet); // puts socket in the set, activeFDset currently only contains this socket.

    int currentState = INITCONNECT;

    while(1) {
        switch (currentState) {
            case INITCONNECT: // our startup state
                readFdSet = activeFdSet;

                timeout_t.tv_sec = 120;
                timeout_t.tv_usec = 0;

                if (select(FD_SETSIZE, &readFdSet, NULL, NULL, &timeout_t) < 0) { // blocking, waits until one the FD is set to ready, will keep the ready ones in readFdSet
                    perror("Select failed\n");
                    exit(EXIT_FAILURE);
                }
                else if(FD_ISSET(sock, &readFdSet)) // if we have recieved on socket
                {

                    if ((recvfrom(sock, &inputBuf, sizeof(Package), 0, (struct sockaddr *) &clientInfo, &slen)) == -1) {
                        // perror(recvfrom());
                        die("recvfrom()");
                    }
                    printTime();
                    printf("\n Input package, first syn");
                    printPackage(inputBuf);

                    if(viewPackage(inputBuf) == 1) // syn
                    {

                        emptyPackage(&outputBuf);
                        outputBuf.seq = initSEQ() -895034580135; // just for some variation in seq number
                        outSeq = outputBuf.seq + 1;
                        outputBuf.ack = inputBuf.seq;
                        incLowestSeq = inputBuf.seq + 1;
                        outputBuf.winSize = WINDOWSIZE;
                        outputBuf.checkSum = checksum(outputBuf);

                        jail(&jailList, outputBuf, sock, clientInfo, genError); //


                        currentState = WAITINITCONNECT;

                    }
                    else
                    {
                        //
                    }


                    // break
                }
                else
                {
                    // want to quit?
                    break;

                }
                //currentState = WAITINITCONNECT;

                break;
            case WAITINITCONNECT:

                trigger = rand() %100;
                if(trigger > RELEASEPROBABILITY)
                {
                    Node *ptr = freeFromJail(&jailList);
                    if(ptr != NULL)
                    {
                        if (sendto(sock, &ptr->data, sizeof(Package), 0, (struct sockaddr *) &clientInfo, slen) == -1) {
                            perror("sendto()");
                            exit(1);
                            // die("sendto()"); // fails
                        }
                        printTime();
                        printf("\n-=JAIL RELEASE=-");
                        printPackage(ptr->data);
                        free(ptr);
                    }
                }



                printf("\nReached WAITINITCONNECT!, sent syn + ack to client");

                readFdSet = activeFdSet;
                // lägg in time envl

                timeout_t.tv_sec = 3;
                timeout_t.tv_usec = 0;


                if (select(FD_SETSIZE, &readFdSet, NULL, NULL, &timeout_t) < 0) { // blocking, waits until one the FD is set to ready, will keep the ready ones in readFdSet
                    perror("Select failed\n");
                    exit(EXIT_FAILURE);
                }
                else if(FD_ISSET(sock, &readFdSet))
                {

                    if ((recvfrom(sock, &inputBuf, sizeof(Package), 0, (struct sockaddr *) &clientInfo, &slen)) == -1) {
                        // perror(recvfrom());
                        die("recvfrom()");
                    }
                    printTime();
                    printf("\n Input package, possibly ack on syn +ack ");
                    printPackage(inputBuf);

                    packageType = viewPackage(inputBuf);

                    if(packageType == 1) // recieved syn, resend syn+ack
                    {
                        printf("\n recieved syn duplicate, resending ack");
                        jail(&jailList, outputBuf, sock, clientInfo, genError);

                    }
                    else if(packageType == 2) // recieved ack, correct!
                    {
                        //Check if ack correct
                        if(inputBuf.ack != outputBuf.seq || inputBuf.seq != incLowestSeq)
                        {
                            printf("\n ack on ack returned from client! seqcounter != package.ack\n");
                            // do something more?
                            break;
                        }
                        else
                        {
                            //incLowestSeq = outputBuf.ack;
                            incLowestSeq++;
                            printf("\n Input package was ack on syn+ack, success!");

                            currentState = CONNECTED;
                            printf("\n reached connected!");


                        }


                    }
                    else
                    {
                        // weird package?
                    }


                    // break (why is this here?)
                }
                else // resend if timeout, syn+ack lost?
                {

                    jail(&jailList, outputBuf, sock, clientInfo, genError);

                    break;

                }
                //currentState = WAITINITCONNECT;




                break;
            case WAITINGCONFIRMCONNECT:
                // does not exist on host

                break;

            case CONNECTED:
                printf("\n WAITING FOR %zu", incLowestSeq);


                trigger = rand() %100;
                if(trigger > RELEASEPROBABILITY)
                {
                    Node *ptr = freeFromJail(&jailList);
                    if(ptr != NULL)
                    {
                        if (sendto(sock, &ptr->data, sizeof(Package), 0, (struct sockaddr *) &clientInfo, slen) == -1) {
                            die("sendto()");
                        }
                        printTime();
                        printf("\n-=JAIL RELEASE=-");
                        printPackage(ptr->data);
                        free(ptr);
                        ptr = NULL;
                    }
                }


                timeout_t.tv_sec = 60; // Time that we listen for acks b4 resending the window.
                timeout_t.tv_usec = 0;
                if (select(FD_SETSIZE, &readFdSet, NULL, NULL, &timeout_t) < 0) { // blocking, waits until one the FD is set to ready, will keep the ready ones in readFdSet
                    perror("Select failed\n");
                    exit(EXIT_FAILURE);
                }
                else if(FD_ISSET(sock, &readFdSet)) // if true we got a package so read it.
                {
                    if ((recvfrom(sock, &inputBuf, sizeof(Package), 0, (struct sockaddr *) &clientInfo, &slen)) ==
                        -1)
                    {

                        die("recvfrom()");
                    }
                    printTime();
                    printf("\n Incoming package");
                    printPackage(inputBuf);

                    packageType = viewPackage(inputBuf);


                    if(packageType == 3) // ack + seq + data
                    {

                        if(incLowestSeq == inputBuf.seq)  // first expected
                        {

                            addNodeLast(&database, inputBuf); // save to file
                            outputBuf.seq = outSeq;
                            outSeq++;
                            outputBuf.ack = inputBuf.seq;
                            outputBuf.checkSum = 0;
                            outputBuf.checkSum = checksum(outputBuf);
                            incLowestSeq++;

                            jail(&jailList, outputBuf, sock, clientInfo, genError);

                            while(ackBySEQ(&list, incLowestSeq, sock, clientInfo, &database, &outSeq) == true)
                            {
                                incLowestSeq++;
                            }






                        }
                        else if (incLowestSeq > inputBuf.seq) // old package
                        {
                            printf("\n OLD PACKAGE!");
                            jail(&jailList, outputBuf, sock, clientInfo, genError);
                        }
                        else if(incLowestSeq + WINDOWSIZE + 1 > inputBuf.seq) // package in span
                        {
                            if(findSeq(&list, inputBuf.seq) == false)
                            {
                                printf("\n package in span");
                                addNodeLast(&list, inputBuf);
                            } else{
                                printf("\n package already in list");
                            }

                        }
                        else // unmeant condition
                        {
                            printf("WEIRD CONDITION IN CONNECTED! Packages seq is too big");
                            printList(&database);
                            return 1;
                        }

                    }
                    else if(packageType == 4)
                    {
                        if(outputBuf.ack + 1 == inputBuf.seq)
                        {
                            //close file

                            printf("\n FIN RECIEVED!");
                            currentState = INITCLOSE;
                        }
                        else
                        {
                            printf("\n FIN had wrong SEQ?");
                        }

                    }

                }
                else // timeout occured
                {
                    // removes list
                    printf("\n MAJOR TIMEOUT! client unresponsive\n");

                    currentState = CLOSED;
                    // big timeout
                }




                break;
            case INITCLOSE:
                trigger = rand() %100;
                if(trigger > RELEASEPROBABILITY)
                {
                    Node *ptr = freeFromJail(&jailList);
                    if(ptr != NULL)
                    {
                        if (sendto(sock, &ptr->data, sizeof(Package), 0, (struct sockaddr *) &clientInfo, slen) == -1) {
                            perror("sendto()");
                            exit(1);
                            // die("sendto()"); // fails
                        }
                        printf("\n-=JAIL RELEASE=-");
                        printPackage(ptr->data);
                        free(ptr);
                        ptr = NULL;
                    }
                }

                printf("\n Reached INITCLOSE\n");
                readFdSet = activeFdSet;


                timeout_t.tv_sec = 10;
                timeout_t.tv_usec = 0;

                emptyPackage(&outputBuf);

                outputBuf.seq = outSeq;
                outputBuf.ack = inputBuf.seq;
                outputBuf.ack = inputBuf.seq;
                outputBuf.fin = true;
                //outputBuf.checkSum = 0; // aleady empty
                outputBuf.checkSum = checksum(outputBuf);
                jail(&jailList, outputBuf, sock, clientInfo, genError);




                if (select(FD_SETSIZE, &readFdSet, NULL, NULL, &timeout_t) < 0) { // blocking, waits until one the FD is set to ready, will keep the ready ones in readFdSet
                    perror("Select failed\n");
                    exit(EXIT_FAILURE);
                }
                else if(FD_ISSET(sock, &readFdSet))
                {

                    if ((recvfrom(sock, &inputBuf, sizeof(Package), 0, (struct sockaddr *) &clientInfo, &slen)) == -1) {
                        // perror(recvfrom());
                        die("recvfrom()");
                    }
                    printTime();
                    printf("\n Input package, 2nd fin?");
                    printPackage(inputBuf);


                    if(viewPackage(inputBuf) == 4) // wait for type 4 for fin
                    {
                        if(inputBuf.ack == outputBuf.seq)
                        {
                            currentState = CLOSED;
                        }


                    }
                    else
                    {
                        //
                    }


                    // break
                }
                else
                {
                    jail(&jailList, outputBuf, sock, clientInfo, genError);

                }
                //currentState = WAITINITCONNECT;


                break;
            case WAITINGCLOSE:

                break;
            case CLOSED:
                // reminder to remove all linked list

                printf("\n WE REACHED CLOSED!");
                //printList(&database);
                printData(&database);
                printTime();
                return 0;

                break;

        }
    }


}