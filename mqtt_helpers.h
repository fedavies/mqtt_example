#include "mosquitto.h"

enum STATUS {SUCCESS = 0, FAILURE = 1};

#define CHECK_STATUS(_status)                   \
  if ((_status) != SUCCESS)                     \
    {                                           \
      printf ("Status error.\n");               \
      return 1;                                 \
    }

/* Function declarations */

enum STATUS initalise ();
void clean_up ();
