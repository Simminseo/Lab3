#include <sys/types.h>
#include <mqueue.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFFER 256
#define QNAME "/msg_queue"
#define PRIORITY 1

int main(){
    char *msg[BUFFER];
    int prio;
    mqd_t msg_queue;
    struct mq_attr q_attr;
    q_attr.mq_maxmsg = 10;
    q_attr.mq_msgsize = BUFFER;

    if((msg_queue = mq_open(QNAME, O_CREAT | O_RDWR | O_NONBLOCK, 0600, &q_attr)) == -1){
        perror("mq_open");
        exit(1);
    }
    
    while(1){
        printf("send msg :");
        gets(msg);
        if(mq_send(msg_queue, msg, strlen(msg), PRIORITY)==-1){
            perror("mq_send");
            exit(1);
        }
        if(!strcmp(msg, "end")){break;}
        memset(msg, 0, 256);
    }
    if (mq_close(msg_queue) == -1) {
        perror ("mq_close");
        exit (1);
    }
    if (mq_unlink(QNAME) == -1){
        perror("mq_unlink");
        exit(1);
    }

    return 0;
}
