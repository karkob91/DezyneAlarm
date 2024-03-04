# Implementing a Dezyne Model as an Alarm System on ESP32

Learn how to use the Verum Dezyne model to create a simple yet fully functional alarm system on the ESP32 board using the Arduino Framework. This tutorial covers the entire development process, from design to testing, providing a convenient starting point for various applications.

## Overview

1. **Introduction**
   - Briefly introduce the goal and the scope of the project.
2. **Requirements**
   - Hardware: ESP32 ESP-WROOM-32, PIR sensor HC-SR501, active buzzer module, LED, resistors, cables, USB cable.
   - Software: PlatformIO extension, Verum Dezyne VS Code extension.

## Setup

### Install PlatformIO Extension

- Guide on installing PlatformIO IDE within Visual Studio Code.

### Clone DezyneAlarm Repository

- Instructions for cloning the repository and navigating the project structure.

## Hardware Setup

![Hardware Setup](path/to/hardware_setup_image.jpg)

- Detailed description of connecting the components as per the scheme below:

![Alarm Scheme](path/to/alarm_scheme_image.png)

## Software Development

### Designing the System

- Steps to design the system using the Verum Dezyne VS Code extension.

### Generating C++ Code

- Instructions on how to generate C++ code from the Dezyne model.

### Setting Up the Project

- Copy all Dezyne runtime files to the `src` directory:

![Dezyne Runtime Files](path/to/dezyne_runtime_files_image.png)

- Include the Dezyne locator and runtime in your `main.cpp`:

```cpp
#include <dzn/locator.hh>
#include <dzn/runtime.hh>
#include "alarm.hh"
```

### Initializing the Dezyne Component

Guide on initializing the Dezyne component, creating an empty pointer, and setting up the locator and runtime:

```cpp
dzn::runtime runtime;
dzn::locator loc;
std::unique_ptr<AlarmController> alarm_comp;
```

### Initialize Dezyne locator and runtime:

```cpp
loc.set(runtime);
alarm_comp = std::unique_ptr<AlarmController>(new AlarmController(loc));
```

### Setting Up Callbacks:

Detailed instructions on setting up callbacks for the Dezyne alarm component, including validation, timers, and action handlers:

```cpp
void setupDezyneComponent(){
  // Setup callbacks for the Dezyne alarm component
  alarm_comp->auth.in.valid = [](int pin) {
      Serial.println("Checking PIN...");
      bool valid = pin == VALID_PIN; // Replace VALID_PIN with your actual pin
      Serial.println(valid ? "PIN valid." : "PIN invalid.");
      return valid;
  };

  alarm_comp->timer.in.set = [](int ms_delay){
      if (alarm_comp->console.state == IConsole::State::Disarmed)
        Serial.print("Arming in ");
      else
        Serial.print("Starting siren in ");
      Serial.print(ms_delay / 1000);
      Serial.println(" seconds...");
      alarmTimer.set(ms_delay, [](){ 
        alarm_comp->timer.out.timeout(); 
      });
  };

  alarm_comp->timer.in.cancel = [](){
      alarmTimer.cancel();
  };

  alarm_comp->siren.in.enable = [](){
      Serial.println("Siren Enabled!");
      // startBuzzer(); // Implement buzzer start
      // controlStatusLED(PWM_FREQUENCY_FAST, DUTY_CYCLE_HALF); // Implement LED control
  };

  alarm_comp->siren.in.disable = [](){
      Serial.println("Siren Disabled!");
      // stopBuzzer(); // Implement buzzer stop
  };

  alarm_comp->console.out.detected = [](){
      Serial.println("Burglar Detected!");
      // controlStatusLED(PWM_FREQUENCY_SLOW, DUTY_CYCLE_HALF); // Implement LED control
  };

  alarm_comp->console.out.armed = [](){
      Serial.println("Alarm Armed!");
      // controlStatusLED(PWM_FREQUENCY_SLOW, DUTY_CYCLE_FULL); // Implement LED control
  };

  alarm_comp->console.out.disarmed = [](){
      Serial.println("Alarm Disarmed!");
      // controlStatusLED(PWM_FREQUENCY_SLOW, DUTY_CYCLE_OFF); // Implement LED control
  };

  alarm_comp->console.out.arming = [](){
      // controlStatusLED(PWM_FREQUENCY_SLOW, DUTY_CYCLE_HALF); // Implement LED control
  };
}
```
