#pragma once
#ifndef _SARQUEUE_H
#define _SARQUEUE_H

#include <mqueue.h>
#include <stdio.h>
#include <string.h>

#define YATE_2_OCP_QUEUE_NAME "/yate2ocp"
#define YATE_2_OCP_MAX_SIZE 4096
#define OCP_2_YATE_QUEUE_NAME "/ocp2yate"
#define OCP_2_YATE_MAX_SIZE 4096

enum SARProgram { YATE, OCP };

class MQCommon {
   
   public:

      static void push(const char *buffer, unsigned int buflen);
      static unsigned int pop(char **buffer);
      static void init(SARProgram prog = YATE);
      static void cleanup();

   private:

      static bool initialized;
      static SARProgram mode;

      // yate to ocp
      static mqd_t yate2ocp;
      static struct mq_attr yate2ocp_attr;
      static char yate2ocp_buffer[YATE_2_OCP_MAX_SIZE + 1];

      // ocp to yate
      static mqd_t ocp2yate;
      static struct mq_attr ocp2yate_attr;
      static char ocp2yate_buffer[OCP_2_YATE_MAX_SIZE + 1];

};

#endif
