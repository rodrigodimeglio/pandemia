#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <pthread.h>
#include <ncurses.h>
#include <time.h>
#include <signal.h>
#include <sys/time.h>
#include <errno.h>
#include <ctype.h>
#include <semaphore.h>

#define     MSGSZ           32
#define     MSGBUFFSZ       8  
#define     MSGMASK         7

typedef struct
{
    sem_t *mtx;
    sem_t *mtx_go;
    char msgs[MSGBUFFSZ][MSGSZ];
    int putter;
    int getter;
}MSGManager_t;

extern bool RUNFlag;

extern MSGManager_t MSGMngr;

void MSGInit(void);

void MSGPut(char * msg, int msglen);

void * MSGSender(void);

void MSGDestroy(void);