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

#include "sarqueue.h"

volatile sig_atomic_t stop;

using namespace std;

/* Interupt handler */
void inthand(int signum) {
  stop = 1;
}

void makePhyMessage(char **buffer);
void makeStatusMessage(char **buffer);

int main(int argc, char **argv)
{
    /* Register for sigint */
    signal(SIGINT, inthand);

    for (int i = 0; !stop; i = (i + 1) % 5) {
       char *buffer;
       if (i == 4)
          makeStatusMessage(&buffer);
       else
          makePhyMessage(&buffer);
       std::cout << buffer << std::endl;

       MQCommon::push(buffer);
       std::cout << "simulator sent " << strlen(buffer) << " bytes\n";
       std::cout << "----------" << std::endl;

       sleep(2);
       free(buffer);
    }

    return 0;
}

void makePhyMessage(char **buffer) {
   const char *phyTemplate = "{\"type\": \"phy\", \"time\": 1502129624628, \"data\": {"
      "\"IMSI\": \"111222333444555\", \"TMSI\": \"007b0001\", \"UpRSSI\": %i, \"DnRSSIdBm\": -82, \"time\": %u"
      "}}\n";

   int upRSSI = -44;

   asprintf(buffer, phyTemplate, upRSSI, (unsigned)time(NULL));
}

void makeStatusMessage(char **buffer) {
   const char *imsi = "111222333444555";
   const char *tmsi = "007b0001";
   const char *msisdn = "17242177";
   unsigned int pendingSMSs = 3;

   const char *statusTemplate = "{\"type\": \"status\", \"time\": 1502129624628, \"data\": {"
      "\"subscribers\": [{\"imsi\": %s, \"tmsi\": %s, \"msisdn\": %s}], \"pendingSMSs\": %d"
      "}}\n";

   asprintf(buffer, statusTemplate, imsi, tmsi, msisdn, pendingSMSs);
}

