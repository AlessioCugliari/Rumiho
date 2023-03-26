#include <stdio.h>
#include <Arduino.h>

#include "rumiho_motors.h"

void setPinMotor(){
  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);
}

void setSpeed(uint8_t speed){
  analogWrite(SPEED_PIN_A,speed);
  analogWrite(SPEED_PIN_B,speed);
}

void move_forward(){
  setSpeed(speed);
  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);

  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);
}

void move_backward(){
  setSpeed(speed);
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH);

  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, HIGH);
}

void turn_left_inplace(){
  setSpeed(speed);
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH);

  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);
}

void turn_right_inplace(){
  setSpeed(speed);
  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);

  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, HIGH);
}

void turn_right(){
  
  analogWrite(SPEED_PIN_A,speed);
  analogWrite(SPEED_PIN_B,100);

  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);

  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, HIGH);
}

void turn_left(){

  analogWrite(SPEED_PIN_A,100);
  analogWrite(SPEED_PIN_B,speed);
  
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH);

  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);
}

void move_stop(){
  setSpeed(0);
}