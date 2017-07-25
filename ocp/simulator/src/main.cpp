#include <iostream>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

#include "mqcommon.h"

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

    while (!stop)
    {
       /* send the message */
       char *buffer;
       int upRSSI = -44;

       asprintf (&buffer, "{\"IMSI\": \"310150123456789\", \"type\": \"phy\", \"timestamp\""
        ": \"1499879446\", \"data\": {\"UpRSSI\": %i, \"TxPwr\": 33, \"DnRSSIdBm\": -82, \"time\": %u}}", upRSSI, (unsigned)time(NULL));

       MQCommon::push(buffer);

       std::cout << "simulator sent " << strlen(buffer) << " bytes\n";

       /* wait */
       sleep(3);
    }

    return 0;
}