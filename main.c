/*
    Simple udp server
*/
#include<stdio.h> //printf
#include<stdint.h>
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/select.h>



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


uint8_t viewPackage(Package pack)
{

    if(1) // checksum
    {
        if(pack.syn == true) return 1; // syn

        if(pack.fin == true) return 4; // last package

        if(pack.reset == true) return 5; // server resets connection

        if(pack.data == '\0') return 2; // ack + seq, no data

        if(pack.data != '\0') return 3; // ack + seq with data, package to read

        printf("\n ERROR!");
        exit(1);



    }

    //checksum wrong return
    // 0 bad case, (checksum fail)
    // 1 syn (seq, no data, no ack)
    // 2 ack (seq + ack, no data)
    // 3 data (seq + ack + data)
    // 4 fin (fin = true, rest doesn't matter)
    // 5 reset

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

int main(void)
{
    struct sockaddr_in serverInfo, clientInfo, AcceptedHost;
    Package outputBuf, inputBuf;
    fd_set activeFdSet, readFdSet;
    struct timeval timeout_t;

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

                timeout_t.tv_sec = 10;
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
                    printf("\n Inputpackage");
                    printPackage(inputBuf);

                    if(viewPackage(inputBuf) == 1) // syn
                    {
                        emptyPackage(&outputBuf);
                        outputBuf.seq = initSEQ();
                        outputBuf.ack = inputBuf.seq + 1;

                        if (sendto(sock, &outputBuf, sizeof(Package), 0, (struct sockaddr*) &clientInfo, slen) == -1)
                        {
                            die("sendto()");
                        }

                        printf("\n Output package");
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



                printf("\nReached WAITINITCONNECT!");

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
                    printf("\n Input package");
                    printPackage(inputBuf);

                    if(viewPackage(inputBuf) == 1) // recieved syn, resend syn+ack
                    {

                        if (sendto(sock, &outputBuf, sizeof(Package), 0, (struct sockaddr*) &clientInfo, slen) == -1)
                        {
                            die("sendto()");
                        }
                        printf("\n Output package");
                        printPackage(outputBuf);

                        currentState = WAITINITCONNECT;

                    }
                    else if(viewPackage(inputBuf) == 2) // recieved ack, correct!
                    {
                        currentState = CONNECTED;

                    }
                    else
                    {
                        //
                    }


                    // break
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
                getchar();

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