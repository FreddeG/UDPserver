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

#define WINDOWSIZE 3

/*
typedef struct {

    bool fin;
    bool reset;
    bool syn;
    uint64_t seq;
    uint64_t ack;
    uint16_t timeStamp;
    char data;
    uint64_t checkSum;
} Package;
*/

/*
typedef struct {

    Package pack;
    bool ack;

} PackageList;

void die(char *s)
{
    perror(s);
    exit(1);
}

void printPackage(Package pack)
{
    printf("\n fin: %d", pack.fin);
    printf("\n reset: %d", pack.reset);
    printf("\n syn: %d", pack.syn);
    printf("\n seq: %u", pack.seq);
    printf("\n ack: %u", pack.ack);
    printf("\n data: %c", pack.data);
    printf("\n checksum: %u\n", pack.checkSum);

}

// checks the type
// 0 bad case, (checksum fail)
// 1 syn (seq, no data, no ack)
// 2 ack (seq + ack, no data)
// 3 data (seq + ack + data)
// 4 fin (fin = true, rest doesn't matter)
// 5 reset of the package, and returns a corresponding value

uint8_t packageType(Package pack)
{
    if(pack.syn == true) return 1; // syn

    if(pack.fin == true) return 4; // last package

    if(pack.reset == true) return 5; // server resets connection

    if(pack.data == '\0') return 2; // ack + seq, no data

    if(pack.data != '\0') return 3; // ack + seq with data, package to read

    printf("\n ERROR!");
    exit(1);

}

//checks that timestamp and CRC is good, returns packagetype if success, else 0
uint8_t errorCheckPackage(Package pack)
{
    //checks crc/checksum

        // checks timestamp




}

void emptyPackage(Package *packToEmpty)
{
    packToEmpty->fin = false;
    packToEmpty->reset = false;
    packToEmpty->syn = false;
    packToEmpty->seq = 0;
    packToEmpty->ack = 0;
    packToEmpty->timeStamp = 0;
    packToEmpty->data = '\0';
    packToEmpty->checkSum = 0;

}

uint64_t initSEQ(void)
{

    return 10;
}

uint16_t getTimeStamp(void)
{
    // make sure to compare absolute value between current time and timestamp, check if bigger than x minutes/seconds

    // 0-3 min ok, 57-60 ok check values
    return 0;
}

*/

