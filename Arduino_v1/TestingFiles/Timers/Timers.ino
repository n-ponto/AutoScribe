/*
Used to test setting a timer using the TimerOne library, and test changing the
duration of the timer during runtime
*/
#include <TimerOne.h>

#define RATE 6000000

void setup()
{
    Serial.begin(9600);
    Serial.println("Starting...");
    Serial.println("Send a number to set the timer in milliseconds.")
    delay(3000);
    Timer1.initialize(RATE);
    Timer1.attachInterrupt(&timerISR);
    Timer1.start();
}

void loop()
{
    Serial.println("loop");
    // Check if user input
    if (Serial.available() > 1)
    {
        // Get input number and set timer
        unsigned long period = Serial.parseInt() * 1000;
        Timer1.setPeriod(period);
        Serial.print("reset perdiod to ");
        Serial.print(period);
        Serial.println(" us");
    }
    // Main loop always delays one second
    delay(1000);
}

void timerISR()
{
    // Print to indicate the timer has gone off
    Serial.println("interrupt");
}