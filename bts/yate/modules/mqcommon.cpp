/** mqcommon.cpp: cpp file for a POSIX message queue to send messages 
  * to the OCP
  *    
  * Cellular Search and Rescue - Cellular Sensor BTS
  *   Copyright (C) 2017 Microsoft
  * 
  * This file is part of cell-sar
  * 
  * cell-sar is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 2 of the License, or
  * (at your option) any later version.
  * 
  * cell-sar is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  * 
  * You should have received a copy of the GNU General Public License
  * along with cell-sar.  If not, see <http://www.gnu.org/licenses/>.
  */


#include <mqueue.h>
#include <stdio.h>
#include <string.h>

#include "mqcommon.h"

bool initialized = false;

mqd_t MQCommon::mq;
struct mq_attr MQCommon::attr;
char MQCommon::buffer[MAX_SIZE + 1];

void MQCommon::init(bool write)
{
    /* create queue attributes */
    MQCommon::attr.mq_flags = 0;
    MQCommon::attr.mq_maxmsg = 10;
    MQCommon::attr.mq_msgsize = MAX_SIZE;
    MQCommon::attr.mq_curmsgs = 0;

    if (write)
    {
	    /* create the message queue if does not exist and open for writing */
	    MQCommon::mq = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY | O_NONBLOCK, 0666, &MQCommon::attr);
	}
	else
	{
		  /* create the message queue for reading */
		  MQCommon::mq = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY | O_NONBLOCK, 0666, &MQCommon::attr);
	}

	initialized = true;
}

void MQCommon::cleanup()
{
	mq_close(MQCommon::mq);
}

void MQCommon::push(const char* buffer)
{
	if (!initialized) MQCommon::init(true);

	mq_send(MQCommon::mq, buffer, strlen(buffer), 0);
}

char* MQCommon::pop()
{
	if (!initialized) MQCommon::init(false);

	int bytes_read = mq_receive(mq, MQCommon::buffer, MAX_SIZE, NULL);

    if (bytes_read >= 0)
    {
    	return &MQCommon::buffer[0];
    }

    return NULL;
}
