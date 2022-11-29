#include <sys/types.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 256
#define QNAME "my_queue"
#define PRIORITY 1

int main()
{
    char *msg[BUFSIZE];
    mqd_t msg_queue;
    struct mq_attr q_attr;
    q_attr.mq_maxmsg = 10;
    q_attr.mq_msgsize = BUFSIZE;

    if ((msg_queue = mq_open("/msg_queue", O_CREAT | O_RDWR, 0666, &q_attr)) == -1)
    {
        perror("mq_open failed");
        exit(1);
    }

    while (1)
    {
        if (mq_receive(msg_queue, msg, BUFSIZE, NULL) == -1)
        {
            perror("mq_receive failed");
            exit(1);
        }
        printf("receive : %s\n", msg);
        memset(msg, 0, 256);
    }
    
    if (mq_close(msg_queue) == -1)
    {
        perror("mq_close failed");
        exit(1);
    }
    if (mq_unlink(QNAME) == -1)
    {
        perror("mq_unlink failed");
        exit(1);
    }

    return 0;
}
