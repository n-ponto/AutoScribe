
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

char c[2];
void setup() {
  Serial.begin(9600);
  // Steppers

  // Servo
  servo.attach(ServoPin);
  servo.write(servoAngle);
  // Joystick
  pinMode(SW_pin, INPUT);
  // X and Y pins are analog & auto input
  sc.home();
  Serial.readBytes(c, 2);
}

// TESTING FUNCTIONS
void readStepsAndSqureDiamond();
void readAndTravelToCoordinates();
void testTheory();

int steps;

void loop() 
{
    testTheory();
}

/*******************************************************************************
*******************************************************************************/

void readStepsAndSqureDiamond()
{
    if (Serial.available())
    {
        steps = Serial.parseInt();
        Serial.readBytes(c, 2);
        if (steps < 1)
        {
            sc.home();
        }
        else 
        {
            Serial.print("Moving ");
            Serial.print(steps);
            Serial.println(" steps.");
            sc.enable();
            sc.square(steps);
            sc.diamond(steps);
            sc.disable();
        }
    }
}

char inarr[10];
void readAndTravelToCoordinates()
{
    if (Serial.available())
    {   
        Point pt; 
       
        pt.x = Serial.parseInt();
        pt.y = Serial.parseInt();
        Serial.readBytes(c, 2);
        Serial.println("Received: (" + String(pt.x) + ", " +  String(pt.y) + ")");
        sc.enable();
        sc.travel(pt);
        sc.disable();
    }
}


void testTheory()
{
    if (Serial.available())
    {
        Serial.read();
        Point p1, p2, p3;
        p1.x = 400; p1.y = 400;
        p2.x = 400; p2.y = 200;
        p3.x = 200; p3.y = 200;  
        sc.enable();
        sc.travel(p1);
        sc.travel(p2);
        sc.travel(p3);
        sc.disable();
        sc.position.x = sc.position.y = 0;
    }
}
