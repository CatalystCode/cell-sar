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

#include "sarqueue.h"
#include "from_fc.pb.h"

#define CMD_TYPE_SIZE 10
#define PLMN_REQUEST "plmn\0\0\0\0\0\0"
#define SMS_REQUEST  "sms\0\0\0\0\0\0\0"

using namespace std;
using namespace DJI;
using namespace DJI::onboardSDK;
using namespace com::microsoft::cellsar::protobuf;

volatile sig_atomic_t stop;

// Handle sigint
void int_hand(int signum) {
    stop = 1;
}

// inspired by 
// https://github.com/MenchiG/DJI-Onboard-SDK-Raspberry-Transparent-Transmit/blob/master/pm25/src/DJI_LIB/DJI_Pro_App.cpp
void ocpFromMobileCallback(DJI::onboardSDK::CoreAPI *core_api, Header *header, 
      UserData user_data) {

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

void poll_messages(LinuxSerialDevice* serialDevice, CoreAPI* api, LinuxThread* read)
{
  bool droneConnected = false;
  int iterationsUntilAttempt = 10;

  droneConnected = dji_connect(serialDevice, api, read);

  MQCommon::init(OCP);

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

            std::cout << "----------" << std::endl;
            std::cout << buffer << "\n";
            std::cout << "----------" << std::endl;

            try {
                Json::Value root;
                Json::Reader reader;
                reader.parse(buffer, root);

                /* Signal strength messages for a phone */
                if (root["type"].asString() == "phy") {
                    std::string message = root["data"]["IMSI"].asString() + "_" + root["data"]["UpRSSI"].asString();

                    std::cout << message << "\n";

                    if (droneConnected) {
                        api->sendToMobile((uint8_t*)message.c_str(), strlen(message.c_str()));
                    }
                }
                /* Other status messages */
                else if (root["type"].asString() == "status") {

                    // Prefix message with a !bang! to signal to the mobile device this is a status message.
                    std::string message = "!ping"; // We can pull a message field if there is a meaningful one, later.

                    std::cout << message << "\n";

                    if (droneConnected) {
                        api->sendToMobile((uint8_t*)message.c_str(), strlen(message.c_str()));
                    }
                }
                else if (root["type"].asString() == "sms") {

                    char buffer[100];
                    memcpy(buffer, "sms", 3);

                    std::string imsi = root["data"]["subscriber"]["imsi"].asString();
                    memcpy(buffer + 10, imsi.c_str(), 15);

                    std::string text = root["data"]["msg"].asString();
                    size_t text_len = text.length();
                    if (text_len > 75) text_len = 75;
                    memcpy(buffer + 25, text.c_str(), text_len);

                    if (droneConnected)
                        api->sendToMobile((uint8_t *)buffer, 25 + text_len);
                }
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
