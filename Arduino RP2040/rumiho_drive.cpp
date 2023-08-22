#include <Arduino.h>

#include "rumiho_drive.h"
#include "rumiho_motors.h"
#include "rumiho_globals.h"

void read_ir_sensor(){
    valueIR_R = analogRead(IR_PIN_R);
    valueIR_C = analogRead(IR_PIN_C);
    valueIR_L = analogRead(IR_PIN_L);
}

void set_drive_value(){
    minValue[0] = valueIR_R;
    maxValue[0] = valueIR_R;

    minValue[1] = valueIR_C;
    maxValue[1] = valueIR_C;

    minValue[2] = valueIR_L;
    maxValue[2] = valueIR_L;
}

int8_t calibrate(){

    //read the initial value
    read_ir_sensor();

    for(int i = 0; i < 3100; i++){
        turn_right_inplace();

        read_ir_sensor();

        if(valueIR_L > maxValue[0]) maxValue[0] = valueIR_L;
        if(valueIR_L < minValue[0]) minValue[0] = valueIR_L;

        if(valueIR_C > maxValue[1]) maxValue[1] = valueIR_C;
        if(valueIR_C < minValue[1]) minValue[1] = valueIR_C;
        
        if(valueIR_R > maxValue[2]) maxValue[2] = valueIR_R;
        if(valueIR_R < minValue[2]) minValue[2] = valueIR_R;

    }

    move_stop();
    
    for(int i = 0; i < 3; i++){
        threshold[i] = (minValue[i] + maxValue[i]) >> 1;
    }

    return 1;
}

void line_follow(){
  while(1){

    read_ir_sensor();
    
    /*if(valueIR_L > threshold[0] && valueIR_C > threshold[1]){
      turn_left_inplace();
    }*/
    if(valueIR_C > threshold[1]){
      move_forward();
    }
    /*else if(valueIR_R > threshold[2] && valueIR_C > threshold[1]){
      turn_right_inplace();
    }
    else if(valueIR_C > threshold[1]){
      move_forward();
    }*/
    else if(valueIR_R > threshold[2]){
      //turn_left();
      turn_right_inplace();
    }
    else if(valueIR_L > threshold[0]){
      //turn_right();
      turn_left_inplace();
    }
  }
}
