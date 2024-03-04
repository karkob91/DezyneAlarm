#include <Arduino.h>
#include <memory>
#include "alarm.hh"
#include "timer.hh"
#include <dzn/locator.hh>
#include <dzn/runtime.hh>

// Pin Definitions
#define PIR_SENSOR_PIN 33
#define BUZZER_PIN 25
#define STATUS_LED_PIN 26

// PWM Channels and Settings
#define BUZZER_CHANNEL 0
#define STATUS_LED_CHANNEL 2
#define PWM_RESOLUTION 8
#define PWM_FREQUENCY_SLOW 4 // Hz
#define PWM_FREQUENCY_FAST 8 // Hz
#define DUTY_CYCLE_HALF 127
#define DUTY_CYCLE_FULL 255
#define DUTY_CYCLE_OFF 0

// System Constants
#define VALID_PIN 1234
#define DEBOUNCE_DELAY 200 // milliseconds
#define BUZZER_FREQUENCY 2000 // Hz

// Global Variables
dzn::runtime runtime;
dzn::locator loc;
std::unique_ptr<AlarmController> alarm_comp;
Timer alarmTimer;
String inputString = "";
bool pinEntered = false;

// Function Declarations
void serialEvent();
void startBuzzer();
void stopBuzzer();
void setupPWM();
void controlStatusLED(int frequency, int dutyCycle);
void setupDezyneComponent();
void detectMovement();
void processSerialInput();

void setup() {
    Serial.begin(115200); // Initialize serial communication
    pinMode(PIR_SENSOR_PIN, INPUT); // Set PIR sensor as input
    pinMode(BUZZER_PIN, OUTPUT); // Set buzzer pin as output
    pinMode(STATUS_LED_PIN, OUTPUT); // Set status LED pin as output

    setupPWM(); // Initialize PWM settings for buzzer and LED

    inputString.reserve(200); // Reserve 200 bytes for inputString to prevent reallocation

    // Initialize Dezyne locator and runtime
    loc.set(runtime);
    alarm_comp = std::unique_ptr<AlarmController>(new AlarmController(loc));

    // Setup Dezyne component callbacks
    setupDezyneComponent();
}

void loop() {
  // Check and update timer for alarm system
  alarmTimer.check();
  
  // Process serial input if PIN has been entered
  if (pinEntered) {
    processSerialInput();
  }

  // Check for movement if system is armed
  detectMovement();
}

void setupDezyneComponent(){
  // Setup callbacks for the Dezyne alarm component
  // Dezyne generated code comes with empty handlers for the system ports
  alarm_comp->auth.in.valid = [](int pin) {
        // Terrible way of validating PIN
        Serial.println("Checking PIN...");
        bool valid = pin == VALID_PIN;
        Serial.println(valid ? "PIN valid." : "PIN invalid.");
        return valid;
    };

    alarm_comp->timer.in.set = [](int ms_delay){
        // Set a delay for actions like arming or triggering the siren
        // Dezyne allows direct access to internal state variables
        if (alarm_comp->console.state == IConsole::State::Disarmed)
          Serial.print("Arming in ");
        else
          Serial.print("Starting siren in ");
        Serial.print(ms_delay / 1000);
        Serial.println(" seconds...");
        alarmTimer.set(ms_delay, [](){ 
          // Use Dezyne timeout event as a callback
          alarm_comp->timer.out.timeout(); 
        });
    };

    alarm_comp->timer.in.cancel = [](){
        alarmTimer.cancel();
    };

    alarm_comp->siren.in.enable = [](){
        Serial.println("Siren Enabled!");
        startBuzzer();
        controlStatusLED(PWM_FREQUENCY_FAST, DUTY_CYCLE_HALF);
    };

    alarm_comp->siren.in.disable = [](){
        Serial.println("Siren Disabled!");
        stopBuzzer();
    };

    alarm_comp->console.out.detected = [](){
        Serial.println("Burglar Detected!");
        controlStatusLED(PWM_FREQUENCY_SLOW, DUTY_CYCLE_HALF);
    };

    alarm_comp->console.out.armed = [](){
        Serial.println("Alarm Armed!");
        controlStatusLED(PWM_FREQUENCY_SLOW, DUTY_CYCLE_FULL);
    };

    alarm_comp->console.out.disarmed = [](){
        Serial.println("Alarm Disarmed!");
        controlStatusLED(PWM_FREQUENCY_SLOW, DUTY_CYCLE_OFF);
    };

    alarm_comp->console.out.arming = [](){
        controlStatusLED(PWM_FREQUENCY_SLOW, DUTY_CYCLE_HALF);
    };
}

void processSerialInput() {
    // Process received serial input for PIN validation
    Serial.println(inputString); // Debug print
    int receivedPin = inputString.toInt();
    // Trigger Dezyne alarm component with entered PIN
    alarm_comp->console.in.arm(receivedPin);
    inputString = ""; // Reset input string for next input
    pinEntered = false; // Reset PIN entered flag
}

void serialEvent() {
    while (Serial.available()) {
        // get the new byte:
        char inChar = (char)Serial.read();
        // add it to the inputString:
        inputString += inChar;
        // if the incoming character is a newline, set a flag so the main loop can
        // do something about it:
        if (inChar == '\n') {
            pinEntered = true;
        }
    }
}

void detectMovement() {
    // Detect movement using PIR sensor when system is armed
    if ((alarm_comp->console.state == IConsole::State::Armed) && (digitalRead(PIR_SENSOR_PIN) == HIGH)) {
        alarm_comp->sensor.out.MovementDetected();
    }
}

void setupPWM() {
    // Setup PWM for buzzer
    ledcSetup(BUZZER_CHANNEL, PWM_FREQUENCY_SLOW, PWM_RESOLUTION);
    ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);

    // Setup PWM for status LED
    ledcSetup(STATUS_LED_CHANNEL, PWM_FREQUENCY_SLOW, PWM_RESOLUTION);
    ledcAttachPin(STATUS_LED_PIN, STATUS_LED_CHANNEL);
    ledcWrite(STATUS_LED_CHANNEL, DUTY_CYCLE_OFF); // Start with LED off
}

void controlStatusLED(int frequency, int dutyCycle) {
    // Control the status LED's blinking frequency and brightness
    ledcSetup(STATUS_LED_CHANNEL, frequency, PWM_RESOLUTION);
    ledcWrite(STATUS_LED_CHANNEL, dutyCycle);
}

void startBuzzer() {
    // Start the buzzer with a predefined frequency
    ledcWriteTone(BUZZER_CHANNEL, BUZZER_FREQUENCY);
}

void stopBuzzer() {
    // Stop the buzzer by setting the duty cycle to 0
    ledcWrite(BUZZER_CHANNEL, DUTY_CYCLE_OFF);
}