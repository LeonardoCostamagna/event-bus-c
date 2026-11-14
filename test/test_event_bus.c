/**
 * @file test_event_bus.c
 * @brief Unit tests for the Static Pub/Sub Event Bus Module using Ceedling and Unity.
 */

#include "unity.h"
#include "event_bus.h"
#include <string.h>

static event_bus_t bus;
#define TEST_BUFFER_CAPACITY 4U
static event_subscription_t subs_buffer[TEST_BUFFER_CAPACITY];

/* Auxiliary variables to verify callbacks, payloads, and user contexts */
static int callback_called_count = 0;
static event_t last_event_seen = 0;
static const void *last_data_seen = NULL;
static void *last_user_data_seen = NULL;

/* Sample event callback */
static void sample_event_callback(event_t event, const void *data, void *user_data) {
    callback_called_count++;
    last_event_seen = event;
    last_data_seen = data;
    last_user_data_seen = user_data;
}

void setUp(void) {
    callback_called_count = 0;
    last_event_seen = 0;
    last_data_seen = NULL;
    last_user_data_seen = NULL;
    memset(subs_buffer, 0, sizeof(subs_buffer));
}

void tearDown(void) {
}

/* ============================================================================
   1. DEFENSIVE PROGRAMMING & BOUNDARY TESTS (NULL Pointers & Invalid Inputs)
   ============================================================================ */

void test_event_bus_null_parameters_and_invalid_inputs(void) {
    /* Defensive checks for event_bus_init */
    TEST_ASSERT_EQUAL_INT32(-1, event_bus_init(NULL, subs_buffer, TEST_BUFFER_CAPACITY));
    TEST_ASSERT_EQUAL_INT32(-1, event_bus_init(&bus, NULL, TEST_BUFFER_CAPACITY));
    TEST_ASSERT_EQUAL_INT32(-1, event_bus_init(&bus, subs_buffer, 0)); /* Zero capacity is invalid */

    /* Correct initialization for subsequent tests */
    TEST_ASSERT_EQUAL_INT32(0, event_bus_init(&bus, subs_buffer, TEST_BUFFER_CAPACITY));

    /* Defensive checks for event_bus_subscribe */
    TEST_ASSERT_EQUAL_INT32(-1, event_bus_subscribe(NULL, 1, sample_event_callback, NULL));
    
    event_bus_t bad_bus = {0}; /* subscriptions buffer is NULL */
    TEST_ASSERT_EQUAL_INT32(-1, event_bus_subscribe(&bad_bus, 1, sample_event_callback, NULL));
    
    TEST_ASSERT_EQUAL_INT32(-1, event_bus_subscribe(&bus, 1, NULL, NULL)); /* Null callback */

    /* Defensive checks for event_bus_publish */
    TEST_ASSERT_EQUAL_INT32(-1, event_bus_publish(NULL, 1, NULL));
    TEST_ASSERT_EQUAL_INT32(-1, event_bus_publish(&bad_bus, 1, NULL));
}

/* ============================================================================
   2. INITIALIZATION & CONFIGURATION
   ============================================================================ */

void test_event_bus_initialization_and_configuration(void) {
    int32_t result = event_bus_init(&bus, subs_buffer, TEST_BUFFER_CAPACITY);

    TEST_ASSERT_EQUAL_INT32(0, result);
    TEST_ASSERT_EQUAL_PTR(subs_buffer, bus.subscriptions);
    TEST_ASSERT_EQUAL_INT(TEST_BUFFER_CAPACITY, bus.capacity);
    TEST_ASSERT_EQUAL_INT(0, bus.count);
}

/* ============================================================================
   3. SUBSCRIPTION REGISTRATION & CAPACITY LIMITS
   ============================================================================ */

void test_event_bus_subscription_and_capacity(void) {
    int dummy_ctx1 = 11;
    int dummy_ctx2 = 22;
    int dummy_ctx3 = 33;
    int dummy_ctx4 = 44;
    int dummy_ctx5 = 55;

    event_bus_init(&bus, subs_buffer, 4); /* Small capacity of 4 for testing limits */

    /* Register up to capacity (4 items) -> Must succeed (0) */
    TEST_ASSERT_EQUAL_INT32(0, event_bus_subscribe(&bus, 1, sample_event_callback, &dummy_ctx1));
    TEST_ASSERT_EQUAL_INT32(0, event_bus_subscribe(&bus, 1, sample_event_callback, &dummy_ctx2));
    TEST_ASSERT_EQUAL_INT32(0, event_bus_subscribe(&bus, 2, sample_event_callback, &dummy_ctx3));
    TEST_ASSERT_EQUAL_INT32(0, event_bus_subscribe(&bus, 3, sample_event_callback, &dummy_ctx4));
    TEST_ASSERT_EQUAL_INT(4, bus.count);

    /* Attempting to register when capacity is full -> Must return -2 */
    TEST_ASSERT_EQUAL_INT32(-2, event_bus_subscribe(&bus, 4, sample_event_callback, &dummy_ctx5));
    TEST_ASSERT_EQUAL_INT(4, bus.count);
}

/* ============================================================================
   4. EVENT PUBLICATION & BROADCASTING
   ============================================================================ */

void test_event_bus_publish_and_broadcast(void) {
    int ctx_a = 100;
    int ctx_b = 200;
    int payload_val = 999;

    event_bus_init(&bus, subs_buffer, TEST_BUFFER_CAPACITY);

    /* Register two listeners for event 10, and one listener for event 20 */
    event_bus_subscribe(&bus, 10, sample_event_callback, &ctx_a);
    event_bus_subscribe(&bus, 10, sample_event_callback, &ctx_b);
    event_bus_subscribe(&bus, 20, sample_event_callback, &ctx_a);

    /* Publish event 10 with payload -> Both listeners for event 10 must be triggered */
    int32_t result = event_bus_publish(&bus, 10, &payload_val);

    TEST_ASSERT_EQUAL_INT32(0, result);
    TEST_ASSERT_EQUAL_INT(2, callback_called_count);
    TEST_ASSERT_EQUAL_UINT8(10, last_event_seen);
    TEST_ASSERT_EQUAL_PTR(&payload_val, last_data_seen);
    TEST_ASSERT_EQUAL_PTR(&ctx_b, last_user_data_seen); /* Last executed callback context */

    /* Reset counter and publish event 20 with NULL payload -> Only 1 listener triggered */
    callback_called_count = 0;
    result = event_bus_publish(&bus, 20, NULL);

    TEST_ASSERT_EQUAL_INT32(0, result);
    TEST_ASSERT_EQUAL_INT(1, callback_called_count);
    TEST_ASSERT_EQUAL_UINT8(20, last_event_seen);
    TEST_ASSERT_NULL(last_data_seen);
    TEST_ASSERT_EQUAL_PTR(&ctx_a, last_user_data_seen);

    /* Publish unregistered event 30 -> Zero callbacks invoked */
    callback_called_count = 0;
    result = event_bus_publish(&bus, 30, NULL);

    TEST_ASSERT_EQUAL_INT32(0, result);
    TEST_ASSERT_EQUAL_INT(0, callback_called_count);
}