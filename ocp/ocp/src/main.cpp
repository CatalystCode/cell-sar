/*! @file main.cpp
 *  @date 7/30/2017
 *
 *  @brief
 *  Microsoft OCP program for interfacing between detection radios
 *  and DJI drones
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
#include <fcntl.h> /* For O_* constants */
#include <sys/stat.h> /* For mode constants */
#include <sys/types.h>

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

#include <google/protobuf/util/json_util.h>
#include "sarqueue.h"
#include "from_fc.pb.h"
#include "from_yate.pb.h"

#define CMD_TYPE_SIZE 10
#define PLMN_REQUEST "plmn\0\0\0\0\0\0"
#define SMS_REQUEST  "sms\0\0\0\0\0\0\0"

using namespace std;
using namespace DJI;
using namespace DJI::onboardSDK;
using namespace com::microsoft::cellsar::protobuf;
using namespace google::protobuf::util;

volatile sig_atomic_t stop;

// Handle sigint
void int_hand(int signum) {
    stop = 1;
}

// inspired by 
// https://github.com/MenchiG/DJI-Onboard-SDK-Raspberry-Transparent-Transmit/blob/master/pm25/src/DJI_LIB/DJI_Pro_App.cpp
void ocpFromMobileCallback(DJI::onboardSDK::CoreAPI *core_api, Header *header, 
      UserData user_data) {

   std::cout << "------------------------------" << std::endl;
   std::cout << "FromMobileCallback executing..." << std::endl;

   // get the buffer
   unsigned int buflen = header->length - EXC_DATA_SIZE - 2 > 100
      ? 100 : (header->length - EXC_DATA_SIZE - 2);
   char *buffer = (char*)header + sizeof(Header) + 2;
   std::cout << "   obtained byte buffer of length " << buflen << std::endl;

   // check if it is a custom request
   std::cout << "   checking if the buffer is a custom request" << std::endl;
   fc::FcMessage fc_message;
   bool result = fc_message.ParseFromArray((const void *)buffer, buflen);

   if (!result) {

      // fallback to default behavior
      std::cout << "   no custom request, fallback to default behavior" << std::endl;
      core_api->parseFromMobileCallback(core_api, header, user_data);

   } else {

      // pass to yate
      std::cout << "   passing message to yate" << std::endl;
      std::string for_yate(buffer, buflen);
      MQCommon::push(for_yate.c_str());

   }
   std::cout << "------------------------------" << std::endl;
}

bool dji_connect(LinuxSerialDevice* serialDevice, CoreAPI* api, LinuxThread* read)
{
    // connect to drone
    int setupStatus = setup(serialDevice, api, read);
    if (setupStatus == -1) {
        std::cout << "Failed to connect to drone.  Will retry later.\n";
        return false;
    }

    // restrict broadcasts
    std::cout << "Connected to drone.  Setting broadcast freq to zero.\n";
    api->setBroadcastFreqToZero();
    std::cout << "Broadcast freq configured.\n";

    // setup callbacks
    api->setFromMobileCallback(ocpFromMobileCallback, NULL);

    return true;
}

bool handle_yate_message(char *buffer, unsigned int buflen, std::string *message) {
   std::string result;
   std::cout << "------------------------------" << std::endl;
   std::cout << "Read byte buffer of size " << buflen << " from YATE" << std::endl;

   std::cout << "   Verifying that the buffer is a YateMessage" << std::endl;
   yate::YateMessage yate_message;
   bool is_yate_message = yate_message.ParseFromArray((const void *)buffer, buflen);
   if (!is_yate_message) {
      std::cout << "   Could not convert byte array to YateMessage. Skipping." << std::endl;
   std::cout << "------------------------------" << std::endl;
      return false;
   }

   // convert to JSON and print for debugging purposes.
   std::string json("");
   Status status = MessageToJsonString(yate_message, &json);
   if (!status.ok())
      std::cout << "   Failed to turn YateMessage into JSON, passing along anyways." << std::endl;
   else 
      std::cout << "   " << json << std::endl;

   std::cout << "   Passing YateMessage to the flight controller." << std::endl;
   std::cout << "------------------------------" << std::endl;
   *message = result;
   return true;
}

void poll_messages(LinuxSerialDevice* serialDevice, CoreAPI* api, LinuxThread* read)
{
  bool droneConnected = false;
  int iterationsUntilAttempt = 10;

  droneConnected = dji_connect(serialDevice, api, read);

  MQCommon::init(OCP);

  std::string message;
  while (!stop) {
        char* buffer;
        unsigned int buflen = MQCommon::pop(&buffer);

        if (buffer != NULL) {
            // attempt drone connection
            if (!droneConnected && iterationsUntilAttempt <= 0)
            {
              droneConnected = dji_connect(serialDevice, api, read);
              iterationsUntilAttempt = 10;
            }

            try {
               if (handle_yate_message(buffer, buflen, &message) && droneConnected)
                  api->sendToMobile((uint8_t *)message.c_str(), strlen(message.c_str()));
            }
            catch (std::exception const& ex) {
                std::cout << ex.what() << "\n";
            }
        }
        usleep(500000); // Half a second
        if (!droneConnected) iterationsUntilAttempt--;
    }

}

int main(int argc, char* argv[])
{
    signal(SIGINT, int_hand);

    std::cout << "OCP starting up on serial port " << UserConfig::deviceName << " at " << UserConfig::baudRate << " baud.\n";

    /* initialize serial port and listener threads */
    LinuxSerialDevice* serialDevice = new LinuxSerialDevice(UserConfig::deviceName, UserConfig::baudRate);
    CoreAPI* api = new CoreAPI(serialDevice);
    Flight* flight = new Flight(api);
    WayPoint* waypointObj = new WayPoint(api);
    Camera* camera = new Camera(api);
    LinuxThread read(api, 2);

    /* listen for messages (blocking) */
    poll_messages(serialDevice, api, &read);

    /* cleanup resources */
    int cleanupStatus = cleanup(serialDevice, api, flight, &read);
    if (cleanupStatus == -1) {
        std::cout << "Unable to cleanly destroy OSDK infrastructure. There may be residual objects in the system memory.\n";
        return 0;
    }

    std::cout << "Program exited successfully." << std::endl;
    return 0;
}

