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

void MQCommon::push(char* buffer)
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
    	return MQCommon::buffer;
    }

    return NULL;
}
