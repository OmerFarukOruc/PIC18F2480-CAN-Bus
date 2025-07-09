# PIC18F2480 CAN Bus Communication Project

This project provides a robust firmware solution for CAN bus communication using the PIC18F2480 microcontroller. It is designed to be a reliable foundation for applications requiring data acquisition and control over a CAN network.

## Project Overview

The firmware is engineered to handle CAN bus messages in an efficient, interrupt-driven manner. The core functionality includes receiving CAN messages, storing the data in a structured way, and responding to requests from a master device. The design is modular, separating low-level drivers from application-level logic to ensure maintainability and ease of future development.

## Features

This firmware successfully implements the core requirements for seamless CAN bus integration:

*   **CAN Message Reception**: The system reliably receives standard CAN messages and extracts the data payload.
*   **Data Storage**: Incoming data is stored in a circular buffer, ensuring that a history of received messages is maintained.
*   **User Function Callback**: A callback mechanism is in place (`user_data_callback`) to allow for custom application-level processing of incoming data.
*   **Master-Slave Communication**: The firmware correctly identifies and responds to data requests from a designated master device.
*   **Timer-Based Events**: A timer module is integrated, providing a foundation for implementing periodic, time-based actions.

## How It Works

The application operates on an interrupt-driven model, which minimizes CPU overhead and ensures timely processing of events:

1.  **Initialization**: The system initializes the CAN controller, data manager, and a periodic timer.
2.  **CAN Interrupt**: Upon receiving a CAN message, a hardware interrupt is triggered. The Interrupt Service Routine (ISR) reads the message and passes it to the data manager.
3.  **Data Processing**: The data manager stores the message payload and invokes a user-defined callback function for further processing.
4.  **Master Request**: If a message is identified as a master request, the firmware retrieves the stored data and transmits it back over the CAN bus.

## Simulated Application Log

The following log demonstrates the firmware's expected behavior in a typical operational scenario.

```plaintext
=================================================
= PIC18F2480 CAN Bus Application Log (Simulated) =
=================================================

[0000.000] INFO: System Initializing...
[0000.001] INFO: Data Manager initialized. Data array cleared.
[0000.002] INFO: CAN bus driver initialized. Mode: Normal, Baudrate: 250kbps.
[0000.003] INFO: Timer initialized and started.
[0000.004] INFO: User callback registered. System entering main loop.
[0000.005] INFO: System running. Waiting for interrupts.

[0001.250] EVENT: CAN Interrupt Received (RXB0).
[0001.251] RX: ID=0x100, Len=8, Data=[0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02, 0x03, 0x04]
[0001.252] ACTION: Storing data at index 0. Timestamp: 0.
[0001.253] CALLBACK: Calling user_data_callback() with received data.

[0002.500] EVENT: CAN Interrupt Received (RXB0).
[0002.501] RX: ID=0x100, Len=4, Data=[0xAA, 0xBB, 0xCC, 0xDD]
[0002.502] ACTION: Storing data at index 1. Timestamp: 1.
[0002.503] CALLBACK: Calling user_data_callback() with received data.

[0003.100] EVENT: Timer Interrupt Received.
[0003.101] INFO: Tick count incremented.

[0004.750] EVENT: CAN Interrupt Received (RXB1).
[0004.751] RX: ID=0x200, Len=0, Data=[] (Master Request)
[0004.752] ACTION: Master request received. Preparing response.
[0004.753] TX: Responding with stored data entry 0.
[0004.754] TX: ID=0x100, Len=8, Data=[0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x02, 0x03, 0x04]
[0004.760] TX: Responding with stored data entry 1.
[0004.761] TX: ID=0x100, Len=4, Data=[0xAA, 0xBB, 0xCC, 0xDD]
[0004.765] INFO: Master response sequence complete.

[0005.100] EVENT: Timer Interrupt Received.
[0005.101] INFO: Tick count incremented.

... system continues to run ...
```

## Build Instructions

To build the project, use the provided `Makefile`.

```bash
# Build the hex file
make

# Clean build artifacts
make clean

# Flash to the microcontroller
make flash
```
