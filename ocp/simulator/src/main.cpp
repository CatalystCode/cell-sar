#include <iostream>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <mqueue.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

#include "MQCommon.h"

volatile sig_atomic_t stop;

using namespace std;

/* Interupt handler */
void inthand(int signum) {
  stop = 1;
}

int main(int argc, char **argv)
{
    /* Register for sigint */
    signal(SIGINT, inthand);

    mqd_t mq;
    struct mq_attr attr;

    /* create queue attributes */
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    /* create the message queue if does not exist and open for writing */
    mq = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY | O_NONBLOCK, 0666, &attr);
    CHECK((mqd_t)-1 != mq);

    while (!stop)
    {
       /* send the message */
       char *buffer;
       int upRSSI = -44;

       asprintf (&buffer, "{\"IMSI\": \"310150123456789\", \"type\": \"phy\", \"timestamp\""
        ": \"1499879446\", \"data\": {\"UpRSSI\": %i, \"TxPwr\": 33, \"DnRSSIdBm\": -82, \"time\": %u}}", upRSSI, (unsigned)time(NULL));
       CHECK(0 <= mq_send(mq, buffer, strlen(buffer), 0));

       std::cout << "sent: " << strlen(buffer) << "\n";

       /* wait */
       sleep(2);
    }

    /* cleanup */
    CHECK((mqd_t)-1 != mq_close(mq));

    return 0;
}