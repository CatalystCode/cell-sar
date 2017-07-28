#include <mqueue.h>

#define QUEUE_NAME  "/sar"
#define MAX_SIZE    4096

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: ", __func__, __LINE__); perror(#x); exit(-1); } } while (0)

class MQCommon
{
	public:
  		static void push(const char* buffer);
  		static char* pop();
  		static void cleanup();
	private:
		static void init(bool write);
		static mqd_t mq;
		static struct mq_attr attr;
		static char buffer[MAX_SIZE + 1];
};
