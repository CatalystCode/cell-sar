#include "sarqueue.h"

// forward declarations
mqd_t MQCommon::yate2ocp;
struct mq_attr MQCommon::yate2ocp_attr;
char MQCommon::yate2ocp_buffer[YATE_2_OCP_MAX_SIZE + 1];
mqd_t MQCommon::ocp2yate;
struct mq_attr MQCommon::ocp2yate_attr;
char MQCommon::ocp2yate_buffer[OCP_2_YATE_MAX_SIZE + 1];


bool MQCommon::initialized = false;
SARProgram MQCommon::mode = YATE;

void MQCommon::init(SARProgram prog) {

   MQCommon::mode = prog;
   
   MQCommon::yate2ocp_attr.mq_flags = MQCommon::ocp2yate_attr.mq_flags = 0;
   MQCommon::yate2ocp_attr.mq_maxmsg = MQCommon::ocp2yate_attr.mq_maxmsg = 10;
   MQCommon::yate2ocp_attr.mq_msgsize = YATE_2_OCP_MAX_SIZE;
   MQCommon::ocp2yate_attr.mq_msgsize = OCP_2_YATE_MAX_SIZE;
   MQCommon::yate2ocp_attr.mq_curmsgs = MQCommon::ocp2yate_attr.mq_curmsgs = 0;

   if (MQCommon::mode == OCP) {
      MQCommon::yate2ocp = mq_open(YATE_2_OCP_QUEUE_NAME,  O_CREAT | O_RDONLY | O_NONBLOCK, 0666, &MQCommon::yate2ocp_attr);   
      MQCommon::ocp2yate = mq_open(OCP_2_YATE_QUEUE_NAME,  O_CREAT | O_WRONLY | O_NONBLOCK, 0666, &MQCommon::ocp2yate_attr);
   } else {
      MQCommon::yate2ocp = mq_open(YATE_2_OCP_QUEUE_NAME,  O_CREAT | O_WRONLY | O_NONBLOCK, 0666, &MQCommon::yate2ocp_attr);   
      MQCommon::ocp2yate = mq_open(OCP_2_YATE_QUEUE_NAME,  O_CREAT | O_RDONLY | O_NONBLOCK, 0666, &MQCommon::ocp2yate_attr);
   }

   MQCommon::initialized = true;
}

void MQCommon::cleanup() {
   mq_close(MQCommon::yate2ocp);
   mq_close(MQCommon::ocp2yate);
   MQCommon::initialized = false;
}

void MQCommon::push(const char *buffer, unsigned int buflen) {
   if (!MQCommon::initialized) MQCommon::init();

   if (MQCommon::mode == OCP)
      mq_send(MQCommon::ocp2yate, buffer, buflen, 0);
   else
      mq_send(MQCommon::yate2ocp, buffer, buflen, 0);
}

unsigned int MQCommon::pop(char **buffer) {
   if (!MQCommon::initialized) MQCommon::init();
   int bytes_read;

   if (MQCommon::mode == OCP) {
      bytes_read = mq_receive(MQCommon::yate2ocp, MQCommon::yate2ocp_buffer, YATE_2_OCP_MAX_SIZE, NULL);
      if (bytes_read >= 0) {
         *buffer = MQCommon::yate2ocp_buffer;
         return bytes_read;
      }
   } else {
      bytes_read = mq_receive(MQCommon::ocp2yate, MQCommon::ocp2yate_buffer, OCP_2_YATE_MAX_SIZE, NULL);
      if (bytes_read >= 0) {
         *buffer = MQCommon::ocp2yate_buffer;
         return bytes_read;
      }
   }

   *buffer = NULL;
   return 0;
}
