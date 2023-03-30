//#include <ArduinoBLE.h>

#include <SPI.h>
#include <WiFiNINA.h>
#include <Arduino_LSM6DSOX.h>

//#include "common.h"
#include "private.h"
#include "rumiho_globals.h"
#include "rumiho_motors.h"
#include "rumiho_imu.h"
#include "rumiho_drive.h"
#include "rumiho_web.h"

uint8_t speed = 250;

int8_t calib = 0;
int P, D, I, erroR, precError, PIDvalue;

float Ax, Ay, Az;
float Gx, Gy, Gz;

char pass[] = SECRET_PASS;
char ssid[] = SECRET_ID;

int status = WL_IDLE_STATUS;
WiFiServer server(80);
long rssi = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
  Serial.begin(9600);
  //while(!Serial);   //wait to open the serial son we don't miss information
  IMU.begin();
  
  wifiSetup();
  setPinMotor();
  printWifiStatus();
  setSpeed(speed);
}


void loop() {
  // put your main code here, to run repeatedly:
  
  /*valueIR_R = analogRead(IR_PIN_R);
  Serial.print("Sensor Value R: ");
  Serial.print(valueIR_R);

  valueIR_C = analogRead(IR_PIN_C);
  Serial.print("    Sensor Value C: ");
  Serial.print(valueIR_C);

  valueIR_L = analogRead(IR_PIN_L);
  Serial.print("    Sensor Value L: ");
  Serial.println(valueIR_L);*/
  
  rssi_led_notity();

  if(IMU.accelerationAvailable()){
    IMU.readAcceleration(Ax, Ay, Az);
  }

  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(Gx, Gy, Gz);
  }

  WiFiClient client = server.available();
  if(client){
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";
    rssi = WiFi.RSSI();
    if(rssi){
      digitalWrite(LEDB, LOW);
    }
    else{
      digitalWrite(LEDR, LOW);
    }
    while (client.connected()){
      digitalWrite(LEDG, LOW);
      if(client.available()){
        char c = client.read();
        Serial.write(c);
        if(c == '\n'){
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0){
            webPage(&client,calib);
            break;
          }else{
            currentLine = "";
            }
        }else if(c != '\r'){  // if you got anything else but a carriage return character,
          currentLine += c;   // add it to the end of the currentLine
        }
        doCommand(currentLine);     
      }
    }
    // close the connection
    client.stop();
    Serial.println("client disconnected");
  }    
}

void doCommand(String command){
  if (command.endsWith("GET /UP")) {
    move_forward();
    return;
  }
  if (command.endsWith("GET /DOWN")) {
    move_backward();
    return;           
  }
  if (command.endsWith("GET /RIGHT")) {
    turn_right_inplace();
    return;                
  }
  if (command.endsWith("GET /LEFT")) {
    turn_left_inplace();
    return;         
  }
  if (command.endsWith("GET /LINE")) {
    calib = calibrate();            
  }
  if (command.endsWith("GET /STOP")) {
    move_stop();
    return;              
  }
  if (command.endsWith("GET /RIGHT2")) {
    turn_right();
    return;                
  }
  if (command.endsWith("GET /LEFT2")) {
    turn_left();
    return;         
  }
  if (command.endsWith("GET /START")) {
    line_follow();
    return;         
  }
  
}

void wifiSetup(){
  // check for the WiFi module:
  WiFi.setHostname("Rumiho");
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }
  //attemp to conncet to WiFi
  while(status != WL_CONNECTED){
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network.
    status = WiFi.begin(ssid,pass);
    delay(10000); // wait 10 seconds for connection
  }
  server.begin();
}

//commento porta fortuna :)