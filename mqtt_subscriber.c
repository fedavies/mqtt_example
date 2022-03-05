#include <stdio.h>
#include <string.h>
#include "mqtt_broker_interface.h"
#include "mosquitto.h"

#define FDEBUG

/************************************************************************/

void fn_read_message (int length, char *message)
{
  printf ("We caught something!\n");
}

/************************************************************************/

static void print_table (char *message)
{
  printf ("PRINTING TABLE\n");
}
/************************************************************************/

int main (int argc, char *argv[])
{
  enum STATUS status;
  char *host = "127.0.0.1";
  int port = 1883;
  int qos = 1;
  const char *topic = "spaceship/polarity_inverter"; /* For example */
  char *message;

  status = initialise ();
  CHECK_STATUS (status);


  status = subscribe_and_read (host, port, topic, qos, fn_read_message);
  CHECK_STATUS (status);

  print_table (message);

  clean_up ();

  return 0;
}
