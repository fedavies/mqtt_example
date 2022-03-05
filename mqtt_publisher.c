#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "mqtt_broker_interface.h"

#define FDEBUG

/**************************************************************************/

/* Saves getting into complicated memory clean-up, if slightly inelegant. */
static char *_numbers[100]=
  { "00", "01", "02", "03", "04", "05", "06", "07", "08", "09",
    "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
    "20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
    "30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
    "40", "41", "42", "43", "44", "45", "46", "47", "48", "49",
    "50", "51", "52", "53", "54", "55", "56", "57", "58", "59",
    "60", "61", "62", "63", "64", "65", "66", "67", "68", "69",
    "70", "71", "72", "73", "74", "75", "76", "77", "78", "79",
    "80", "81", "82", "83", "84", "85", "86", "87", "88", "89",
    "90", "91", "92", "93", "94", "95", "96", "97", "98", "99",
  };

/**************************************************************************/

static char *itoa_2 (int a)
{
#ifdef ASSERT
  ASSERT (a >= 0);
  ASSERT (a <= 99);
#endif

  return _numbers[a];
}
/**************************************************************************/

static char *convert_message_to_JSON (char *time, int temp)
{
  char *json;

  json = malloc (20);
  if (!json)
    return NULL;

  memset (&json[ 0], '{', 1);
  memset (&json[ 1], '\n', 1);

  memset (&json[ 2], '\"', 1);
  memset (&json[ 3], time[0], 1);
  memset (&json[ 4], time[1], 1);
  memset (&json[ 5], ':', 1);
  memset (&json[ 6], time[2], 1);
  memset (&json[ 7], time[3], 1);
  memset (&json[ 8], ':', 1);
  memset (&json[ 9], time[4], 1);
  memset (&json[10], time[5], 1);
  memset (&json[11], '\"', 1);

  memset (&json[12], ' ', 1);
  memset (&json[13], ':', 1);
  memset (&json[14], ' ', 1);

  memcpy (&json[15], itoa_2 (temp), 2);

  memset (&json[17], '\n', 1);
  memset (&json[18], '}', 1);

  memset (&json[19], '\0', 1);

#ifdef FDEBUG
  printf ("%s\n", json);
#endif

  return json;

}

/**************************************************************************/

static char *create_message ()
{
  time_t rawtime;
  struct tm *timeinfo;
  char clock_time[6];
  int temp;
  char *message;

  time (&rawtime);
  timeinfo = localtime (&rawtime);

  /* Create a random temperature that is at least reasonable for a polarity inverter */
  temp = rand () % 30 + 5;

  memcpy (&clock_time[0], itoa_2 (timeinfo -> tm_hour), 2);
  memcpy (&clock_time[2], itoa_2 (timeinfo -> tm_min), 2);
  memcpy (&clock_time[4], itoa_2 (timeinfo -> tm_sec), 2);
  message = convert_message_to_JSON (clock_time, temp);
  if (!message)
    return NULL;

  return message;
}

/**************************************************************************/

static void destroy_message (char *message)
{
  free (message);
}

/**************************************************************************/

int main (int argc, char *argv[])
{
  enum STATUS status;
  char *host = "127.0.0.1";
  int port = 1883;
  int qos = 1;
  const char *topic = "spaceship/polarity_inverter"; /* For example */
  char *message;
  int i;

  status = initialise ();
  CHECK_STATUS (status);

  for (i = 0; i < 2000; ++i)
  {
    message = create_message ();
    status = publish_message (host, port, message, topic, qos);
    CHECK_STATUS (status);
    destroy_message (message);
    sleep (1);
  }

  clean_up ();

  return 0;
}
