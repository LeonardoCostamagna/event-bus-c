# Static Pub/Sub Event Bus Module

## Overview

The **Static Pub/Sub Event Bus** is a lightweight, high-performance publish-subscribe dispatcher module designed for deterministic, decoupled inter-module communication in embedded systems and real-time firmware applications. Traditional tightly coupled function calls can lead to tangled dependencies and rigid architectures; the event bus module overcomes these limitations by utilizing a static pre-allocated subscription buffer paired with broadcast publishing. Written in pure C with zero external runtime dependencies and no dynamic memory allocation, it provides safe multi-subscriber event routing, optional payload data attachments, and user context integration, making it ideal for scalable, event-driven embedded firmware design.

### Key Features

* **Static Pub/Sub Architecture:** Manages event subscriptions via pre-allocated static memory buffers, guaranteeing deterministic behavior with zero heap usage.
* **Multi-Subscriber Broadcasting:** Allows multiple independent listeners to subscribe to the same event identifier, automatically notifying all matching callbacks upon publication.
* **Optional Payload Data:** Supports passing arbitrary event payload pointers alongside subscriber-specific user context data during event dispatch.
* **Quality & Verification:** Verified using Ceedling/Unity complemented by Doxygen-compatible inline documentation.

---

## Event Pub/Sub & Broadcast Flow

The following structural flow demonstrates the subscription registration and broadcast publishing pipeline of the event bus module, ensuring decoupled, safe, and deterministic event routing across system components:

---

## Project Structure

```text
event_bus/
├── docs/                   # Doxygen configuration
├── event_bus.h             # Public API
├── event_bus.c             # Event bus dispatcher implementation
├── test/                   # Unit testing environment
│   ├── support/            # Auxiliary headers and Ceedling mocks
│   ├── project.yml         # Ceedling project configuration
│   └── test_event_bus.c    # Unit test suite for event bus logic
└── README.md               # Main project documentation

```

---

## Integration and Getting Started

### Integration Scheme

Integration consists of defining your event identifiers, allocating a static subscription buffer array, initializing the event bus via `event_bus_init()`, registering listeners using `event_bus_subscribe()`, and broadcasting events across the system via `event_bus_publish()`.

---

### Integration Steps

1. Copy `event_bus.h` and `event_bus.c` into your project's source tree.
2. Include `"event_bus.h"` in your application or communication module.
3. Define your event enumeration and allocate a static `event_subscription_t` backing array.
4. Call `event_bus_init()` passing your event bus instance pointer, subscription buffer, and buffer capacity.
5. Register subscribers using `event_bus_subscribe()` and trigger event broadcasts via `event_bus_publish(&bus, event, payload)`.

---

### Code Examples

#### Example Usage

The following example demonstrates how to initialize the event bus, register multiple subscribers, and publish events with optional payload data.

```c
/**
 * @file main.c
 * @brief Minimal usage example for the Event Bus module.
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <stdint.h>
#include "event_bus.h"

// Define system events
typedef enum {
    EVENT_SENSOR_DATA_READY = 0,
    EVENT_SYSTEM_FAULT
} system_event_t;

// Subscriber callback for sensor data
static void on_sensor_data(event_t event, const void *data, void *user_data) {
    if (event == EVENT_SENSOR_DATA_READY && data != NULL) {
        float temperature = *(const float *)data;
        int sensor_id = *(int *)user_data;
        printf("[Sensor Sub] Received temperature: %.2f °C (Sensor ID: %d)\n", temperature, sensor_id);
    }
}

// Subscriber callback for fault monitoring
static void on_fault_monitor(event_t event, const void *data, void *user_data) {
    (void)data;
    (void)user_data;
    if (event == EVENT_SYSTEM_FAULT) {
        printf("[Fault Sub] Critical system fault detected! Executing safety handler.\n");
    }
}

int main(void) {
    event_bus_t my_bus;
    
    // Allocate static subscription buffer (max 5 subscriptions)
    event_subscription_t subs_buffer[5];
    int sensor_context = 42;

    // 1. Initialize the event bus instance
    if (event_bus_init(&my_bus, subs_buffer, 5) != 0) {
        printf("Event Bus Initialization failed!\n");
        return 1;
    }

    printf("Event Bus initialized successfully.\n\n");

    // 2. Register subscriptions statically
    event_bus_subscribe(&my_bus, EVENT_SENSOR_DATA_READY, on_sensor_data, &sensor_context);
    event_bus_subscribe(&my_bus, EVENT_SYSTEM_FAULT, on_fault_monitor, NULL);

    // 3. Publish an event with payload data
    float current_temp = 25.45f;
    printf("Publishing EVENT_SENSOR_DATA_READY...\n");
    event_bus_publish(&my_bus, EVENT_SENSOR_DATA_READY, &current_temp);

    printf("\n");

    // 4. Publish a fault event
    printf("Publishing EVENT_SYSTEM_FAULT...\n");
    event_bus_publish(&my_bus, EVENT_SYSTEM_FAULT, NULL);

    return 0;
}

```

---

## Running Unit Tests

The development environment uses **Ceedling** (along with **Unity** and **CMock**) for hardware-decoupled unit testing.

### Prerequisites

* **Ruby** (for executing the Ceedling engine)
* **Ceedling** (unit testing framework for C)
* **gcovr** (optional, for coverage reporting)

### Execution Commands

1. Navigate to the test directory:

```bash
cd test

```

2. Run the full unit test suite:

```bash
ceedling test:all

```

3. Generate code coverage report:

```bash
ceedling gcov:all

```

The resulting HTML report will be available at:
`test/build/artifacts/gcov/gcovr/index.html`

---

## Generating Documentation

The public API is fully documented using **Doxygen**-compatible comment blocks.

### Option 1: Via Command Line

1. Navigate to the `docs/` directory:

```bash
cd docs

```

2. Build the documentation using the `doxyfile` configuration file:

```bash
doxygen doxyfile

```

3. Open the generated file in your web browser:

```bash
xdg-open html/index.html

```

### Option 2: Via Graphical User Interface

1. Launch Doxywizard from the terminal:

```bash
doxywizard

```

2. Select **File > Open** and open the `doxyfile` inside the `docs/` directory.
3. Select the **Run** tab -> Click **Run doxygen**.
4. Open `html/index.html` in your browser.

---

## License

This project is distributed under the **MIT** License.