/**
  * @file    event_bus.h
  * @author  Leonardo Costamagna
  * @brief   Header file containing API declarations, data types,
  *          and context structures for the Static Pub/Sub Event Bus module.
  */

#ifndef EVENT_BUS_H
#define EVENT_BUS_H

/* Ensures C linkage compatibility when compiled with C++, preventing name mangling. */
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
  * @defgroup Event_Bus_Driver Event Bus Module
  * @{
  */

/**
  * @defgroup Event_Bus_Types Data Types & Forward Declarations
  * @brief Core types, pointers, and structures for static event subscription and management.
  * @{
  */

/** Forward declaration of the Event Bus instance structure */
typedef struct event_bus_t event_bus_t;

/** @brief Type definition for event identifiers */
typedef uint8_t event_t;

/**
  * @brief Function pointer type for event subscriber callbacks.
  * @param event     The event that was published.
  * @param data      Optional pointer to event payload data (can be NULL).
  * @param user_data Pointer to subscriber-specific context data.
  */
typedef void (*event_bus_callback_t)(event_t event, const void *data, void *user_data);

/**
  * @brief Subscription mapping structure linking an event to a callback and context.
  */
typedef struct
{
  event_t               event;     /**< Event of interest */
  event_bus_callback_t  callback;  /**< Callback function to trigger */
  void                 *user_data; /**< Application-specific subscriber context */
} event_subscription_t;

/**
  * @brief Main Event Bus instance context structure.
  */
struct event_bus_t
{
  event_subscription_t *subscriptions; /**< Pointer to the backing subscription storage array */
  size_t                capacity;      /**< Maximum number of subscriptions supported */
  size_t                count;         /**< Current number of active subscriptions registered */
};

/**
  * @} Event_Bus_Types
  */

/**
  * @defgroup Event_Bus_Core_API High-Level Application Interface
  * @brief Primary API function declarations for event bus initialization, subscription, and dispatching.
  * @{
  */

/**
  * @brief  Initializes the Event Bus instance with a pre-allocated static subscription buffer.
  * @param  bus                  Pointer to the Event Bus instance.
  * @param  subscriptions_buffer Pointer to pre-allocated memory for storing subscriptions.
  * @param  capacity             Maximum total capacity of the subscriptions buffer.
  * @return Execution status (0: Success, <0: Invalid arguments).
  */
int32_t event_bus_init(event_bus_t *bus,
                       event_subscription_t *subscriptions_buffer,
                       size_t capacity);

/**
  * @brief  Registers a new event subscription statically at boot time.
  * @param  bus       Pointer to the Event Bus instance.
  * @param  event     Event identifier to listen for.
  * @param  callback  Subscriber callback function pointer.
  * @param  user_data Pointer to subscriber context data (can be NULL).
  * @return Execution status (0: Success, <0: Invalid parameters or capacity reached).
  */
int32_t event_bus_subscribe(event_bus_t *bus,
                            event_t event,
                            event_bus_callback_t callback,
                            void *user_data);

/**
  * @brief  Publishes an event to the bus, dispatching it to all matching subscribers.
  * @param  bus   Pointer to the Event Bus instance.
  * @param  event Event identifier being published.
  * @param  data  Optional pointer to event payload data (can be NULL).
  * @return Execution status (0: Success, <0: Invalid parameters).
  */
int32_t event_bus_publish(event_bus_t *bus,
                          event_t event,
                          const void *data);

/**
  * @} Event_Bus_Core_API
  */

/**
  * @} Event_Bus_Driver
  */

#ifdef __cplusplus
}
#endif

#endif /* EVENT_BUS_H */