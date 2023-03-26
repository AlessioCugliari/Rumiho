#pragma once

#define IR_PIN_C A3
#define IR_PIN_R A6
#define IR_PIN_L A7

#define SPEED_PIN_A A1
#define SPEED_PIN_B A2
/*
uint8_t motor1pin1 = 10;  //IN1
uint8_t motor1pin2 = 9;  //IN2

uint8_t motor2pin1 = 8;  //IN3
uint8_t motor2pin2 = 7;  //IN4
*/

#define motor1pin1 10  //IN1
#define motor1pin2 9   //IN2

#define motor2pin1 8   //IN3
#define motor2pin2 7   //IN4

extern uint8_t speed;

void setPinMotor();

void setSpeed(uint8_t speed);

void move_forward();

void move_backward();

void turn_left_inplace();

void turn_right_inplace();

void turn_right();

void turn_left();

void move_stop();
