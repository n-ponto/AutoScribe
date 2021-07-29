
// CNC Shield Stepper  Control Demo
// Superb Tech
// www.youtube.com/superbtech

// X axis   step: 2     direction: 5
// Y axis   step: 3     direction: 6

#include <Servo.h>
#include "StepperController.h"

StepperController sc;

long lastUpdate = 0;
const long toggleDelay = 2000;

// Servo
const int ServoPin = 11;  // PWM pin for the servo
int servoAngle = 90;
Servo servo;
bool penDown = false;

// Joystick
const int X_pin = 0;
const int Y_pin = 1;
const int SW_pin = 13;

const int cutoff = 180; // difference in joystick reading to ignore
const int loRange = (512-cutoff), hiRange = (512+cutoff);
void handleJoystick()
{
    bool sw = digitalRead(SW_pin);
    int x = analogRead(X_pin);
    int y = analogRead(Y_pin);

    if (sw && toggleDelay < millis() - lastUpdate ) 
    {
        Serial.println("move pen");
        servoAngle = penDown ? 110 : 130;
        servo.write(servoAngle);
        penDown = !penDown;
        lastUpdate = millis();
    }

    // Readings range from 0-1023 (center 512)
    // Y=0 -> UP  X=0 -> RIGHT
    bool u, d, l, r;
    d = y < loRange;
    u = y > hiRange;
    r = x > hiRange;
    l = x < loRange;

    // Disable if not receving commands
    if (!(u || d || l || r)) 
    {
        sc.disable();
        return;
    }

    sc.enable();
    Direction dir = (u << 3) + (d << 2) + (l << 1) + r;
    Serial.print("direction: ");
    Serial.print(dir);
    // sc.move(dir, 1);
}

// void handleSerialInputComplex()
// {
//     if (Serial.available())
//     {
//         disable(); enableSteppers = false;
//         char c, _[2];
//         Serial.readBytes(&c, 1);
//         Serial.readBytes(_, 2);
//         if (c == 'a' || c == 'A')
//         {
//             Serial.print("current angle: ");
//             Serial.println(servoAngle);
//             while (!Serial.available()) {}
//             servoAngle = Serial.parseInt();
//             servo.write(servoAngle);
//             Serial.print("new angle: ");
//             Serial.println(servoAngle);
//         }
//         else if (c == 's' || c == 'S')
//         {
//             Serial.print("current speed (us): ");
//             Serial.println(WAIT);
//             while (!Serial.available()) {}
//             WAIT = Serial.parseInt();
//             Serial.print("new speed (us): ");
//             Serial.println(WAIT);
//         }
//         Serial.readBytes(_, 2);
//     }
// }

// void handleSerialInput()
// {
//     if (Serial.available())
//     {
//         disable(); enableSteppers = false;
//         char _[2];
//         int n = Serial.parseInt();
//         Serial.readBytes(_, 2);
//         if (n > 180)
//         {
//             Serial.print("old speed (us): ");
//             Serial.println(WAIT);
//             WAIT = n;
//             Serial.print("new speed (us): ");
//             Serial.println(WAIT);
//         }
//         else if (n > 0)
//         {
//             Serial.print("old angle: ");
//             Serial.println(servoAngle);
//             servoAngle = n;
//             servo.write(servoAngle);
//             Serial.print("new angle: ");
//             Serial.println(servoAngle);
//         }
//         else 
//         {
//             servoAngle = penDown ? 120 : 140;
//             servo.write(servoAngle);
//             penDown = !penDown;
//         }
//     }
// }

void setup() {
  Serial.begin(9600);
  // Steppers

  // Servo
  servo.attach(ServoPin);
  servo.write(servoAngle);
  // Joystick
  pinMode(SW_pin, INPUT);
  // X and Y pins are analog & auto input
}

void square(int steps)
{
    sc.move(UP, steps);
    sc.move(RIGHT, steps);
    sc.move(DOWN, steps);
    sc.move(LEFT, steps);
}

void diamond(int steps)
{
    sc.move(UP + RIGHT, steps);
    sc.move(DOWN + RIGHT, steps);
    sc.move(DOWN + LEFT, steps);
    sc.move(UP + LEFT, steps);
}

int steps;
char c[2];
void loop() 
{
    if (Serial.available())
    {
        steps = Serial.parseInt();
        Serial.readBytes(c, 2);
        Serial.print("Moving ");
        Serial.print(steps);
        Serial.println(" steps.");
        sc.enable();
        diamond(steps);
        sc.disable();
    }
}