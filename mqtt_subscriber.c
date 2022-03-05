#include <stdio.h>
#include <string.h>
#include "mosquitto.h"

#define FDEBUG
enum STATUS {SUCCESS = 0, FAILURE = 1};

/**************************************************************************/

#define CHECK_STATUS(_status)                   \
  if ((_status) != SUCCESS)                     \
    {                                           \
      printf ("Status error.\n");               \
      return 1;                                 \
    }

/**************************************************************************/

static enum STATUS initialise ()
{
  int status;

  status = mosquitto_lib_init ();
  if (status != MOSQ_ERR_SUCCESS)
    return FAILURE;

  return SUCCESS;
}
