#include <TimerOne.h>

#define PACE 6000000  // 50,000 us 

void setup()
{
    Serial.begin(9600);
    Serial.println("Starting...");
    delay(3000);
    Timer1.initialize(PACE);
    Timer1.attachInterrupt(&timerISR);
    Timer1.start();
}

void loop()
{
    Serial.println("loop");
    if (Serial.available() > 1)
    {
        unsigned long period = Serial.parseInt() * 1000;
        Timer1.setPeriod(period);
        Serial.print("reset perdiod to ");
        Serial.print(period);
        Serial.println(" us");
    }
    delay(1000);
}

void timerISR()
{
    Serial.println("interrupt");
}