int main(void)
{
    struct sockaddr_in serverInfo, clientInfo, AcceptedHost;
    Package outputBuf, inputBuf;
    fd_set activeFdSet, readFdSet;
    struct timeval timeout_t;
    uint64_t incLowestSeq;

    List list;
    list.head = NULL;

    List database;
    database.head = NULL;

    FILE *fp = NULL;

    int sock, i, slen = sizeof(clientInfo) , recv_len;
    //char buf[BUFLEN];

    //create a UDP socket
    if ((sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
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
            case INITCONNECT:
                readFdSet = activeFdSet;
                // lägg in time envl

                timeout_t.tv_sec = 120;
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
                    printf("\n Input package, first syn");
                    printPackage(inputBuf);

                    if(viewPackage(inputBuf) == 1) // syn
                    {
                        emptyPackage(&outputBuf);
                        //outputBuf.seq = initSEQ(); // inputseq. not for incoming!!
                        outputBuf.seq = 0;
                        outputBuf.ack = inputBuf.seq;
                        incLowestSeq = inputBuf.seq + 1;

                        outputBuf.checkSum = checksum(outputBuf);

                        if (sendto(sock, &outputBuf, sizeof(Package), 0, (struct sockaddr*) &clientInfo, slen) == -1)
                        {
                            die("sendto()");
                        }

                        printf("\n Output package, output first syn + ack"); // no syn state but is no data, only seq + ack
                        printPackage(outputBuf);

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
                    printf("\n Input package, possibly ack on syn +ack ");
                    printPackage(inputBuf);

                    if(viewPackage(inputBuf) == 1) // recieved syn, resend syn+ack
                    {

                        if (sendto(sock, &outputBuf, sizeof(Package), 0, (struct sockaddr*) &clientInfo, slen) == -1)
                        {
                            die("sendto()");
                        }
                        printf("\n Output package, was not ack on syn + ack, resending syn+ack");
                        printPackage(outputBuf);

                        //currentState = WAITINITCONNECT;

                    }
                    else if(viewPackage(inputBuf) == 2) // recieved ack, correct!
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

                            if(fp == NULL)
                            {
                                fp = fopen ("file.txt", "w");
                            }
                            else
                            {
                                return 0; // solve loop somehow to exit when EOF is found
                            }
                            if(fp == NULL) // error
                            {
                                die("fopen");
                            }
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
                    if (sendto(sock, &outputBuf, sizeof(Package), 0, (struct sockaddr*) &clientInfo, slen) == -1)
                    {
                        die("sendto()");
                    }
                    printf("\n Output package");
                    printPackage(outputBuf);

                    break;

                }
                //currentState = WAITINITCONNECT;



                break;
            case WAITINGCONFIRMCONNECT:
                // does not exist on host

                break;

            case CONNECTED:
                printf("\n reached connected!");


                timeout_t.tv_sec = 120; // Time that we listen for acks b4 resending the window.
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
                        // perror(recvfrom());
                        die("recvfrom()");
                    }
                    printf("\n Incoming package");
                    printPackage(inputBuf);

                    if(viewPackage(inputBuf) == 3) // ack + seq + data (we are not using checksum)
                    {
                        // Oldpack! Discard!
                        if(incLowestSeq == inputBuf.seq)  // first expected
                        {
                            /*
                            if(list.head->data.seq + WINDOWSIZE <= inputBuf.ack)// Should not happen!
                            {
                                printf("\nDebug: List exeeded");
                                getchar();
                                exit(1);
                            }
                             */

                            // send ack
                            //outputBuf.seq
                            addNodeLast(&database, inputBuf); // save to file

                            outputBuf.ack = inputBuf.seq;
                            outputBuf.checkSum = 0;
                            outputBuf.checkSum = checksum(outputBuf);

                            if (sendto(sock, &outputBuf, sizeof(Package), 0, (struct sockaddr*) &clientInfo, slen) == -1)
                            {
                                die("sendto()");
                            }
                            printf("\n Sending ack for expected SEQ");
                            printPackage(outputBuf);


                            list.head = ackBySEQRecursive(list.head, &incLowestSeq, sock, clientInfo, &database);




                        }
                        else if (incLowestSeq > inputBuf.seq) // old package
                        {
                            //fix resend last outputbuff
                            if (sendto(sock, &outputBuf, sizeof(Package), 0, (struct sockaddr*) &clientInfo, slen) == -1)
                            {
                                die("sendto()");
                            }
                            printf("\n Recieved old SEQ Output package");
                            printPackage(outputBuf);
                        }
                        else if(incLowestSeq + WINDOWSIZE + 1 > inputBuf.seq) // package in span
                        {
                            addNodeLast(&list, inputBuf);

                        }
                        else // unmeant condition
                        {
                            printf("WEIRD CONDITION IN CONNECTED! Packages seq is too big");
                            printList(&database);
                            return 1;
                        }

                    }

                }
                else
                {
                    // removes list
                    printf("\MAJOR TIMEOUT! client unresponsive\n");

                    if(fp != NULL) close(fp);
                    removeList(&list);
                    currentState = CLOSED;
                    // big timeout
                }


               // printList(&list);

                break;
            case INITCLOSE:

                break;
            case WAITINGCLOSE:

                break;
            case CLOSED:

                break;

        }
    }
/*
    //keep listening for data
    while(1)
    {
        printf("Waiting for data...");
        fflush(stdout);

        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, &buf, sizeof(Package), 0, (struct sockaddr *) &clientInfo, &slen)) == -1)
        {
           // perror(recvfrom());
            die("recvfrom()");
        }


        //print details of the client/peer and the data received
        printf("Received packet from %s:%d\n", inet_ntoa(clientInfo.sin_addr), ntohs(clientInfo.sin_port));
        printf("Data: %d %d %s\n" , buf.seq, buf.ack, buf.data);


        //now reply the client with the same data
        if (sendto(s, &buf, sizeof(Package), 0, (struct sockaddr*) &clientInfo, slen) == -1)
        {
            die("sendto()");
        }
    }

    close(s);
    */
    return 0;
}