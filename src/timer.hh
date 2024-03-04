#include <Arduino.h>
#include <functional> // Include for std::function

class Timer {
public:
    Timer() : startTime(0), timeoutDuration(0), running(false) {}

    void set(unsigned long duration, std::function<void()> callback) {
        timeoutDuration = duration;
        this->callback = callback;
        startTime = millis();
        running = true;
    }

    void cancel() {
        running = false;
    }

    void check() {
        if (!running) return;
        if ((millis() - startTime) >= timeoutDuration) {
            running = false; // Stop the timer
            if (callback) callback(); // Call the timeout callback function
        }
    }

private:
    unsigned long startTime;
    unsigned long timeoutDuration;
    bool running;
    std::function<void()> callback; // Callback function for timeout
};
