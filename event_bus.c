/**
  * @file    event_bus.c
  * @author  Leonardo Costamagna
  * @brief   Static Pub/Sub Event Bus module implementation.
  */

#include "event_bus.h"
#include <string.h>

/* Core Event Bus Operations */

int32_t event_bus_init(event_bus_t *bus,
                       event_subscription_t *subscriptions_buffer,
                       size_t capacity)
{
  if ((bus == NULL) || (subscriptions_buffer == NULL) || (capacity == 0U))
  {
    return -1;
  }

  bus->subscriptions = subscriptions_buffer;
  bus->capacity      = capacity;
  bus->count         = 0U;

  /* Clear backing memory for deterministic behavior */
  memset(bus->subscriptions, 0, capacity * sizeof(event_subscription_t));

  return 0;
}

int32_t event_bus_subscribe(event_bus_t *bus,
                            event_t event,
                            event_bus_callback_t callback,
                            void *user_data)
{
  if ((bus == NULL) || (bus->subscriptions == NULL) || (callback == NULL))
  {
    return -1;
  }

  /* Check if the static subscription table has remaining capacity */
  if (bus->count >= bus->capacity)
  {
    return -2;
  }

  /* Register the subscription into the next available slot */
  bus->subscriptions[bus->count].event     = event;
  bus->subscriptions[bus->count].callback  = callback;
  bus->subscriptions[bus->count].user_data = user_data;

  bus->count++;

  return 0;
}

int32_t event_bus_publish(event_bus_t *bus,
                          event_t event,
                          const void *data)
{
  size_t i;
  const event_subscription_t *sub;

  if ((bus == NULL) || (bus->subscriptions == NULL))
  {
    return -1;
  }

  /* Iterate through registered subscriptions and broadcast to all matching listeners */
  for (i = 0U; i < bus->count; i++)
  {
    sub = &bus->subscriptions[i];

    if ((sub->event == event) && (sub->callback != NULL))
    {
      sub->callback(event, data, sub->user_data);
    }
  }

  return 0;
}