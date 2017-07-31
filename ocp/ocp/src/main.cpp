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

#include "mqcommon.h"

using namespace std;
using namespace DJI;
using namespace DJI::onboardSDK;

volatile sig_atomic_t stop;

// Handle sigint
void inthand(int signum)
{
    stop = 1;
}

int main(int argc, char* argv[])
{
    signal(SIGINT, inthand);

    /* Check for first param of "testmode" to disable connecting to drone */
    bool enableDrone = argc <= 1 && argv[1] != "testmode";

    if (enableDrone) {
        std::cout << "Starting in production mode and connecting to drone.\n";
    }
    else {
        std::cout << "TEST MODE - WILL NOT CONNECT TO DRONE.\n";
    }

    /* Connect to the DJI drone via Serial */
    LinuxSerialDevice* serialDevice = new LinuxSerialDevice(UserConfig::deviceName, UserConfig::baudRate);
    CoreAPI* api = new CoreAPI(serialDevice);
    Flight* flight = new Flight(api);
    WayPoint* waypointObj = new WayPoint(api);
    Camera* camera = new Camera(api);
    LinuxThread read(api, 2);

    if (enableDrone) {
        //! Setup
        int setupStatus = setup(serialDevice, api, &read);
        if (setupStatus == -1) {
            std::cout << "This program will exit now. \n";
            return 0;
        }
    }

    //! Set broadcast Freq Defaults later to get gps broadcasts or reduce flooding serial channel
    //unsigned short broadcastAck = api->setBroadcastFreqDefaults(1);
    //usleep(500000);

    /* listen for messages */
    while (!stop) {
        char* buffer = MQCommon::pop();

        if (buffer != NULL) {
            std::cout << buffer << "\n";
            try {
                Json::Value root;
                Json::Reader reader;
                reader.parse(buffer, root);

                /* Signal strength messages for a phone */
                if (root["type"].asString() == "phy") {
                    std::string message = root["IMSI"].asString() + "_" + root["data"]["UpRSSI"].asString();

                    std::cout << message << "\n";

                    if (enableDrone) {
                        api->sendToMobile((uint8_t*)message.c_str(), strlen(message.c_str()));
                    }
                }
                /* Other status messages */
                else if (root["type"].asString() == "status") {

                    // Prefix message with a !bang! to signal to the mobile device this is a status message.
                    std::string message = "!ping"; // We can pull a message field if there is a meaningful one, later.

                    std::cout << message << "\n";

                    if (enableDrone) {
                        api->sendToMobile((uint8_t*)message.c_str(), strlen(message.c_str()));
                    }
                }
            }
            catch (std::exception const& ex) {
                std::cout << ex.what() << "\n";
            }
        }
        usleep(500000);
    }

    if (enableDrone) {
        int cleanupStatus = cleanup(serialDevice, api, flight, &read);
        if (cleanupStatus == -1) {
            std::cout << "Unable to cleanly destroy OSDK infrastructure. There may be residual objects in the system memory.\n";
            return 0;
        }
        std::cout << "Program exited successfully." << std::endl;
    }

    return 0;
}