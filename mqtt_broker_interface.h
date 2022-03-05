#include "mosquitto.h"

enum STATUS {SUCCESS = 0, FAILURE = 1};

#define CHECK_STATUS(_status)                   \
  if ((_status) != SUCCESS)                     \
    {                                           \
      printf ("Status error.\n");               \
      return 1;                                 \
    }

/* Function declarations */

enum STATUS initialise ();
void clean_up ();
enum STATUS publish_message (const char *host,
                             const int   port,
                             char *message,
                             const char *topic,
                             const int   qos);

enum STATUS subscribe_and_read (const char *host,
                                const int   port,
                                const char *topic,
                                const int   qos,
                                void (* fn_read_message) (int, char *));
