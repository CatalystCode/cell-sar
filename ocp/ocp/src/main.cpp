/*! @file main.cpp
 *  @version 3.1.8
 *  @date Aug 05 2016
 *
 *  @brief
 *  New Linux App for DJI Onboard SDK.
 *  Provides a number of convenient abstractions/wrappers around core API calls.
 *
 *  Calls are blocking; the calling thread will sleep until the
 *  call returns. Use Core API calls or another sample if you
 *  absolutely need non-blocking calls.
 *
 *  @copyright
 *  2016 DJI. All rights reserved.
 * */

//System Headers
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <stdio.h>
#include <fstream>
#include <signal.h>
#include <time.h>
#include <mqueue.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <sys/types.h>

//JSONCPP Amalgamated
#include <json.h>

//DJI Linux Application Headers
#include "LinuxSerialDevice.h"
#include "LinuxThread.h"
#include "LinuxSetup.h"
#include "LinuxCleanup.h"
#include "ReadUserConfig.h"
#include "LinuxMobile.h"
#include "LinuxFlight.h"
#include "LinuxInteractive.h"
#include "LinuxWaypoint.h"
#include "LinuxCamera.h"

//DJI OSDK Library Headers
#include <DJI_Follow.h>
#include <DJI_Flight.h>
#include <DJI_Version.h>
#include <DJI_WayPoint.h>

#include "MQCommon.h"


//Local Mission Planning Suite Headers
//#include <MissionplanHeaders.h>

using namespace std;
using namespace DJI;
using namespace DJI::onboardSDK;

#define MAX_BUF 100

#define STATUS_FILE "/tmp/sar.log"
//#define ENABLE_DRONE

volatile sig_atomic_t stop;

void inthand(int signum) {
    stop = 1;
}

//! Main function for the Linux sample. Lightweight. Users can call their own API calls inside the Programmatic Mode else on Line 68.
int main(int argc, char *argv[])
{
  signal(SIGINT, inthand);

  #ifdef ENABLE_DRONE
    //! Instantiate a serialDevice, an API object, flight and waypoint objects and a read thread.
    LinuxSerialDevice* serialDevice = new LinuxSerialDevice(UserConfig::deviceName, UserConfig::baudRate);
    CoreAPI* api = new CoreAPI(serialDevice);
    Flight* flight = new Flight(api);
    WayPoint* waypointObj = new WayPoint(api);
    Camera* camera = new Camera(api);
    LinuxThread read(api, 2);

    //! Setup
    int setupStatus = setup(serialDevice, api, &read);
    if (setupStatus == -1)
    {
      std::cout << "This program will exit now. \n";
      return 0;
    }
  #endif
  //! Set broadcast Freq Defaults
  //unsigned short broadcastAck = api->setBroadcastFreqDefaults(1);
  //usleep(500000);

  /* prep queue */
  mqd_t mq;
  struct mq_attr attr;
  char buffer[MAX_SIZE + 1];

  /* initialize the queue attributes */
  attr.mq_flags = 0;
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = MAX_SIZE;
  attr.mq_curmsgs = 0;

  /* create the message queue */
  mq = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY | O_NONBLOCK, 0666, &attr);
  CHECK((mqd_t)-1 != mq);

  /* listen for messages */
   while (!stop)
   {
        ssize_t bytes_read;

        /* receive the message */
        bytes_read = mq_receive(mq, buffer, MAX_SIZE, NULL);
        if (bytes_read >= 0)
        {
          std::cout << "Bytes read: " << bytes_read << "\n" << buffer << "\n";
          try {
              Json::Value root;
              Json::Reader reader;
              reader.parse(buffer, root);

              if (root["type"].asString() == "phy")
              {
                std::string message = root["IMSI"].asString() + "_" + root["data"]["UpRSSI"].asString();

                std::cout << message << "\n";

                #ifdef ENABLE_DRONE
                api->sendToMobile((uint8_t *) message.c_str(), strlen(message.c_str()));
                #endif
              }
            }
            catch (std::exception const & ex) {
              std::cout << ex.what() << "\n";
            }
        }
        sleep(1);
    }

  // Cleanup
  CHECK((mqd_t)-1 != mq_close(mq));

  #ifdef ENABLE_DRONE
  int cleanupStatus = cleanup(serialDevice, api, flight, &read);
  if (cleanupStatus == -1)
  {
    std::cout << "Unable to cleanly destroy OSDK infrastructure. There may be residual objects in the system memory.\n";
    return 0;
  }
  std::cout << "Program exited successfully." << std::endl;
  #endif

  return 0;
}