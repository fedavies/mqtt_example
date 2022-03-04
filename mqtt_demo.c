#include <stdio.h>
#include <string.h>
#include "mosquitto.h"
#define TRUE 1
#define FALSE 0

static enum STATUS{SUCCESS, FAILURE};

/**************************************************************************/

#define CHECK_STATUS(_status) \
  if ((_status) != SUCCESS) \
  { \
    printf ("Status error.\n"); \
    return 1; \
  }

/**************************************************************************/

static void fn_on_message (struct mosquitto *mosq,
                           void *obj,
                           const struct mosquitto_message *message)
{

  printf ("Recieved message '%s' on topic '%s'.\n",
          (char *) message -> payload,
          message -> topic);
}

/**************************************************************************/

static void fn_on_connect (struct mosquitto *mosq, void *obj, int result)
{
  printf ("Connected to MQTT broker");
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

/**************************************************************************/

static enum STATUS publish_message (const char *host,
                               const int   port,
                               const char *message,
                               const char *topic,
                               const int   qos)
{
  struct mosquitto *publisher;
  int status;

  if (!(publisher = mosquitto_new (NULL, TRUE, NULL)))
  {
    fprintf (stderr, "Out of memory!\n");
    return FAILURE;
  }

  mosquitto_connect_callback_set (publisher, fn_on_connect);

  status = mosquitto_connect (publisher, host, port, 60);
  if (status != MOSQ_ERR_SUCCESS)
    return FAILURE;


  status = mosquitto_publish (publisher, NULL, topic, strlen (message),
                              message, qos, true);
  if (status != MOSQ_ERR_SUCCESS)
    return FAILURE;


  status = mosquitto_loop (publisher , -1, 1);
  if (status != MOSQ_ERR_SUCCESS)
    return FAILURE;


  mosquitto_destroy (publisher);

  return SUCCESS;
}


/**************************************************************************/

static void clean_up ()
{
  mosquitto_lib_cleanup ();
}

/**************************************************************************/

int main (int argc, char *argv[])
{
  enum STATUS status;
  char *host = "127.0.0.1";
  int port = 1883;
  int qos = 1;
  const char *message = "A message from beyond";
  const char *topic = "house/sensor";


  status = initialise ();
  CHECK_STATUS (status);

  status = publish_message (host, port, message, topic, qos);
  CHECK_STATUS (status);

  clean_up ();

  return 0;
}
