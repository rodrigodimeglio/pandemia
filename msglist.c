#include "msglist.h"
#include "main.h"

void MSGInit(void)
{
    sem_unlink("/mtx");
    sem_unlink("/mtx_go");
    MSGMngr.mtx = sem_open("/mtx", O_CREAT|O_EXCL, S_IRWXU, 1);
    MSGMngr.mtx_go = sem_open("/mtx_go", O_CREAT|O_EXCL, S_IRWXU, 0);
}


void MSGPut(char * msg, int msglen)
{
    sem_wait(MSGMngr.mtx);
    memcpy(&MSGMngr.msgs[MSGMngr.putter & MSGMASK],msg, msglen);
    MSGMngr.putter++;
    sem_post(MSGMngr.mtx);
    sem_post(MSGMngr.mtx_go);
}

void * MSGSender(void)
{
    MSGInit();
    int i,ret,n;
    PLAYERListNode_t node;
    ToSend_t tosend;

    tosend.cmd.cmd = CMD_TXT;
    tosend.cmd.len = MSGSZ;

    while(RUNFlag)
    {
        sem_wait(MSGMngr.mtx_go);
        sem_wait(MSGMngr.mtx);


        memcpy(&tosend.buf,&MSGMngr.msgs[MSGMngr.getter & MSGMASK],MSGSZ);
        while(n<MSGSZ)
        {
            if(tosend.buf[n]=='\0') break;
            n++;
        }
        MSGMngr.getter++;
        sem_post(MSGMngr.mtx);
        tosend.cmd.len = n+1;

        for(i =0; i<_clients; i++)
        {
            node = PLAYERListGetByidx(plist,i);
            sendt(node.fd,(char *)&tosend, sizeof(tosend),&node.pmtx,"sender error");
        }

    }
    return 0;
}


void MSGDestroy(void)
{
    sem_close(MSGMngr.mtx);
    sem_close(MSGMngr.mtx_go);
    sem_unlink("/mtx");
    sem_unlink("/mtx_go");
   
}