#include "Motor.h"
#include <iostream>

Motor::Motor(){
    mSpeed= 150;
    mDir=true;
    pinMode(pwmPin, PWM_OUTPUT);
    pinMode(leftDirPin, OUTPUT);//left direction
    pinMode(rightDirPin, OUTPUT);//right direction
    digitalWrite(rightDirPin, LOW);
    digitalWrite(leftDirPin, HIGH);
    
    
}
void Motor::setSpeed(int speed){
    mSpeed = speed;
    std::cout << "Setspeed:" << mSpeed << std::endl;
   
}
void Motor::setDirection(bool direction){
    mDir=direction;
    if (mDir==false){//left direction
        digitalWrite(rightDirPin, LOW);
        digitalWrite(leftDirPin, HIGH);
    }
    else{//right direction
        digitalWrite(rightDirPin, HIGH);
        digitalWrite(leftDirPin, LOW);
    }
}
void Motor::startMotor(){
     std::cout << "Start:" << mSpeed << std::endl;
     pwmWrite(pwmPin, mSpeed);
}
void Motor::stopMotor(){
     pwmWrite(pwmPin,0);
}
