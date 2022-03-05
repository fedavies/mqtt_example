#include <stdio.h>
#include <string.h>
#include "mqtt_broker_interface.h"
#include "mosquitto.h"

#define FDEBUG

/************************************************************************/

void fn_read_message (int length, char *message)
{
  /* JSON format */
  char time[9];
  char temp[2];
  int i;
  int j;

  /* Get to the start of the information */
  i = 0;
  while (true)
  {
    if (message[i] == '\"')
      break;
    i++;
  }
  /* Step ahead of the " */
  i++;

  /* Read out the time */
  j = 0;
  while (true)
  {
    memset (&time[j], message[i], 1);
    i++;
    j++;
    if (message[i] == '\"')
      break;
  }

  printf ("Gets here\n");
  memset (&time[j+1], '\0', 1);

  /* Get to the temperature */
  while (true)
  {
    if ((message[i] == '\"') ||
        (message[i] == ':') ||
        (message[i] == ' '))
      i++;
    else
      break;
  }

  /* Read out the temp */
  j = 0;
  while (true)
    {
      memset (&temp[j], message[i], 1);
      i++;
      j++;
      if (message[i] == '\"')
        break;
    }


  printf ("|                     |\n");
  printf ("| ");
  printf ("%s", time);
  printf ("   | ");
  printf ("%s", temp);
  printf ("           |\n");
}

/************************************************************************/

int main (int argc, char *argv[])
{
  enum STATUS status;
  char *host = "127.0.0.1";
  int port = 1883;
  int qos = 1;
  const char *topic = "spaceship/polarity_inverter"; /* For example */

  status = initialise ();
  CHECK_STATUS (status);


  printf ("|___TIME___|__TEMP/K__|");
  status = subscribe_and_read (host, port, topic, qos, fn_read_message);
  CHECK_STATUS (status);

  clean_up ();

  return 0;
}
