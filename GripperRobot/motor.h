#ifndef MOTOR_H
#define MOTOR_H
#include <wiringPi.h>

class Motor {
public:
    Motor();
    void setSpeed(int speed);
    void setDirection(bool direction);
    void startMotor();
    void stopMotor();

private:
    int mSpeed;
    bool mDir;
    int leftDirPin = 27;
    int rightDirPin = 22;
    int pwmPin = 12;
};

#endif
