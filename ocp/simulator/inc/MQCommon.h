#define QUEUE_NAME  "/sar"
#define MAX_SIZE    4096

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: ", __func__, __LINE__); perror(#x); exit(-1); } } while (0)