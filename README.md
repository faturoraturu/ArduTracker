# ArduTracker

<div align="center">

![GitHub last commit](https://img.shields.io/github/last-commit/faturoraturu/ArduTracker)
![GitHub repo size](https://img.shields.io/github/repo-size/faturoraturu/ArduTracker)
![GitHub stars](https://img.shields.io/github/stars/faturoraturu/ArduTracker?style=social)
![GitHub forks](https://img.shields.io/github/forks/faturoraturu/ArduTracker?style=social)
![License](https://img.shields.io/github/license/faturoraturu/ArduTracker)

</div>

---

## Description

**ArduTracker** is an open-source Arduino and Pixhawk-based Antenna Tracker designed for autonomous UAV tracking using MAVLink telemetry, closed-loop PID control, encoder feedback, and ArduPlane Software-In-The-Loop (SITL) simulation.

The system supports both **real UAV tracking** and **simulation-based testing**, allowing the same hardware and software architecture to be validated before flight. By integrating ArduPilot, Mission Planner, MAVProxy, and Arduino-based motor control, the tracker can automatically calculate the required azimuth and elevation angles to continuously point a directional antenna toward a moving UAV.

This project is being developed as part of the **Kontes Robot Terbang Indonesia (KRTI) 2026** competition and focuses on building a reliable, accurate, and low-cost antenna tracking platform for long-range UAV telemetry applications.

---

## Key Features

- 🎯 Closed-loop PID Position Control
- 📡 MAVLink Telemetry Communication
- 🚁 Real UAV Tracking Support
- 💻 ArduPlane SITL Simulation
- 🔄 MAVProxy Multi-Vehicle Routing
- 📍 Pixhawk ArduTracker Integration
- ⚙️ Arduino + BTS7960 Motor Controller
- 📈 Incremental Encoder Feedback
- 🛰️ Mission Planner Support
- 💾 EEPROM-based PID Calibration
- 🔧 Manual Home Position Configuration
- 📊 Easy Parameter Tuning
- 🛠️ Modular Hardware Design

---
# Hardware Components

The Antenna Tracker is built using commercially available hardware combined with custom Arduino-based motor control. The system is designed to automatically track a UAV using real-time MAVLink telemetry while maintaining precise antenna orientation through a closed-loop PID controller.

| Component | Model | Function |
|-----------|-------|----------|
| Flight Controller (UAV) | SpeedyBee F405 Wing | Generates MAVLink telemetry during real flight |
| Ground Flight Controller | Pixhawk 2.4.8 | Runs ArduPilot Antenna Tracker firmware |
| Telemetry Radio | SIYI HM30 Air & Ground Unit | Wireless MAVLink communication |
| Microcontroller | Arduino Uno R3 | Executes PID control and motor driver logic |
| Motor Driver | BTS7960 | Drives high-current DC motors |
| PAN Motor | GM25-370-CE with Encoder | Rotates antenna horizontally |
| TILT Motor | GM25-370-CE with Encoder | Rotates antenna vertically |
| Position Sensor | Incremental Encoder | Provides closed-loop position feedback |
| Ground Control Station | Mission Planner | Mission planning and telemetry monitoring |
| Simulation Platform | ArduPlane SITL | Software-in-the-loop flight simulation |
| MAVLink Router | MAVProxy | Routes telemetry to multiple devices |

---

## Hardware Architecture

```text
                     UAV
          (SpeedyBee F405 Wing)
                    │
            MAVLink Telemetry
                    │
              SIYI HM30 Air
                    │
          Wireless Communication
                    │
            SIYI HM30 Ground
                    │
             USB / UART Serial
                    │
         Pixhawk 2.4.8 Tracker
                    │
            PWM Servo Outputs
                    │
               Arduino Uno
                    │
          PID Position Controller
                    │
              BTS7960 Driver
          ┌─────────┴─────────┐
          ▼                   ▼
     PAN Motor           TILT Motor
          │                   │
      Encoder             Encoder
          └─────────┬─────────┘
                    ▼
          Directional Antenna
```

---

## Hardware Responsibilities

### SpeedyBee F405

The SpeedyBee F405 functions as the UAV flight controller during real flight operations. It collects navigation information from onboard sensors (GPS, IMU, compass, barometer, etc.) and continuously transmits MAVLink telemetry.

---

### SIYI HM30 Telemetry

The SIYI HM30 telemetry system provides long-range wireless communication between the UAV and the Ground Station.

Responsibilities include:

- Transmitting MAVLink packets
- Low-latency communication
- Long-distance operation
- Transparent serial communication

---

### Pixhawk 2.4.8 (ArduTracker)

The Pixhawk runs the official ArduPilot Antenna Tracker firmware.

Its primary tasks are:

- Receive MAVLink telemetry
- Calculate Azimuth (PAN)
- Calculate Elevation (TILT)
- Generate PWM outputs
- Maintain target tracking

---

### Arduino Uno

Instead of driving hobby servos directly, this project uses an Arduino as an intermediate motion controller.

Responsibilities include:

- Reading PWM commands from Pixhawk
- Reading encoder pulses
- Calculating actual motor position
- Executing PID control
- Driving BTS7960
- Applying mechanical limits
- Storing calibration data in EEPROM

---

### BTS7960 Motor Driver

The BTS7960 provides high-current bidirectional control for the DC gear motors.

Features:

- High current capability
- PWM speed control
- Direction control
- Motor braking support

---

### Incremental Encoder

Each GM25-370-CE motor is equipped with an incremental encoder.

Encoder feedback enables:

- Closed-loop position control
- Accurate angle estimation
- Anti-overshoot PID
- High repeatability

---

## Supported Configurations

This project supports two hardware configurations.

### Configuration A — Real UAV

```text
SpeedyBee F405
      │
      ▼
 SIYI HM30 Air
      │
 Wireless
      │
 SIYI HM30 Ground
      │
 Pixhawk Tracker
      │
 Arduino
      │
 BTS7960
      │
 PAN/TILT Motors
```

---

### Configuration B — Development (SITL)

```text
ArduPlane SITL
      │
 MAVProxy
      │
 ├──────────────► Mission Planner
 │
 └──────────────► Pixhawk Tracker
                       │
                    Arduino
                       │
                    BTS7960
                       │
                 PAN/TILT Motors
```

The software architecture remains identical in both configurations, allowing algorithms to be validated in simulation before deployment on a real UAV.

---
# Software Stack

This project combines several open-source software tools to build a complete UAV antenna tracking system. Each software component has a specific responsibility, ranging from flight simulation and MAVLink routing to firmware development and system monitoring.

| Software | Purpose |
|----------|---------|
| ArduPilot | Open-source autopilot firmware |
| ArduPlane SITL | Software-In-The-Loop UAV simulation |
| ArduPilot Antenna Tracker | Ground station tracking firmware |
| MAVProxy | MAVLink Router / Ground Control Console |
| Mission Planner | Ground Control Station (GCS) |
| Arduino IDE | Arduino firmware development |
| Visual Studio Code | Source code editor |
| Ubuntu WSL2 | Linux development environment |
| Python 3 | Required by ArduPilot tools |
| Git | Version control |
| GitHub | Repository hosting |
| usbipd-win | USB passthrough between Windows and WSL |

---

# Software Architecture

```text
                Windows 11
                     │
     ┌───────────────┴───────────────┐
     │                               │
 Mission Planner                 Visual Studio Code
     │                               │
     └───────────────┬───────────────┘
                     │
               Ubuntu WSL2
                     │
        Python Virtual Environment
                     │
               ArduPilot Source
                     │
        Tools/autotest/sim_vehicle.py
                     │
                ArduPlane SITL
                     │
                  MAVProxy
          ┌──────────┴──────────┐
          ▼                     ▼
 Mission Planner         Pixhawk Tracker
                               │
                           Arduino Uno
                               │
                            BTS7960
                               │
                        PAN / TILT Motors
```

---

# Software Responsibilities

## Ubuntu WSL2

Ubuntu WSL2 provides a Linux development environment running directly on Windows.

It is used to:

- Build ArduPilot
- Run ArduPlane SITL
- Execute MAVProxy
- Manage Python packages
- Develop and test firmware

Using WSL2 avoids the need for a dedicated Linux computer while maintaining compatibility with the official ArduPilot development environment.

---

## Python Virtual Environment

A dedicated Python Virtual Environment is used to isolate ArduPilot dependencies from the system-wide Python installation.

Advantages include:

- Clean dependency management
- Preventing package conflicts
- Easy upgrades
- Reproducible development environment

Example activation:

```bash
source venv-ardupilot/bin/activate
```

---

## ArduPilot

ArduPilot is the open-source autopilot platform used throughout this project.

It provides:

- ArduPlane firmware
- Antenna Tracker firmware
- Simulation environment
- MAVLink implementation
- Mission execution

Repository:

https://github.com/ArduPilot/ardupilot

---

## ArduPlane SITL

Software In The Loop (SITL) executes the complete ArduPlane firmware on the host computer.

Unlike a simple simulator, SITL runs the exact flight code used on real aircraft.

It simulates:

- GPS
- Compass
- IMU
- Airspeed
- RC Receiver
- Flight Dynamics
- Navigation

The simulated aircraft continuously publishes MAVLink telemetry identical to a real UAV.

Typical startup command:

```bash
Tools/autotest/sim_vehicle.py -v ArduPlane
```

---

## MAVProxy

MAVProxy is the communication hub of this project.

Responsibilities include:

- Receiving telemetry from SITL
- Routing MAVLink packets
- Connecting multiple Ground Stations
- Sending telemetry to Pixhawk Tracker
- Providing a command-line interface

Example:

```text
SITL
 │
 ▼
MAVProxy
 ├────────► Mission Planner
 └────────► Pixhawk Tracker
```

---

## Mission Planner

Mission Planner serves as the primary Ground Control Station.

Capabilities include:

- Vehicle monitoring
- Mission planning
- Parameter configuration
- Log analysis
- HUD visualization
- MAVLink Inspector
- Servo Output Monitor

Mission Planner can connect to:

- Real UAV
- ArduPlane SITL
- ArduTracker

---

## Arduino IDE

Arduino IDE is used to develop the custom motor controller firmware.

Main responsibilities:

- PID controller implementation
- Encoder processing
- PWM interpretation
- EEPROM calibration
- Serial tuning interface

---

## Visual Studio Code

Visual Studio Code is used for:

- Editing Arduino code
- Editing documentation
- Managing Git repositories
- Writing README files
- Python development

---

## usbipd-win

Since ArduPlane SITL runs inside WSL2 while Pixhawk is connected to Windows through USB, usbipd-win is used to forward the USB device into WSL.

Typical workflow:

Windows

↓

USB Device

↓

usbipd-win

↓

WSL2

↓

/dev/ttyACM0

This allows MAVProxy inside WSL to communicate directly with the Pixhawk Tracker.

---

## Git

Git provides version control for:

- Arduino firmware
- Documentation
- Configuration files
- Parameter files
- Images

---

## GitHub

GitHub hosts the complete project repository, enabling:

- Source code sharing
- Documentation
- Issue tracking
- Version history
- Collaboration

---

# Software Dependencies

The following tools are required before building or running the project.

| Dependency | Version |
|------------|---------|
| Ubuntu WSL2 | Latest |
| Python | 3.x |
| Git | Latest |
| ArduPilot | Latest Stable / Dev |
| MAVProxy | Latest |
| Mission Planner | Latest |
| Arduino IDE | 2.x |
| usbipd-win | 5.x |

---

# Development Workflow

The typical development workflow is shown below.

```text
Write Arduino Code
        │
        ▼
Upload to Arduino
        │
        ▼
Start Ubuntu WSL2
        │
        ▼
Activate Python Environment
        │
        ▼
Run ArduPlane SITL
        │
        ▼
Start MAVProxy
        │
        ▼
Connect Mission Planner
        │
        ▼
Forward MAVLink to Pixhawk Tracker
        │
        ▼
Arduino Executes PID
        │
        ▼
Motors Track UAV
```

---

# Why This Software Stack?

This software ecosystem enables the entire antenna tracking system to be developed, tested, and validated without requiring a real UAV.

By combining:

- ArduPlane SITL
- MAVProxy
- Mission Planner
- Pixhawk ArduTracker
- Arduino PID Controller

developers can safely simulate complete tracking scenarios, verify MAVLink communication, tune control algorithms, and debug the system before field deployment.

This significantly reduces development cost, minimizes hardware risk, and accelerates system integration.

---

# System Architecture

The ArduTracker system is designed to support two different telemetry sources while sharing the same tracking hardware and control algorithm.

- **Real UAV Mode** receives telemetry from an actual aircraft through a long-range telemetry radio.
- **Simulation Mode (ArduPlane SITL)** generates the same MAVLink telemetry virtually inside a computer.

Both modes ultimately send identical MAVLink messages to the Pixhawk running ArduPilot Antenna Tracker firmware. The tracker computes the desired antenna orientation and outputs PWM commands to an Arduino-based closed-loop motor controller.

Because both modes use the same MAVLink protocol and tracking logic, all software and hardware can be validated in simulation before deployment on a real UAV.

---

# Overall System Architecture

```text
                           ┌────────────────────────────────────┐
                           │           Telemetry Source         │
                           └────────────────────────────────────┘

                     REAL UAV                     SIMULATION

          ┌─────────────────────────┐      ┌─────────────────────────┐
          │ SpeedyBee F405 (Plane)  │      │    ArduPlane SITL       │
          │ GPS • IMU • Compass     │      │ Virtual Flight Model    │
          └────────────┬────────────┘      └────────────┬────────────┘
                       │                               │
                 MAVLink Telemetry               MAVLink Telemetry
                       │                               │
             SIYI HM30 Air Unit                       │
                       │                               │
             Wireless Communication                   │
                       │                               │
            SIYI HM30 Ground Unit                 MAVProxy Router
                       │                               │
                       └──────────────┬────────────────┘
                                      │
                                      ▼
                          Pixhawk 2.4.8 ArduTracker
                         (Target Angle Calculation)
                                      │
                         PWM Servo Output (PAN/TILT)
                                      │
                                      ▼
                               Arduino Uno R3
                        Closed-Loop PID Controller
                                      │
                         Incremental Encoder Feedback
                                      │
                                      ▼
                             BTS7960 Motor Driver
                                      │
                    ┌─────────────────┴─────────────────┐
                    ▼                                   ▼
               PAN Motor                         TILT Motor
                    │                                   │
                    └───────────────┬───────────────────┘
                                    ▼
                           Directional Antenna
```

---

# System Layers

The complete system can be divided into five independent layers.

## Layer 1 — Telemetry Source

This layer generates the MAVLink telemetry used for antenna tracking.

Two sources are supported:

- Real UAV (SpeedyBee F405)
- ArduPlane Software In The Loop (SITL)

Both produce identical MAVLink packets.

---

## Layer 2 — Communication Layer

This layer transports telemetry from the aircraft (or simulator) to the Ground Station.

Depending on the operating mode, communication is performed by:

- SIYI HM30 telemetry radios
- MAVProxy MAVLink Router

The communication layer is completely transparent to the tracking algorithm.

---

## Layer 3 — Target Tracking

This layer runs on the Pixhawk using the official ArduPilot Antenna Tracker firmware.

Its responsibilities include:

- Receiving MAVLink telemetry
- Tracking the target aircraft
- Computing Azimuth (PAN)
- Computing Elevation (TILT)
- Generating PWM outputs

The tracker performs only navigation calculations.

Actual motor positioning is delegated to the Arduino controller.

---

## Layer 4 — Motion Controller

The Arduino acts as an intelligent motion controller.

Its tasks include:

- Reading PWM commands
- Measuring encoder position
- Calculating angular error
- Executing PID control
- Driving the BTS7960
- Limiting mechanical travel
- Saving calibration parameters

This layer converts desired angles into accurate motor movement.

---

## Layer 5 — Mechanical System

The final layer consists of:

- PAN axis
- TILT axis
- Gear motors
- Encoder
- Directional antenna

The mechanical subsystem physically points the antenna toward the UAV.

---

# Data Flow

The following diagram illustrates the complete telemetry path from UAV to antenna movement.

```text
GPS / Virtual GPS
        │
        ▼
Flight Controller
        │
        ▼
MAVLink Telemetry
        │
        ▼
HM30 Radio / MAVProxy
        │
        ▼
Pixhawk Tracker
        │
        ▼
PWM Output
        │
        ▼
Arduino PID Controller
        │
        ▼
Encoder Feedback
        │
        ▼
BTS7960 Driver
        │
        ▼
DC Motors
        │
        ▼
Directional Antenna
```

---

# Control Flow

Unlike conventional antenna trackers that directly control hobby servos, this project separates navigation from motion control.

```text
Target Position
       │
       ▼
 Pixhawk Tracker
       │
 Desired Angle
       ▼
 Arduino
       │
 PID Controller
       │
 Motor Driver
       ▼
 Encoder
       ▲
 Position Feedback
```

This architecture provides several advantages:

- High positioning accuracy
- Stable closed-loop control
- Smooth motor movement
- Minimal overshoot
- Easy PID tuning
- Independent navigation and motion control
- Support for high-torque DC gear motors

---

# Design Philosophy

The system is intentionally modular.

Each subsystem has a single responsibility:

| Module | Responsibility |
|---------|----------------|
| ArduPlane | Generates aircraft telemetry |
| SIYI HM30 | Wireless telemetry transmission |
| MAVProxy | MAVLink routing |
| Mission Planner | Ground control and monitoring |
| Pixhawk Tracker | Target angle computation |
| Arduino | Closed-loop motion controller |
| BTS7960 | Power stage |
| Encoder | Position feedback |
| Motors | Mechanical actuation |

This modular architecture simplifies development, testing, maintenance, and future upgrades.

---
# Operation Modes

ArduTracker is designed to operate in two independent modes while maintaining the same tracking algorithm, communication protocol, and hardware architecture.

The two modes are:

1. **Real UAV Mode**
2. **Software In The Loop (SITL) Mode**

Both modes produce identical MAVLink telemetry, allowing the same Pixhawk ArduTracker firmware and Arduino motion controller to be used without modifying the control algorithm.

---

# Mode 1 — Real UAV

In this mode, the antenna tracker follows an actual UAV in flight.

The UAV continuously transmits MAVLink telemetry through a long-range radio link, allowing the ground station to determine the aircraft's position and automatically point the directional antenna toward the target.

---

## System Overview

```text
         UAV
          │
          ▼
 SpeedyBee F405 (ArduPlane)
          │
    MAVLink Telemetry
          │
     SIYI HM30 Air
          │
 Wireless Communication
          │
    SIYI HM30 Ground
          │
      USB / UART
          │
 Pixhawk ArduTracker
          │
     PWM Outputs
          │
      Arduino PID
          │
       BTS7960
          │
 PAN / TILT Motors
          │
 Directional Antenna
```

---

## Startup Sequence

The recommended startup order is:

1. Power the UAV.
2. Wait until the GPS obtains a valid fix.
3. Verify telemetry transmission through the SIYI HM30.
4. Power the Pixhawk running ArduTracker firmware.
5. Wait until Home Position is initialized.
6. Connect Mission Planner.
7. Verify that telemetry is being received.
8. Arm the tracker if required.
9. Start the UAV mission.

---

## Communication Flow

```text
GPS
 │
 ▼
SpeedyBee F405
 │
 ▼
MAVLink
 │
 ▼
HM30 Air
 │
 ▼
Wireless Link
 │
 ▼
HM30 Ground
 │
 ▼
Pixhawk Tracker
 │
 ▼
PWM
 │
 ▼
Arduino
 │
 ▼
Motors
```

---

## Requirements

- SpeedyBee F405 running ArduPlane
- SIYI HM30 telemetry system
- Pixhawk with ArduTracker firmware
- Arduino Uno
- Encoder-equipped motors
- BTS7960 motor driver
- Mission Planner

---

## Advantages

- Real GPS navigation
- Long-range tracking
- Real telemetry testing
- Complete system validation
- Suitable for outdoor operation

---

# Mode 2 — Software In The Loop (SITL)

During development, testing a complete UAV system outdoors is often impractical.

To solve this problem, ArduPilot provides **Software In The Loop (SITL)**.

SITL executes the complete ArduPlane firmware on a computer while simulating all onboard sensors.

The generated MAVLink telemetry is identical to that produced by a real aircraft.

Because the telemetry format is the same, the Pixhawk running ArduTracker cannot distinguish whether the data originates from a real UAV or a simulated aircraft.

This enables the entire antenna tracking system to be tested safely without flying an actual aircraft.

---

## System Overview

```text
Ubuntu WSL2
      │
      ▼
ArduPlane SITL
      │
 MAVLink UDP
      │
      ▼
 MAVProxy
      │
 ┌────┴─────────────┐
 ▼                  ▼
Mission Planner  Pixhawk Tracker
                     │
                 PWM Output
                     │
                  Arduino
                     │
                 BTS7960 Driver
                     │
               PAN/TILT Motors
```

---

## Development Workflow

```text
Start Ubuntu WSL2
        │
        ▼
Activate Python Environment
        │
        ▼
Run ArduPlane SITL
        │
        ▼
Start MAVProxy
        │
        ▼
Forward MAVLink
        │
        ▼
Mission Planner
        │
        ▼
Pixhawk Tracker
        │
        ▼
Arduino PID
        │
        ▼
Motor Movement
```

---

## Startup Sequence

The recommended startup order is:

1. Open Ubuntu WSL2.
2. Activate the Python virtual environment.
3. Connect the Pixhawk to Windows via USB.
4. Attach the USB device to WSL using **usbipd-win**.
5. Verify that `/dev/ttyACM0` is available.
6. Start ArduPlane SITL.
7. Start MAVProxy.
8. Forward MAVLink telemetry to the Pixhawk Tracker.
9. Open Mission Planner.
10. Connect via UDP (14550).
11. Load or create a mission.
12. Start AUTO mode.
13. Observe the antenna tracking behavior.

---

## Communication Flow

```text
ArduPlane SITL
        │
        ▼
MAVProxy
        │
 ┌──────┴─────────┐
 ▼                ▼
Mission Planner  Pixhawk Tracker
                      │
                  PWM Output
                      │
                   Arduino
                      │
               Encoder Feedback
                      │
                 BTS7960 Driver
                      │
                 Pan/Tilt Motors
```

---

## Required Software

- Ubuntu WSL2
- Python Virtual Environment
- ArduPilot
- MAVProxy
- Mission Planner
- usbipd-win
- Arduino IDE

---

## Advantages

- No real UAV required
- Safe indoor testing
- Low development cost
- Rapid debugging
- PID tuning without flight risk
- MAVLink communication validation
- Mission testing
- Parameter verification
- Repeatable experiments

---

# Real UAV vs SITL

| Feature | Real UAV | SITL |
|---------|----------|------|
| Physical Aircraft | ✅ | ❌ |
| Real GPS | ✅ | ❌ (Virtual GPS) |
| Flight Controller | Physical | Simulated |
| MAVLink Telemetry | ✅ | ✅ |
| Mission Planner | ✅ | ✅ |
| Pixhawk Tracker | ✅ | ✅ |
| Arduino PID | ✅ | ✅ |
| Motor Movement | ✅ | ✅ |
| Indoor Testing | ❌ | ✅ |
| Debugging | Limited | Excellent |
| PID Tuning | Difficult | Easy |
| Development Cost | Higher | Lower |
| Safety | Flight Risk | Safe |

---

# Recommended Development Process

The recommended workflow for this project is:

```text
Algorithm Design
        │
        ▼
Arduino Development
        │
        ▼
SITL Testing
        │
        ▼
MAVLink Validation
        │
        ▼
PID Tuning
        │
        ▼
System Integration
        │
        ▼
Real UAV Testing
        │
        ▼
Flight Validation
```

By following this sequence, software and control algorithms can be validated in simulation before deployment on real hardware, significantly reducing development time, cost, and the risk of damaging UAV equipment.

---
# Communication Flow

The ArduTracker system transforms UAV telemetry into precise antenna movement through a multi-stage communication pipeline. Regardless of whether the telemetry originates from a real UAV or ArduPlane SITL, the communication protocol remains identical because both sources use the MAVLink protocol.

The overall communication process can be divided into six stages:

1. Telemetry Generation
2. Telemetry Transmission
3. MAVLink Routing
4. Target Angle Calculation
5. PWM Command Generation
6. Closed-Loop Motor Control

---

# Communication Pipeline

```text
Telemetry Source
      │
      ▼
 MAVLink Messages
      │
      ▼
 Communication Link
      │
      ▼
 Pixhawk ArduTracker
      │
      ▼
 PWM Output
      │
      ▼
 Arduino PID Controller
      │
      ▼
 BTS7960 Motor Driver
      │
      ▼
 Pan / Tilt Motors
      │
      ▼
 Directional Antenna
```

---

# Stage 1 — Telemetry Generation

Telemetry is generated by one of two supported sources.

## Real UAV

```text
GPS
Compass
IMU
Barometer
Airspeed
RC Receiver
        │
        ▼
SpeedyBee F405
        │
        ▼
 MAVLink
```

The flight controller continuously estimates the aircraft state and broadcasts MAVLink messages such as:

- HEARTBEAT
- GLOBAL_POSITION_INT
- GPS_RAW_INT
- ATTITUDE
- VFR_HUD
- HOME_POSITION
- SYS_STATUS

---

## ArduPlane SITL

```text
Virtual GPS
Virtual IMU
Virtual Compass
Virtual Barometer
Virtual Aircraft
          │
          ▼
 ArduPlane SITL
          │
          ▼
 MAVLink
```

SITL generates the same MAVLink messages as a real aircraft, allowing the tracker to operate without modification.

---

# Stage 2 — Telemetry Transmission

### Real UAV

Telemetry is transmitted through the SIYI HM30 radio link.

```text
SpeedyBee F405
        │
        ▼
HM30 Air Unit
        │
 Wireless Link
        │
        ▼
HM30 Ground Unit
        │
 USB/UART
        ▼
Pixhawk Tracker
```

---

### SITL

Telemetry is generated locally.

```text
ArduPlane SITL
        │
 UDP
        ▼
 MAVProxy
```

No radio hardware is required.

---

# Stage 3 — MAVLink Routing

MAVProxy acts as the communication hub.

Its responsibilities include:

- Receiving MAVLink packets
- Managing multiple outputs
- Synchronizing telemetry
- Forwarding messages to multiple clients

Example configuration:

```text
                 MAVProxy
          ┌────────┴─────────┐
          ▼                  ▼
Mission Planner      Pixhawk Tracker
 UDP :14550         /dev/ttyACM0
```

This enables both Mission Planner and the Pixhawk Tracker to receive identical telemetry simultaneously.

---

# Stage 4 — Target Angle Calculation

Once telemetry reaches the Pixhawk, the ArduPilot Antenna Tracker firmware calculates the required antenna orientation.

Required inputs:

- Home Position
- UAV Latitude
- UAV Longitude
- UAV Altitude

The tracker computes:

- Bearing (Azimuth)
- Elevation Angle

Example:

```text
Ground Station

Latitude
Longitude

        │

Target Aircraft

Latitude
Longitude
Altitude

        │

        ▼

Azimuth

Elevation
```

These values represent the desired pointing direction of the antenna.

---

# Stage 5 — PWM Command Generation

After computing the target orientation, the Pixhawk converts the desired angles into PWM outputs.

Typical mapping:

| PWM | PAN Angle |
|------|-----------|
|1000 μs| -180° |
|1500 μs| 0° |
|2000 μs| +180° |

For the TILT axis:

| PWM | TILT Angle |
|------|------------|
|1000 μs| 0° |
|2000 μs| 90° |

These PWM signals represent target positions rather than direct motor commands.

---

# Stage 6 — Closed-Loop Motor Control

The Arduino interprets the incoming PWM values as desired antenna angles.

Instead of directly driving the motors, it performs closed-loop position control.

```text
PWM Target
      │
      ▼
Arduino
      │
Current Position
      ▲
Encoder Feedback
      │
PID Controller
      │
Motor Driver
      │
DC Motors
```

The encoder continuously measures the actual antenna position.

The PID controller calculates the position error:

```text
Error

=

Target Angle

−

Current Angle
```

The controller then adjusts motor speed until the error approaches zero.

---

# Closed-Loop Communication Cycle

```text
UAV Position
      │
      ▼
MAVLink
      │
      ▼
Pixhawk Tracker
      │
Target Angle
      ▼
PWM Output
      │
      ▼
Arduino
      │
Encoder
      ▲
Actual Position
      │
PID Control
      │
Motor Driver
      ▼
Motor Rotation
      │
Directional Antenna
```

This loop repeats continuously throughout the flight.

---

# MAVLink Messages Used

The tracker relies on several MAVLink messages during operation.

| Message | Purpose |
|----------|---------|
| HEARTBEAT | Vehicle status |
| GLOBAL_POSITION_INT | UAV position |
| GPS_RAW_INT | GPS information |
| HOME_POSITION | Ground station reference |
| ATTITUDE | Aircraft orientation |
| VFR_HUD | Flight information |
| SYS_STATUS | Vehicle health |
| SYSTEM_TIME | Time synchronization |

These messages are periodically transmitted and processed by the Pixhawk to maintain accurate antenna pointing.

---

# Communication Timing

The communication sequence during operation is illustrated below.

```text
ArduPlane
    │
    │ HEARTBEAT
    ▼
MAVProxy
    │
    ├────────► Mission Planner
    │
    └────────► Pixhawk Tracker
                     │
                     │ Calculate Azimuth
                     │ Calculate Elevation
                     ▼
               PWM Output
                     │
                     ▼
                  Arduino
                     │
               PID Controller
                     │
                 Encoder
                     │
                     ▼
             Pan/Tilt Motors
```

The communication loop is executed continuously throughout the mission, ensuring that the antenna remains accurately aligned with the UAV even during rapid maneuvers.

---

# Communication Summary

The communication architecture separates navigation, telemetry, and motion control into independent modules.

This modular approach provides several advantages:

- Standard MAVLink communication
- Compatibility with real UAVs and SITL
- Independent motion controller
- Real-time telemetry routing
- Easy debugging with Mission Planner
- Flexible multi-vehicle support through MAVProxy
- Scalable architecture for future extensions

---
# PID Closed-Loop Controller

Unlike conventional antenna trackers that directly drive hobby servos, this project implements a custom **closed-loop position controller** using an Arduino, incremental encoders, and high-torque DC gear motors.

The Pixhawk running ArduPilot Antenna Tracker firmware is responsible only for calculating the desired antenna orientation (PAN and TILT). The actual motor positioning is handled independently by the Arduino using a PID controller with encoder feedback.

This architecture provides higher positioning accuracy, smoother motion, better disturbance rejection, and greater flexibility for future improvements.

---

# Control Architecture

```text
                Pixhawk ArduTracker
                       │
               PWM Target Position
                       │
                       ▼
                  Arduino Uno
                       │
             Target Angle Conversion
                       │
                       ▼
               PID Controller
                       │
                Motor PWM Output
                       │
                       ▼
               BTS7960 Driver
                       │
                       ▼
             GM25-370-CE Motor
                       │
                       ▼
          Incremental Encoder Feedback
                       │
                       └──────────────┐
                                      │
                                      ▼
                           Current Position
```

---

# Closed-Loop Control Principle

The Arduino continuously compares:

- Desired Position (Target Angle)
- Actual Position (Encoder Angle)

The difference between these values is called the **position error**.

```text
Error

=

Target Angle

−

Current Angle
```

If the error is zero, the antenna is perfectly aligned with the UAV.

Otherwise, the controller adjusts the motor speed until the error approaches zero.

---

# PID Algorithm

The controller uses the classical PID equation.

```text
Output

=

P

+

I

+

D
```

where

```text
P = Kp × Error

I = Ki × Σ(Error)

D = Kd × d(Error)/dt
```

Each term contributes differently to the control response.

---

# Proportional Control (P)

The proportional term generates the primary motor command.

```text
P = Kp × Error
```

Characteristics:

- Large error → fast motor movement
- Small error → slow motor movement
- Immediate response
- Simple implementation

Increasing **Kp** results in:

- Faster tracking
- Higher responsiveness

However, excessive Kp may produce oscillation or overshoot.

---

# Integral Control (I)

The integral term accumulates small residual errors over time.

```text
I = Ki × Σ(Error)
```

Its primary purpose is to eliminate steady-state error caused by:

- Gearbox friction
- Mechanical backlash
- Motor dead zone
- External disturbances

Without the integral term, the antenna may stop slightly before reaching the desired position.

---

# Derivative Control (D)

The derivative term predicts future error by observing how quickly the antenna approaches the target.

```text
D = Kd × d(Error)/dt
```

Derivative control acts as a predictive brake.

Benefits include:

- Reduced overshoot
- Improved stability
- Faster settling time
- Smoother motion

---

# Closed-Loop Feedback

The Arduino continuously receives encoder pulses.

```text
Motor Rotation
       │
       ▼
Encoder Pulses
       │
       ▼
Current Angle
       │
       ▼
PID Controller
       │
Target Angle
       ▼
Motor Output
```

This feedback loop executes continuously during operation.

---

# Incremental Encoder

Each GM25-370-CE motor is equipped with an incremental encoder.

The encoder produces pulses proportional to shaft rotation.

The Arduino counts these pulses to estimate the motor angle.

```text
Encoder Pulses

↓

Pulse Counter

↓

Angular Position

↓

Current Angle
```

Because the controller measures the actual position instead of estimating it, positioning accuracy is significantly improved.

---

# Conditional Integration (Anti-Windup)

One of the main features of this project is the implementation of **Conditional Integration**, a practical anti-windup strategy.

In a conventional PID controller, the integral term continues accumulating error even when the motor is already operating at maximum speed.

This condition is known as **Integral Windup**.

It may cause:

- Severe overshoot
- Slow recovery
- Oscillation
- Poor tracking performance

---

## Saturation Example

```text
Requested PWM = 420

Maximum PWM = 255

Motor cannot rotate any faster.
```

Without protection, the integral term continues increasing.

After the antenna reaches the target, the accumulated integral forces the motor to continue rotating beyond the desired angle.

---

## Conditional Integration

To prevent this problem, the controller only updates the integral term when the motor output is **not saturated**.

```text
if PWM < Maximum PWM

        Integrate Error

else

        Freeze Integral
```

Advantages:

- Eliminates windup
- Faster recovery
- Reduced overshoot
- Stable control

---

# Deadband

Small encoder noise may cause the controller to repeatedly start and stop the motors.

To avoid unnecessary movement, a deadband is implemented.

```text
Error

<

0.5°

↓

Motor OFF
```

Within this region, the antenna is considered accurately aligned.

---

# Hysteresis

Hysteresis prevents rapid switching when the error fluctuates around the deadband threshold.

Example:

```text
Error = 0.4°

Motor OFF

↓

Error = 0.45°

Still OFF

↓

Error = 1.2°

Motor ON
```

This prevents motor chatter and improves mechanical lifetime.

---

# Motor Direction Logic

The sign of the position error determines the rotation direction.

```text
Error > 0

↓

Rotate Clockwise

-----------------------

Error < 0

↓

Rotate Counter-Clockwise
```

This logic is applied independently to both PAN and TILT axes.

---

# PWM Saturation

The BTS7960 driver accepts PWM values between:

```text
0

↓

255
```

The controller limits all outputs to this range.

```text
Calculated PWM

↓

Clamp

↓

0 ... 255
```

Output saturation protects the motors and prevents unstable control.

---

# Complete Control Loop

```text
Target Angle
       │
       ▼
Read PWM from Pixhawk
       │
       ▼
Read Encoder Position
       │
       ▼
Calculate Error
       │
       ▼
PID Controller
       │
       ▼
Conditional Integration
       │
       ▼
Deadband Check
       │
       ▼
Output Saturation
       │
       ▼
BTS7960 Driver
       │
       ▼
Motor Rotation
       │
       ▼
Encoder Feedback
       │
       └───────────────────────────────┘
```

The control loop repeats continuously, allowing the antenna to follow the UAV smoothly and accurately in real time.

---

# Advantages of This Controller

Compared with a standard servo-based antenna tracker, this controller provides several advantages.

| Feature | Conventional Servo | This Project |
|----------|-------------------|--------------|
| Closed-loop position control | ❌ | ✅ |
| Encoder feedback | ❌ | ✅ |
| High-torque DC motors | ❌ | ✅ |
| PID tuning | Limited | Full |
| Anti-windup | ❌ | ✅ |
| Deadband | Limited | ✅ |
| Hysteresis | Limited | ✅ |
| EEPROM parameter storage | ❌ | ✅ |
| Custom motion profile | ❌ | ✅ |
| Expandable control algorithm | ❌ | ✅ |

---

# Summary

The Arduino-based motion controller extends the capabilities of the standard ArduPilot Antenna Tracker by introducing a fully customizable closed-loop control system.

By combining PID control, encoder feedback, conditional integration, deadband, hysteresis, and EEPROM-based parameter storage, the tracker achieves accurate, smooth, and repeatable antenna positioning suitable for both real UAV operations and ArduPlane SITL simulation.

---
# 11. Mechanical Limits

The mechanical design of this Antenna Tracker intentionally limits the rotational range of both the PAN and TILT axes.

Unlike professional antenna trackers that use slip rings for unlimited rotation, this project connects all electronic components using conventional cables. Continuous 360° rotation would eventually twist the cables, increasing mechanical stress and potentially causing wire damage or failure.

To ensure long-term reliability, the tracker combines **mechanical constraints** and **software protection**. The Arduino continuously monitors the target position received from the Pixhawk and guarantees that every commanded movement remains within the safe operating range.

---

# Mechanical Range

## PAN Axis

The horizontal rotation (PAN) is limited to:

```
-180°  ←────────────── 0° ─────────────→ +180°
```

| Position | Angle | Description |
|----------|--------|-------------|
| Full Left | -180° | Mechanical stop |
| Forward | 0° | Antenna faces forward |
| Full Right | +180° | Mechanical stop |

Maximum rotation:

```
360°
(-180° → +180°)
```

The antenna is **not allowed** to rotate beyond these limits.

---

## TILT Axis

The vertical rotation (TILT) is limited to:

```
0°
│
│
│
90°
```

| Position | Angle | Description |
|----------|--------|-------------|
| Horizontal | 0° | Antenna parallel to ground |
| Vertical | 90° | Antenna pointing upward |

The antenna never rotates below the horizon or beyond the vertical position.

---

# PWM to Angle Mapping

Pixhawk ArduTracker generates standard RC servo PWM signals.

The Arduino interprets these PWM values as target angles for the motor controller.

## PAN Mapping

| PWM | Angle |
|------|-------|
| 1000 µs | -180° |
| 1500 µs | 0° |
| 2000 µs | +180° |

Linear interpolation is applied between these values.

```
1000 µs                1500 µs                2000 µs
    │----------------------│----------------------│
 -180°                     0°                  +180°
```

---

## TILT Mapping

| PWM | Angle |
|------|-------|
| 1000 µs | 0° |
| 1500 µs | 45° |
| 2000 µs | 90° |

```
1000 µs                1500 µs                2000 µs
    │----------------------│----------------------│
     0°                   45°                   90°
```

---

# Software Mechanical Protection

Although Pixhawk computes the desired pointing direction, the Arduino is responsible for ensuring that the requested position never exceeds the mechanical limits.

Before applying PID control, every target angle is validated:

```
if (PAN < -180°)
    PAN = -180°

if (PAN > +180°)
    PAN = +180°

if (TILT < 0°)
    TILT = 0°

if (TILT > 90°)
    TILT = 90°
```

This software protection prevents accidental over-rotation caused by unexpected telemetry or controller errors.

---

# Shortest-Path Rotation

One of the most important features of the tracker is its **Shortest-Path Rotation Algorithm**.

Since the system does not use a slip ring, the antenna should never perform unnecessary full rotations.

Instead, it always rotates through the shortest legal path within the allowed mechanical range.

### Example 1

Current position

```
170°
```

Target

```
175°
```

Rotation

```
+5°
```

---

### Example 2

Current position

```
170°
```

Target

```
-170°
```

Instead of rotating

```
340°
```

the controller recognizes that the mechanical limit has been reached and computes the equivalent movement through the legal path.

The software prevents continuous rotation beyond ±180°, ensuring the cables remain untwisted while still reaching the target efficiently.

---

# Mechanical Protection Strategy

The tracker uses multiple layers of protection:

- Mechanical hard-stop to physically limit rotation.
- Software angle limiting before PID execution.
- Encoder feedback to monitor actual position.
- Closed-loop PID control to stop precisely at the target.
- Shortest-path rotation to minimize cable twisting.

Together, these mechanisms provide safe and reliable operation during long-duration tracking.

---

# Why No Slip Ring?

A slip ring allows unlimited continuous rotation by transferring electrical signals through rotating contacts.

However, slip rings increase:

- System cost
- Mechanical complexity
- Maintenance requirements
- Weight
- Electrical noise

For KRTI-class antenna trackers, limiting the PAN axis to ±180° provides a simpler, lighter, and more robust solution while still covering the full operational field of view required for most UAV missions.

---

# Mechanical Layout

```
                     +90°
                       │
                       │
                       │
            -------------------------
           /                         \
      -180°           0°           +180°
           \                         /
            -------------------------

               PAN Rotation

               TILT Rotation

                 90°
                  ▲
                  │
                  │
                  │
                  │
                  │
                  │
                  ▼
                  0°
```

---

# Operational Summary

| Axis | Range | PWM Range | Protection |
|------|-------|-----------|------------|
| PAN | -180° to +180° | 1000–2000 µs | Software + Mechanical Stop |
| TILT | 0° to 90° | 1000–2000 µs | Software + Mechanical Stop |

---

# Advantages of the Mechanical Design

- Prevents cable twisting without requiring a slip ring.
- Reduces mechanical complexity and system cost.
- Protects motors and gears from excessive rotation.
- Ensures repeatable positioning through encoder feedback.
- Provides safe operation for long-duration autonomous UAV tracking.
- Fully compatible with both Real UAV and ArduPlane SITL operation modes.

---
# 12. EEPROM Calibration

To simplify field deployment and maintenance, the Arduino firmware stores all critical calibration parameters inside its internal **Electrically Erasable Programmable Read-Only Memory (EEPROM)**.

Unlike normal RAM, EEPROM retains its contents even after the system is powered off. This allows the Antenna Tracker to restore its last known configuration automatically every time it starts, eliminating the need for repeated calibration.

---

# Why EEPROM?

Every mechanical assembly is slightly different due to manufacturing tolerances, gear backlash, encoder installation, and motor alignment.

As a result, parameters such as:

- Encoder zero position
- Mechanical offset
- PID gains
- Counts-per-degree conversion
- Motor direction
- Calibration flags

must be customized for each individual tracker.

Saving these parameters in EEPROM ensures that every unit preserves its own calibration independently.

---

# Stored Parameters

The firmware stores configuration values similar to the following:

| Parameter | Description |
|-----------|-------------|
| Encoder Zero Offset | Mechanical home position |
| PAN Offset | Horizontal alignment correction |
| TILT Offset | Vertical alignment correction |
| Encoder Counts per Degree | Encoder resolution conversion |
| Kp | Proportional gain |
| Ki | Integral gain |
| Kd | Derivative gain |
| Motor Direction | Reverse/Normal rotation |
| Calibration Status | Indicates whether calibration has been completed |

---

# Startup Procedure

Each time the Arduino boots, it follows this initialization sequence:

```text
Power ON
    │
    ▼
Initialize Hardware
    │
    ▼
Read EEPROM
    │
    ▼
Load Calibration Values
    │
    ▼
Initialize PID Controller
    │
    ▼
Ready for Tracking
```

If valid calibration data exists, the tracker is immediately ready for operation.

Otherwise, default values are loaded and a calibration procedure is required.

---

# Calibration Workflow

The calibration process is performed only when necessary.

Typical workflow:

```text
Install Tracker
        │
        ▼
Move Antenna to Home Position
        │
        ▼
Measure Encoder Offset
        │
        ▼
Tune PID Parameters
        │
        ▼
Save Configuration
        │
        ▼
Write EEPROM
        │
        ▼
Power Cycle
        │
        ▼
Calibration Restored Automatically
```

---

# Serial Monitor Configuration

The firmware supports configuration through the Arduino Serial Monitor.

Typical commands include:

| Command | Description |
|----------|-------------|
| `Z` | Save current encoder position as zero |
| `P90.0` | Move to a specified angle |
| `KP=2.0,0.05,0.30` | Update PID gains |
| `SAVE` | Store parameters to EEPROM |
| `LOAD` | Reload parameters from EEPROM |
| `RESET` | Restore default configuration |

> The exact command syntax may differ depending on the firmware version.

---

# EEPROM Write Process

When a configuration parameter changes, the firmware performs the following steps:

```text
User Command
      │
      ▼
Validate Data
      │
      ▼
Update RAM Variables
      │
      ▼
Write EEPROM
      │
      ▼
Verification
      │
      ▼
Configuration Saved
```

Only validated values are written to EEPROM to avoid storing corrupted or invalid data.

---

# Automatic Loading

At startup:

```cpp
setup()
{
    EEPROM.read(...);

    loadPID();

    loadOffsets();

    loadEncoderCalibration();

    initializeController();
}
```

This process is completely automatic and requires no user intervention.

---

# Advantages of EEPROM Calibration

Using EEPROM provides several important advantages:

- Permanent storage of calibration values.
- No need to recalibrate after every power cycle.
- Faster system startup.
- Easier field deployment.
- Consistent PID performance across restarts.
- Simplified maintenance and replacement of hardware components.

---

# Practical Example

Without EEPROM:

```text
Power OFF
      │
      ▼
Calibration Lost
      │
      ▼
Reconfigure Everything
```

With EEPROM:

```text
Power OFF
      │
      ▼
Calibration Saved
      │
      ▼
Power ON
      │
      ▼
Calibration Restored Automatically
```

---

# Recommended Calibration Procedure

For best tracking performance, the following order is recommended:

1. Assemble the mechanical structure.
2. Set the antenna to its mechanical home position.
3. Calibrate the encoder zero offset.
4. Verify motor rotation direction.
5. Measure encoder counts per degree.
6. Tune the PID controller.
7. Save all parameters to EEPROM.
8. Restart the tracker and verify that all settings are restored correctly.

---

# EEPROM Memory Layout (Conceptual)

```text
+-------------------------------+
| Encoder Zero Offset           |
+-------------------------------+
| PAN Offset                    |
+-------------------------------+
| TILT Offset                   |
+-------------------------------+
| Encoder Counts / Degree       |
+-------------------------------+
| PID Kp                        |
+-------------------------------+
| PID Ki                        |
+-------------------------------+
| PID Kd                        |
+-------------------------------+
| Motor Direction               |
+-------------------------------+
| Calibration Flag              |
+-------------------------------+
```

---

# Summary

The EEPROM-based calibration system enables the Antenna Tracker to maintain its configuration permanently, ensuring reliable operation after every reboot. By storing encoder calibration, PID parameters, mechanical offsets, and motor configuration in non-volatile memory, the tracker can be deployed quickly without repeating the calibration process. This approach improves usability, reduces setup time, and provides consistent closed-loop tracking performance for both **Real UAV** and **ArduPlane SITL** operation modes.

---
# 13. Installation Guide

This section explains how to install and configure all required hardware and software to run the Antenna Tracker in both **Real UAV Mode** and **ArduPlane SITL Simulation Mode**.

---

# System Requirements

## Hardware

- Arduino Mega 2560
- Pixhawk 2.4.8
- BTS7960 Motor Driver
- 2 × GM25-370 DC Gear Motor with Encoder
- SIYI HM30 Telemetry (Real UAV Mode)
- USB Cable
- Windows 10 / Windows 11 PC

For Real UAV Mode:

- SpeedyBee F405 Flight Controller
- GPS Module
- UAV Platform

---

## Software

Install the following software before starting.

| Software | Version |
|----------|----------|
| Arduino IDE | Latest |
| Mission Planner | Latest |
| ArduPilot | Latest Stable / Dev |
| MAVProxy | Latest |
| Python | 3.10+ |
| Git | Latest |
| WSL2 Ubuntu | Ubuntu 24.04 LTS (Recommended) |
| usbipd-win | Latest |

---

# Clone Repository

Clone this repository.

```bash
git clone https://github.com/<username>/ArduTracker.git

cd ArduTracker
```

---

# Install Arduino IDE

Download Arduino IDE:

https://www.arduino.cc/en/software

Install normally.

Required libraries:

- EEPROM
- Encoder
- PID Controller Library

Upload the firmware inside

```
src/
```

to the Arduino Mega.

---

# Install Mission Planner

Download:

https://firmware.ardupilot.org/Tools/MissionPlanner/

Install normally.

Mission Planner is used to:

- configure Pixhawk
- upload firmware
- monitor telemetry
- plan missions
- visualize UAV position

---

# Install Python

Install Python 3.10 or newer.

Verify:

```bash
python --version
```

---

# Install Git

Download:

https://git-scm.com/

Verify:

```bash
git --version
```

---

# Install WSL2

Open PowerShell as Administrator.

```powershell
wsl --install
```

Restart Windows.

Verify:

```powershell
wsl --status
```

---

# Install Ubuntu

Install Ubuntu from Microsoft Store.

Recommended version:

```
Ubuntu 24.04 LTS
```

Launch Ubuntu.

Update packages:

```bash
sudo apt update

sudo apt upgrade
```

---

# Clone ArduPilot

```bash
git clone https://github.com/ArduPilot/ardupilot.git

cd ardupilot
```

---

# Install ArduPilot Dependencies

Run:

```bash
Tools/environment_install/install-prereqs-ubuntu.sh -y
```

Restart Ubuntu.

Reload environment:

```bash
. ~/.profile
```

---

# Install MAVProxy

```bash
pip install MAVProxy
```

Verify:

```bash
mavproxy.py --version
```

---

# Build ArduPlane SITL

Configure waf:

```bash
./waf configure
```

Build:

```bash
./waf plane
```

---

# Install usbipd-win

On Windows:

```powershell
winget install dorssel.usbipd-win
```

Verify:

```powershell
usbipd --version
```

---

# Share Pixhawk USB Device

Connect Pixhawk to Windows.

List USB devices.

```powershell
usbipd list
```

Share the Pixhawk.

```powershell
usbipd bind --busid <BUSID>
```

Example

```powershell
usbipd bind --busid 2-1
```

Attach to WSL.

```powershell
usbipd attach --wsl --busid 2-1
```

Verify inside Ubuntu.

```bash
ls /dev/ttyACM*
```

Expected:

```bash
/dev/ttyACM0
```

---

# Flash Pixhawk

Open Mission Planner.

Install firmware:

```
ArduPilot Antenna Tracker
```

Reconnect Pixhawk.

Configure:

- Servo Output
- HOME Position
- Tracker Parameters

---

# Upload Arduino Firmware

Open:

```
src/
```

Compile.

Upload to Arduino Mega.

Open Serial Monitor.

Verify encoder feedback.

---

# Configure Mission Planner

For SITL:

Connection:

```
UDP
```

Port:

```
14550
```

For Real UAV:

Select COM Port.

Connect normally.

---

# Configure MAVProxy

Run ArduPlane SITL.

```bash
Tools/autotest/sim_vehicle.py -v ArduPlane
```

Inside MAVProxy:

```text
output add 127.0.0.1:14550
```

Forward telemetry to Pixhawk.

```text
output add /dev/ttyACM0,115200
```

Verify:

```text
output
```

Expected:

```text
0: 127.0.0.1:14550

1: /dev/ttyACM0
```

---

# Configure Pixhawk Tracker

Recommended parameters:

| Parameter | Value |
|------------|-------|
| SYSID_TARGET | 1 |
| SERIALx_PROTOCOL | MAVLink2 |
| SERIALx_BAUD | 115200 |
| HOME Position | Manual (Lab Test) |

Servo Outputs:

```
SERVO1 → PAN

SERVO2 → TILT
```

---

# Configure Arduino

Verify:

- Encoder direction
- Motor direction
- PID gains
- EEPROM configuration
- PWM input from Pixhawk

---

# Verify Communication

Mission Planner should display:

- Virtual aircraft (SITL)
- Mission path
- GPS position

Pixhawk should receive:

- HEARTBEAT
- GLOBAL_POSITION_INT
- GPS_RAW_INT

Arduino should receive:

- PAN PWM
- TILT PWM

Encoder values should change while motors move.

---

# Directory Structure

```
ArduTracker
│
├── mechanic/
│
├── hardware/
│
├── src/
│
├── parameter/
│
├── img/
│
└── README.md
```

---

# Installation Checklist

Before operating the tracker, verify the following checklist:

| Item | Status |
|------|:------:|
| Arduino Firmware Uploaded | ☐ |
| Pixhawk Flashed with ArduTracker | ☐ |
| Mission Planner Installed | ☐ |
| WSL Ubuntu Installed | ☐ |
| ArduPilot Installed | ☐ |
| MAVProxy Installed | ☐ |
| usbipd Configured | ☐ |
| Pixhawk Detected as `/dev/ttyACM0` | ☐ |
| SITL Running | ☐ |
| Mission Planner Connected | ☐ |
| MAVProxy Outputs Configured | ☐ |
| Arduino Receives PWM | ☐ |
| Encoder Working | ☐ |
| Motors Respond Correctly | ☐ |
| EEPROM Saved | ☐ |

---

# Next Step

After completing the installation, continue to the next chapter:

**14. Running ArduPlane SITL**

This chapter explains how to launch the simulator, connect Mission Planner, forward MAVLink telemetry through MAVProxy, and interface the virtual aircraft with the physical Pixhawk Antenna Tracker.

---
# 14. Running ArduPlane SITL

This chapter explains how to launch **ArduPlane Software In The Loop (SITL)** and connect it to both **Mission Planner** and the physical **Pixhawk Antenna Tracker**.

The objective is to simulate a UAV without requiring a real aircraft while keeping the Antenna Tracker fully operational.

---

# Overview

During simulation, ArduPlane generates virtual GPS, AHRS, airspeed, and MAVLink telemetry.

MAVProxy forwards this telemetry simultaneously to:

- Mission Planner (Ground Control Station)
- Pixhawk Antenna Tracker

As a result:

- Mission Planner displays the virtual aircraft.
- Pixhawk computes PAN and TILT angles.
- Arduino receives PWM commands.
- The antenna rotates exactly as it would during a real flight.

---

# Communication Architecture

```text
                 ArduPlane SITL
                       │
                  MAVLink UDP
                       │
                   MAVProxy
             ┌─────────┴─────────┐
             │                   │
             ▼                   ▼
     Mission Planner       Pixhawk Tracker
        UDP 14550         USB /dev/ttyACM0
                                  │
                           PWM Servo Output
                                  │
                              Arduino Mega
                                  │
                     PID + Encoder Feedback
                                  │
                           BTS7960 Driver
                                  │
                          Pan / Tilt Motors
```

---

# Step 1 — Open Ubuntu (WSL2)

Open Ubuntu.

Activate your Python virtual environment.

Example:

```bash
cd ~/Projects_antena_tracker/ardupilot

source venv-ardupilot/bin/activate
```

---

# Step 2 — Start ArduPlane SITL

Launch the simulator.

```bash
Tools/autotest/sim_vehicle.py -v ArduPlane
```

Expected output:

```
Starting ArduPlane
```

After initialization:

```
MANUAL>
```

or

```
MAV>
```

depending on the selected flight mode.

---

# Step 3 — Connect Mission Planner

Open Mission Planner.

Connection Type:

```
UDP
```

Port:

```
14550
```

Click **Connect**.

Mission Planner should display:

- Artificial Horizon
- Virtual Aircraft
- GPS Fix
- HUD
- Flight Mode

---

# Step 4 — Share Pixhawk to WSL

If using Windows with WSL2, connect the Pixhawk through USB/IP.

List USB devices:

```powershell
usbipd list
```

Example:

```
BUSID 2-1
ArduPilot (COM4)
```

Attach the device:

```powershell
usbipd attach --wsl --busid 2-1
```

Expected output:

```
Using WSL distribution Ubuntu
Loading vhci_hcd module
```

---

# Step 5 — Verify Pixhawk Detection

Inside Ubuntu:

```bash
ls /dev/ttyACM*
```

Expected:

```bash
/ dev/ttyACM0
```

Check USB devices:

```bash
lsusb
```

Example:

```
Generic fmuv3
```

This confirms that Pixhawk is successfully attached to WSL.

---

# Step 6 — Forward Telemetry to Pixhawk

Inside the MAVProxy console:

```text
output add /dev/ttyACM0,115200
```

Verify:

```text
output
```

Expected:

```
0 : 127.0.0.1:14550

1 : /dev/ttyACM0
```

This means telemetry is being sent to:

- Mission Planner
- Pixhawk Tracker

at the same time.

---

# Step 7 — Verify Communication

Check MAVProxy.

```
link
```

Expected:

```
link 1 OK

0 lost

0% packet loss
```

---

# Step 8 — Verify MAVLink Messages

Inside MAVProxy:

```
watch HEARTBEAT
```

or

```
watch GPS_RAW_INT
```

You should observe messages similar to:

```
HEARTBEAT

GLOBAL_POSITION_INT

GPS_RAW_INT

HOME_POSITION

VFR_HUD

ATTITUDE
```

These messages confirm that SITL is producing valid telemetry.

---

# Step 9 — Verify Mission Planner

Mission Planner should display:

- Connected status
- GPS Fix
- Aircraft Icon
- HUD
- Map Position
- Mission Route

The aircraft icon should move while the simulated mission is running.

---

# Step 10 — Verify Pixhawk Tracker

The Pixhawk should receive MAVLink telemetry from the simulator.

Recommended checks:

- HEARTBEAT received
- GPS data updating
- HOME position available
- SYSID_TARGET correctly configured
- PWM outputs changing

If these conditions are met, the tracker is successfully following the simulated aircraft.

---

# Step 11 — Verify Arduino

Open the Arduino Serial Monitor.

Observe:

```
PAN PWM

TILT PWM

Encoder Position

Target Angle

Current Angle

PID Output
```

As the virtual aircraft moves:

- PAN PWM should change.
- TILT PWM should change.
- Encoder angle should follow the target.
- Motors should rotate smoothly.

---

# Running an AUTO Mission

Mission Planner allows fully autonomous missions.

Typical workflow:

1. Create waypoints.
2. Upload the mission.
3. Arm the virtual aircraft.
4. Switch to AUTO mode.
5. Start the mission.

The virtual aircraft will automatically fly between waypoints while the Antenna Tracker continuously updates its pointing direction.

---

# Expected Data Flow

```text
Virtual GPS
      │
      ▼
 ArduPlane SITL
      │
      ▼
 MAVLink
      │
      ▼
 MAVProxy
      │
      ├──────────────► Mission Planner
      │
      ▼
 Pixhawk Tracker
      │
      ▼
 Servo PWM
      │
      ▼
 Arduino
      │
      ▼
 PID Controller
      │
      ▼
 BTS7960
      │
      ▼
 Pan/Tilt Motors
```

---

# Troubleshooting

## Mission Planner does not connect

Check:

- UDP connection
- Port 14550
- SITL is running

---

## `/dev/ttyACM0` does not exist

Verify:

```powershell
usbipd attach --wsl --busid <BUSID>
```

Then:

```bash
ls /dev/ttyACM*
```

---

## Pixhawk receives no telemetry

Verify:

```text
output
```

Expected:

```
0 : 127.0.0.1:14550

1 : /dev/ttyACM0
```

---

## Motors do not move

Verify:

- Servo outputs are configured.
- PWM reaches the Arduino.
- Encoder feedback is working.
- PID controller is enabled.
- Arduino firmware is running.

---

## Mission Planner only shows the aircraft

This is normal.

Mission Planner connected to the SITL UDP stream displays only the simulated aircraft.

The physical Antenna Tracker is a separate MAVLink vehicle and requires a multi-vehicle MAVLink configuration if you want both the aircraft and the tracker to appear simultaneously.

---

# Verification Checklist

| Item | Expected Result |
|------|-----------------|
| SITL Running | ✅ |
| Mission Planner Connected | ✅ |
| MAVProxy Running | ✅ |
| `/dev/ttyACM0` Available | ✅ |
| Pixhawk Receives MAVLink | ✅ |
| GPS Updating | ✅ |
| HEARTBEAT Received | ✅ |
| PWM Output Active | ✅ |
| Arduino Receives PWM | ✅ |
| Encoder Feedback Active | ✅ |
| Motors Following Target | ✅ |

---

# Next Step

Continue with **Chapter 15 – MAVProxy Configuration**, where the complete multi-output routing configuration, telemetry forwarding, debugging commands, and multi-vehicle communication setup are explained in detail.

---

# 15. MAVProxy Configuration

MAVProxy is a lightweight Ground Control Station (GCS) and **MAVLink Router** used in this project to distribute telemetry generated by **ArduPlane SITL** to multiple endpoints simultaneously.

In this project, MAVProxy plays a central role by forwarding the same MAVLink telemetry stream to:

- Mission Planner (Ground Control Station)
- Pixhawk 2.4.8 running ArduPilot Antenna Tracker
- MAVProxy Console (Monitoring & Debugging)

Without MAVProxy, ArduPlane SITL can only communicate with a single endpoint. By using MAVProxy, telemetry can be shared with multiple devices while maintaining synchronization.

---

# 15.1 System Architecture

```text
                    ArduPlane SITL
                           │
                    MAVLink Telemetry
                           │
                           ▼
                     MAVProxy Router
               ┌───────────┼───────────┐
               │           │           │
               ▼           ▼           ▼
      Mission Planner   Pixhawk     MAVProxy
        UDP :14550      Tracker      Console
                       USB Serial
```

The communication architecture allows both Mission Planner and the physical Pixhawk Antenna Tracker to receive identical MAVLink telemetry from the virtual aircraft.

---

# 15.2 Starting ArduPlane SITL

Navigate to the ArduPilot directory.

```bash
cd ~/ardupilot
```

Activate the Python virtual environment.

```bash
source venv-ardupilot/bin/activate
```

Start the ArduPlane Software-In-The-Loop simulator.

```bash
Tools/autotest/sim_vehicle.py -v ArduPlane
```

If the simulator starts successfully, MAVProxy will automatically launch and display:

```text
MANUAL>
```

This prompt indicates that MAVProxy is ready to receive commands.

---

# 15.3 Default MAVProxy Output

By default, MAVProxy automatically creates one telemetry output for Mission Planner.

```text
MANUAL> output

1 outputs

0: 172.22.xx.xx:14550
```

This UDP output is used by Mission Planner to display the virtual aircraft.

---

# 15.4 Forwarding Telemetry to Pixhawk Tracker

To allow the physical Pixhawk Antenna Tracker to receive the same telemetry stream, add a serial output.

First, verify that Pixhawk is connected to WSL.

```bash
ls /dev/ttyACM*
```

Example output:

```text
/dev/ttyACM0
```

Then add the serial output.

```text
output add /dev/ttyACM0,115200
```

If successful, MAVProxy will display:

```text
Adding output /dev/ttyACM0,115200
```

Verify the output list again.

```text
output
```

Example:

```text
MANUAL> output

2 outputs

0: 172.22.xx.xx:14550

1: /dev/ttyACM0
```

This means telemetry is now transmitted simultaneously to:

- Mission Planner
- Pixhawk Antenna Tracker

---

# 15.5 Verifying Communication

Use the following command.

```text
link
```

Example output:

```text
MANUAL> link

link 1 OK

160470 packets

0 lost

0.0% loss
```

Explanation:

- **OK** → Communication established successfully.
- **Packets** → Number of MAVLink packets transmitted.
- **0 lost** → No packets were lost.
- **0.0% loss** → Excellent communication quality.

---

# 15.6 Frequently Used MAVProxy Commands

### Show all telemetry outputs

```text
output
```

---

### Add a new serial output

```text
output add /dev/ttyACM0,115200
```

---

### Remove an output

```text
output remove 1
```

---

### Display communication status

```text
link
```

---

### Show current flight mode

```text
mode
```

---

### Change flight mode

```text
mode AUTO
```

---

### Arm the aircraft

```text
arm throttle
```

---

### Disarm the aircraft

```text
disarm
```

---

### List mission waypoints

```text
wp list
```

---

### Load a mission

```text
wp load mission.txt
```

---

# 15.7 MAVLink Data Flow

The telemetry forwarding process is illustrated below.

```text
              ArduPlane SITL
                     │
             Virtual Sensors
                     │
       GPS • IMU • Compass • AHRS
                     │
             MAVLink Telemetry
                     │
                 MAVProxy
          ┌──────────┴──────────┐
          │                     │
          ▼                     ▼
 Mission Planner         Pixhawk Tracker
      UDP                    USB
                                │
                        Servo PWM Output
                                │
                           Arduino Mega
                                │
                     PID Position Controller
                                │
                        BTS7960 Motor Driver
                                │
                       PAN / TILT DC Motors
                                │
                      Directional Antenna
```

---

# 15.8 Verifying MAVLink Messages

Mission Planner provides a MAVLink Inspector for monitoring incoming messages.

The following MAVLink messages should continuously update:

- HEARTBEAT
- GLOBAL_POSITION_INT
- GPS_RAW_INT
- ATTITUDE
- VFR_HUD
- HOME_POSITION
- SYSTEM_TIME
- SERVO_OUTPUT_RAW

Continuous updates indicate that MAVProxy is successfully forwarding telemetry from ArduPlane SITL.

---

# 15.9 Common Issues

## Pixhawk does not receive telemetry

Possible causes:

- Incorrect serial port
- USB device not attached to WSL
- Incorrect baud rate
- Serial output not added in MAVProxy

Verify current outputs.

```text
output
```

Expected:

```text
0: UDP Mission Planner

1: /dev/ttyACM0
```

---

## /dev/ttyACM0 does not appear

Attach the USB device from Windows.

```powershell
usbipd list

usbipd attach --wsl --busid <BUSID>
```

Verify from Ubuntu.

```bash
ls /dev/ttyACM*
```

Expected:

```text
/dev/ttyACM0
```

---

## Packet Loss

Check communication quality.

```text
link
```

Recommended result:

```text
0 lost

0.0% loss
```

If packet loss occurs:

- Use a high-quality USB cable.
- Avoid unstable USB hubs.
- Verify the baud rate configuration.
- Ensure the Pixhawk USB connection is stable.

---

# 15.10 Configuration Summary

| Item | Configuration |
|------|---------------|
| Flight Firmware | ArduPlane SITL |
| MAVLink Router | MAVProxy |
| Mission Planner | UDP :14550 |
| Pixhawk Tracker | USB Serial |
| USB Device | `/dev/ttyACM0` |
| Baud Rate | **115200** |
| MAVLink Protocol | MAVLink 2 |
| Telemetry Source | ArduPlane SITL |
| Multiple Outputs | Supported |
| Packet Loss | 0% (Recommended) |

---

# 15.11 Summary

MAVProxy is the core communication component used during simulation. Acting as a **MAVLink Router**, it distributes telemetry generated by ArduPlane SITL to multiple destinations simultaneously. This enables Mission Planner to visualize the virtual aircraft while the Pixhawk Antenna Tracker receives the exact same telemetry stream, calculates the required PAN and TILT angles, and generates PWM outputs for the Arduino-based closed-loop controller.

Because the tracker receives identical MAVLink packets to those produced during real flights, the entire antenna tracking system—including telemetry communication, Pixhawk configuration, Arduino firmware, PID tuning, and motor control—can be developed and validated safely in a simulated environment before deployment on an actual UAV.

---
# 16. Mission Planner Configuration

Mission Planner is used as the primary **Ground Control Station (GCS)** for monitoring telemetry, configuring ArduPilot parameters, uploading missions, visualizing vehicle movement, and debugging the Antenna Tracker system.

In this project, Mission Planner supports both operation modes:

- **Real UAV Mode** (SpeedyBee F405 + SIYI HM30)
- **Simulation Mode** (ArduPlane SITL + MAVProxy)

Although the telemetry source differs, both modes use the same MAVLink protocol, allowing Mission Planner to operate with minimal configuration changes.

---

# 16.1 Mission Planner Overview

Mission Planner provides a graphical interface for interacting with ArduPilot-based vehicles.

Main capabilities include:

- Vehicle telemetry monitoring
- Flight mode management
- Mission planning
- Parameter configuration
- MAVLink Inspector
- DataFlash log download
- Servo output monitoring
- GPS visualization
- HUD visualization
- Firmware installation

Within this project, Mission Planner primarily functions as a monitoring and debugging tool while Pixhawk ArduTracker performs autonomous antenna tracking.

---

# 16.2 Mission Planner in Real UAV Mode

In Real UAV mode, Mission Planner communicates directly with the telemetry radio connected to the Ground Station.

Communication architecture:

```text
                 UAV
                  │
         SpeedyBee F405
                  │
          MAVLink Telemetry
                  │
           SIYI HM30 Air
                  │
         Wireless Communication
                  │
         SIYI HM30 Ground
                  │
             USB Serial
                  │
          Mission Planner
```

Mission Planner receives:

- GPS Position
- Attitude
- Flight Mode
- Battery Status
- Airspeed
- Mission Progress
- Home Position
- Servo Outputs

---

# 16.3 Mission Planner in Simulation Mode

For simulation, Mission Planner does not connect directly to ArduPlane.

Instead, telemetry is forwarded through MAVProxy.

Communication architecture:

```text
             ArduPlane SITL
                    │
             MAVLink Telemetry
                    │
                MAVProxy
                    │
           UDP :14550 Output
                    │
            Mission Planner
```

Mission Planner receives virtual telemetry exactly as if it originated from a real UAV.

This enables complete mission simulation without requiring physical flight hardware.

---

# 16.4 Connecting to ArduPlane SITL

Open Mission Planner.

On the top-right corner:

Connection Type

```
UDP
```

Baud Rate

```
115200
```

*(Baud rate is ignored for UDP connections but can remain at the default value.)*

Click:

```
CONNECT
```

If successful, the HUD will display the virtual aircraft generated by ArduPlane SITL.

---

# 16.5 Connecting to Pixhawk ArduTracker

When configuring the physical Antenna Tracker, disconnect Mission Planner from SITL if necessary and reconnect using the Pixhawk USB serial port.

Typical settings:

Connection Type

```
COMx
```

Example:

```
COM4
```

Baud Rate

```
115200
```

After connecting, Mission Planner should recognize the Pixhawk running the ArduPilot Antenna Tracker firmware.

---

# 16.6 Flight Data Screen

The Flight Data page provides real-time telemetry visualization.

Displayed information includes:

- Artificial Horizon (HUD)
- GPS Coordinates
- Altitude
- Ground Speed
- Airspeed
- Flight Mode
- Compass Heading
- Waypoints
- Vehicle Position
- Home Position

This screen is primarily used during testing and debugging.

---

# 16.7 Flight Plan Screen

The Flight Plan page is used to:

- Create waypoint missions
- Upload missions
- Download missions
- Set Home Position
- Modify waypoint altitude
- Configure mission commands

For SITL testing, missions can be designed and uploaded directly to the virtual aircraft.

---

# 16.8 Initial Setup Page

The Initial Setup page provides access to:

- Firmware Installation
- Compass Calibration
- Accelerometer Calibration
- Radio Calibration
- Mandatory Hardware Configuration
- Optional Hardware Configuration

For the Antenna Tracker project, this page is mainly used during initial Pixhawk setup.

---

# 16.9 Config / Full Parameter List

The Full Parameter List allows direct access to every ArduPilot parameter.

Examples include:

- MAV_SYSID
- SYSID_TARGET
- SERIALx_BAUD
- SERIALx_PROTOCOL
- AHRS parameters
- GPS parameters
- Servo configuration

After modifying parameters, always click:

```
Write Params
```

to save changes into the Pixhawk.

---

# 16.10 MAVLink Inspector

Mission Planner includes a built-in MAVLink Inspector for monitoring all incoming MAVLink messages.

Open:

```
Ctrl + F

↓

MAVLink Inspector
```

Important messages include:

- HEARTBEAT
- GLOBAL_POSITION_INT
- GPS_RAW_INT
- HOME_POSITION
- ATTITUDE
- VFR_HUD
- SERVO_OUTPUT_RAW
- SYSTEM_TIME

These messages are essential for verifying telemetry communication between ArduPlane SITL and Pixhawk ArduTracker.

---

# 16.11 Status Tab

The Status tab provides hundreds of live variables.

Examples:

- GPS Fix
- Satellites
- Latitude
- Longitude
- Roll
- Pitch
- Yaw
- Airspeed
- Ground Speed
- Battery Voltage
- Current
- Flight Mode

The Status tab is particularly useful for diagnosing telemetry and sensor issues.

---

# 16.12 DataFlash Logs

Mission Planner can download DataFlash logs from the Pixhawk.

These logs include:

- GPS
- IMU
- Compass
- EKF
- RC Input
- Servo Output
- System Events

DataFlash logs are valuable for post-flight analysis and troubleshooting.

---

# 16.13 Home Position Configuration

Antenna Tracker requires a valid Home Position to calculate:

- Azimuth (PAN)
- Elevation (TILT)

### Real UAV Mode

Home Position is obtained automatically from the onboard GPS.

### SITL Mode

If the Pixhawk Tracker has no GPS installed, Home Position must be set manually.

Mission Planner provides:

```
Right Click Map

↓

Set Home Here
```

Once configured, the tracker uses this manually defined location as the Ground Station reference.

---

# 16.14 Multi-Vehicle Support

Mission Planner supports monitoring multiple MAVLink systems.

In this project:

Vehicle 1

```
ArduPlane SITL
```

Vehicle 2

```
Pixhawk ArduTracker
```

Using MAVProxy as the MAVLink Router allows both systems to exchange telemetry while remaining synchronized.

---

# 16.15 Recommended Mission Planner Workflow

The recommended workflow for simulation is:

```text
Start ArduPlane SITL
        │
        ▼
Start MAVProxy
        │
        ▼
Add USB Output
        │
        ▼
Connect Mission Planner (UDP)
        │
        ▼
Verify MAVLink Messages
        │
        ▼
Connect Pixhawk Tracker
        │
        ▼
Set Home Position
        │
        ▼
Arm Tracker
        │
        ▼
Run Mission
        │
        ▼
Observe PAN/TILT Movement
```

---

# 16.16 Common Issues

## Mission Planner Cannot Connect

Possible causes:

- Incorrect connection type
- Firewall blocking UDP
- MAVProxy not running
- Wrong COM port
- Incorrect baud rate

---

## HUD Does Not Move

Possible causes:

- SITL not running
- Mission not started
- GPS not initialized
- Telemetry interruption

---

## No MAVLink Messages

Verify using MAVLink Inspector.

Expected messages:

- HEARTBEAT
- GLOBAL_POSITION_INT
- GPS_RAW_INT

If these messages do not appear, telemetry is not reaching Mission Planner.

---

## Pixhawk Does Not Follow SITL

Possible causes:

- SYSID_TARGET mismatch
- Home Position not configured
- No MAVLink forwarding from MAVProxy
- Incorrect serial baud rate
- USB disconnected

---

# 16.17 Configuration Summary

| Component | Configuration |
|-----------|---------------|
| Ground Control Station | Mission Planner |
| Connection (SITL) | UDP :14550 |
| Connection (Tracker) | USB Serial |
| Default Baud Rate | 115200 |
| MAVLink Protocol | MAVLink 2 |
| Home Position | Manual (No GPS) / Automatic (GPS Installed) |
| Telemetry Source | ArduPlane SITL or Real UAV |
| MAVLink Monitoring | MAVLink Inspector |
| Parameter Configuration | Full Parameter List |
| Mission Planning | Flight Plan |

---

# 16.18 Summary

Mission Planner serves as the primary Ground Control Station throughout the development of the ArduTracker project. It provides a unified interface for telemetry visualization, mission planning, parameter configuration, MAVLink inspection, and system diagnostics.

By supporting both **Real UAV Mode** and **ArduPlane SITL Mode**, Mission Planner enables the same monitoring and debugging workflow regardless of whether telemetry originates from a physical aircraft or a virtual simulation. Combined with MAVProxy, this architecture allows the entire antenna tracking system—including Pixhawk ArduTracker, Arduino controller, and closed-loop PID algorithm—to be validated safely before deployment in real flight operations.

---
# 17. Pixhawk ArduTracker Configuration

The Pixhawk 2.4.8 acts as the core tracking controller in this project by running the **ArduPilot Antenna Tracker** firmware. Its primary responsibility is to receive MAVLink telemetry from either a real UAV or ArduPlane SITL, calculate the required antenna pointing angles (PAN and TILT), and generate PWM outputs for the Arduino-based closed-loop motor controller.

Unlike a conventional servo-based antenna tracker, this project uses the Pixhawk only as a **tracking computer**. The actual motor position control is performed by an Arduino Mega using a PID controller with encoder feedback.

---

# 17.1 Pixhawk Responsibilities

The Pixhawk Antenna Tracker performs the following tasks:

- Receive MAVLink telemetry
- Receive GPS position of the UAV
- Determine Ground Station position
- Calculate Azimuth (PAN)
- Calculate Elevation (TILT)
- Generate PWM outputs
- Control PAN and TILT channels
- Continuously update target angles

The Arduino then converts these PWM signals into accurate motor movements.

---

# 17.2 System Architecture

```text
             Real UAV / ArduPlane SITL
                     │
               MAVLink Telemetry
                     │
                     ▼
            Pixhawk ArduTracker
                     │
          Azimuth & Elevation
               Calculations
                     │
             PWM Servo Output
                     │
                     ▼
              Arduino Mega 2560
                     │
           Closed-Loop PID Control
                     │
             BTS7960 Motor Driver
                     │
             PAN / TILT DC Motors
                     │
            Directional Antenna
```

---

# 17.3 Firmware Installation

Flash the Pixhawk with the **ArduPilot Antenna Tracker** firmware.

Mission Planner

```
Initial Setup

↓

Install Firmware

↓

Antenna Tracker
```

After installation, reconnect Mission Planner.

Verify that the firmware type is:

```
ArduPilot Antenna Tracker
```

---

# 17.4 Communication Configuration

The Pixhawk receives MAVLink telemetry through its USB connection during development.

```text
ArduPlane SITL
        │
     MAVProxy
        │
USB (/dev/ttyACM0)
        │
Pixhawk Tracker
```

For real UAV operation:

```text
HM30 Ground

↓

TELEM Port

↓

Pixhawk Tracker
```

---

# 17.5 Home Position

The tracker always requires a valid Ground Station position.

### Real UAV

If a GPS module is installed:

- Home Position is obtained automatically.

### Development (No GPS)

Since this project uses a Pixhawk without a GPS module during development, Home Position is configured manually.

Mission Planner

```
Right Click Map

↓

Set Home Here
```

The manually assigned location becomes the reference point for all Azimuth and Elevation calculations.

---

# 17.6 Important Parameters

The following parameters are essential for proper operation.

| Parameter | Recommended Value | Description |
|------------|-------------------|-------------|
| SYSID_TARGET | 1 | Target UAV System ID |
| MAV_SYSID | Different from UAV | Tracker System ID |
| SERIAL0_PROTOCOL | 2 | MAVLink over USB |
| SERIAL0_BAUD | 115 | 115200 baud |
| FRAME_CLASS | Tracker Default | Tracker frame type |
| AHRS_ORIENTATION | According to installation | Pixhawk orientation |

---

# 17.7 System ID Configuration

Every MAVLink device requires a unique System ID.

Example configuration:

| Device | MAV_SYSID |
|---------|-----------|
| ArduPlane SITL | **1** |
| Pixhawk Tracker | **2** |
| Mission Planner | **255** |

The tracker must know which vehicle to follow.

```
SYSID_TARGET = 1
```

This instructs the tracker to follow the aircraft whose MAV_SYSID equals 1.

---

# 17.8 Servo Output Configuration

The tracker calculates the target pointing angle and generates PWM outputs.

Typical channel assignment:

| Output | Function |
|---------|----------|
| SERVO1 | PAN |
| SERVO2 | TILT |

PWM range:

| PWM | Angle |
|------|-------|
|1000 µs|Minimum|
|1500 µs|Center|
|2000 µs|Maximum|

These PWM signals are connected to the Arduino Mega instead of directly driving hobby servos.

---

# 17.9 Arduino Interface

Arduino continuously reads the PWM signals generated by the Pixhawk.

```text
Pixhawk PWM

↓

Arduino Mega

↓

PID Controller

↓

Encoder Feedback

↓

Motor Driver

↓

DC Motor
```

Unlike hobby servo systems, Arduino interprets PWM as a **target position**, not as a motor command.

---

# 17.10 Tracker Operating Modes

The Antenna Tracker supports several operating modes.

| Mode | Function |
|------|----------|
| MANUAL | Manual positioning |
| AUTO | Automatic tracking |
| STOP | Stop movement |
| INITIALISING | Startup mode |

Normal operation should always use:

```
AUTO
```

---

# 17.11 Arming the Tracker

Before the tracker begins generating PWM outputs, it must be armed.

Mission Planner

```
Actions

↓

Arm
```

Or via MAVProxy:

```text
arm throttle
```

After arming:

- PWM outputs become active.
- PAN and TILT updates are enabled.
- Tracking calculations begin.

---

# 17.12 Verifying PWM Output

Mission Planner provides several ways to verify PWM generation.

Recommended tools:

- Servo Output Monitor
- Status Tab
- MAVLink Inspector

Useful MAVLink messages include:

- SERVO_OUTPUT_RAW
- RC_CHANNELS
- HEARTBEAT

If SERVO_OUTPUT_RAW changes while the aircraft moves, the tracker is calculating correctly.

---

# 17.13 Verifying Telemetry Reception

The tracker must continuously receive:

- HEARTBEAT
- GLOBAL_POSITION_INT
- GPS_RAW_INT
- HOME_POSITION

If these messages stop updating, tracking will also stop.

---

# 17.14 Tracker Without GPS

During laboratory testing, the tracker operates without its own GPS module.

Configuration:

- Manual Home Position
- USB Telemetry
- ArduPlane SITL
- MAVProxy Routing

Although the tracker has no GPS receiver, it can still calculate PAN and TILT correctly because all required aircraft position data is supplied through MAVLink telemetry.

---

# 17.15 Recommended Configuration

| Component | Recommended Setting |
|-----------|---------------------|
| Firmware | ArduPilot Antenna Tracker |
| Tracker Mode | AUTO |
| MAV_SYSID | 2 |
| SYSID_TARGET | 1 |
| SERIAL0_PROTOCOL | MAVLink |
| SERIAL0_BAUD | 115200 |
| Home Position | Manual (No GPS) |
| Telemetry Source | MAVProxy |
| Arduino Interface | PWM |
| Control Method | Closed Loop PID |

---

# 17.16 Troubleshooting

## Tracker Does Not Move

Possible causes:

- Tracker not armed.
- Incorrect SYSID_TARGET.
- No incoming MAVLink telemetry.
- Invalid Home Position.
- AUTO mode not selected.

---

## No PWM Output

Possible causes:

- Tracker disarmed.
- Telemetry unavailable.
- No GPS/Home Position.
- Incorrect servo configuration.

---

## Arduino Receives Constant PWM

Possible causes:

- UAV not moving.
- Mission not started.
- Tracker not calculating target angles.
- No GLOBAL_POSITION_INT messages.

---

## Tracker Does Not Follow SITL

Verify:

- MAVProxy output includes `/dev/ttyACM0`.
- `SYSID_TARGET = 1`.
- SITL `MAV_SYSID = 1`.
- Tracker is armed.
- Tracker is in AUTO mode.
- Home Position has been configured.

---

# 17.17 Configuration Checklist

Before running the complete system, verify the following:

- ✅ Pixhawk flashed with ArduPilot Antenna Tracker firmware.
- ✅ Tracker connected to Mission Planner.
- ✅ Home Position configured.
- ✅ Tracker switched to AUTO mode.
- ✅ Tracker armed successfully.
- ✅ `SYSID_TARGET = 1`.
- ✅ `MAV_SYSID` differs from the aircraft.
- ✅ MAVProxy forwarding telemetry to Pixhawk.
- ✅ Arduino receives PWM signals.
- ✅ Encoder feedback is operational.
- ✅ BTS7960 responds to PID output.
- ✅ PAN and TILT motors rotate correctly.

---

# 17.18 Summary

The Pixhawk Antenna Tracker serves as the intelligent tracking controller within the system. It processes MAVLink telemetry from either a real UAV or ArduPlane SITL, computes the required Azimuth and Elevation angles, and generates PWM references for the Arduino-based motion controller. By combining Pixhawk's navigation capabilities with Arduino's closed-loop PID control and encoder feedback, the system achieves precise, reliable, and repeatable antenna tracking suitable for both laboratory simulation and real-world UAV operations.



