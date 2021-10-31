#ifndef PEN_H_
#define PEN_H_

#include <Servo.h>

typedef uint8_t Angle;

class Pen
{
  public:
    Pen();
    void goTo(Angle);
    void setUpAngle(Angle);
    void setDownAngle(Angle);
    void up();
    void down();
  private:
    Servo servo;
    Angle upAngle, downAngle;
};

#endif // PEN_H_