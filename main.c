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
    uint64_t seq;
    uint64_t ack;
    uint16_t timeStamp;
    char data;
    uint64_t checkSum;
} Package;


void die(char *s)
{
    perror(s);
    exit(1);
}

int main(void)
{
    struct sockaddr_in serverInfo, clientInfo, AcceptedHost;
    Package buf;
    fd_set activeFdSet, readFdSet;

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


                if (select(FD_SETSIZE, &readFdSet, NULL, NULL, NULL) < 0) { // blocking, waits until one the FD is set to ready, will keep the ready ones in readFdSet
                    perror("Select failed\n");
                    exit(EXIT_FAILURE);
                }
                else if(FD_ISSET(sock, &readFdSet))
                {
                    if ((recv_len = recvfrom(sock, &buf, sizeof(Package), 0, (struct sockaddr *) &clientInfo, &slen)) == -1) {
                        // perror(recvfrom());
                        die("recvfrom()");
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


                break;
            case WAITINGCONFIRMCONNECT:

                break;

            case CONNECTED:

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