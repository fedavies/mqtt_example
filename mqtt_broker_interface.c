#include "mqtt_broker_interface.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>

enum STATUS initialise ()
{
  int status;

  status = mosquitto_lib_init ();
  if (status != MOSQ_ERR_SUCCESS)
    return FAILURE;

  return SUCCESS;
}

/**************************************************************************/

void clean_up ()
{
  mosquitto_lib_cleanup ();
}

/**************************************************************************/

enum STATUS publish_message (const char *host,
                             const int   port,
                             char *message,
                             const char *topic,
                             const int   qos)
{
  struct mosquitto *publisher;
  int status;

  if (!(publisher = mosquitto_new (NULL, true, NULL)))
    {
      fprintf (stderr, "Out of memory!\n");
      return FAILURE;
    }

  status = mosquitto_connect (publisher, host, port, 60);
  if (status != MOSQ_ERR_SUCCESS)
    return FAILURE;


  status = mosquitto_publish (publisher, NULL, topic,
                              strlen((char *)message),
                              message, qos, true);
  if (status != MOSQ_ERR_SUCCESS)
    return FAILURE;



  mosquitto_destroy (publisher);

  return SUCCESS;
}

/**************************************************************************/
typedef struct {
  void (*fn_read_message)(int, char *);
} UserData;

/**************************************************************************/

void fn_message_callback (struct mosquitto *mosq,
                          void *obj,
                          const struct mosquitto_message *mosq_message)
{
  UserData *data;


  data = obj;
  (data -> fn_read_message)(mosq_message -> payloadlen,
                            mosq_message -> payload);
}

/**************************************************************************/


enum STATUS subscribe_and_read (const char *host,
                                const int   port,
                                const char *topic,
                                const int   qos,
                                void (* fn_read_message) (int, char *))
{
  struct mosquitto *subscriber;
  int status;
  char client_id[3];
  UserData data;

  client_id[0] = 'f';
  client_id[1] = 'e';
  client_id[2] = 'd';

  data.fn_read_message = fn_read_message;
  if (!(subscriber = mosquitto_new (client_id, true, &data)))
  {
    fprintf (stderr, "Out of memory!\n");
    return FAILURE;
  }

  mosquitto_message_callback_set (subscriber, fn_message_callback);

  status = mosquitto_connect (subscriber, host, port, 60);
  if (status != MOSQ_ERR_SUCCESS)
    return FAILURE;


  status = mosquitto_subscribe (subscriber, NULL, topic, qos);
  if (status != MOSQ_ERR_SUCCESS)
    return FAILURE;


  while (true)
  {
    status = mosquitto_loop_forever (subscriber, -1, 1);
    if (status != MOSQ_ERR_SUCCESS)
    {
      printf ("Failed to connect\n");
      sleep (1);
      mosquitto_reconnect (subscriber);
    }
  }

  mosquitto_destroy (subscriber);

  return SUCCESS;
}
