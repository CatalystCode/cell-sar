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

using namespace std;
using namespace DJI;
using namespace DJI::onboardSDK;

volatile sig_atomic_t stop;


// Handle sigint
void int_hand(int signum)
{
    stop = 1;
}

bool dji_connect(LinuxSerialDevice* serialDevice, CoreAPI* api, LinuxThread* read)
{
    int setupStatus = setup(serialDevice, api, read);
    if (setupStatus == -1) {
        std::cout << "Failed to connect to drone.  Will retry later.\n";
        return false;
    }

    std::cout << "Connected to drone.  Setting broadcast freq to zero.\n";
    api->setBroadcastFreqToZero();

    std::cout << "Broadcast freq configured.\n";
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
