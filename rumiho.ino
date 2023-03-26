//#include <ArduinoBLE.h>
#include <Arduino_LSM6DSOX.h>
#include <SPI.h>
#include <WiFiNINA.h>

//#include "common.h"
#include "private.h"
#include "rumiho_globals.h"
#include "rumiho_motors.h"

uint8_t speed = 250;

int valueIR_C = -1;
int valueIR_R = -1;
int valueIR_L = -1;

int minValue[3], maxValue[3], threshold[3]; //0 L, 1 C, 2 R

int calib = 0;
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
  
  if(rssi){
    digitalWrite(LEDB, HIGH);
    }
    else{
      digitalWrite(LEDR, HIGH);
    }
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
            webPage(&client);
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

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

void webPage(WiFiClient *client){
  // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
  // and a content-type so the client knows what's coming, then a blank line:

  client->println("HTTP/1.1 200 OK");
  client->println("Content-type:text/html");
  client->println();

  // the content of the HTTP response follows the header:
  client->print("<style>");
  client->print(".container {margin: 0 auto; text-align: center; margin-top: 100px;}");
  client->print("button {color: white; width: 100px; height: 100px;");
  client->print("border-radius: 10%; margin: 20px; border: none; font-size: 20px; outline: none; transition: all 0.2s;}");
  client->print(".red{background-color: rgb(196, 39, 39);}");
  client->print(".green{background-color: rgb(39, 121, 39);}");
  client->print(".blue {background-color: rgb(5, 87, 180);}");
  client->print(".off{background-color: grey;}");
  client->print("button:hover{cursor: pointer; opacity: 0.7;}");
  client->print("</style>");
  client->print("<div class='container'>");
  client->print("<div>Rumiho</div>");
  client->print("<button class='off' type='submit' onmousedown='location.href=\"/UP\"'>UP</button><br>");
  client->print("<button class='off' type='submit' onmousedown='location.href=\"/LEFT\"'>LEFT_INPLACE</button>");
  client->print("<button class='off' type='submit' onmousedown='location.href=\"/RIGHT\"'>RIGHT_INPLACE</button><br>");
  client->print("<button class='off' type='submit' onmousedown='location.href=\"/DOWN\"'>DOWN</button><br>");
  client->print("<button class='off' type='submit' onmousedown='location.href=\"/STOP\"'>STOP</button><br>");
  client->print("<button class='off' type='submit' onmousedown='location.href=\"/LINE\"'>LINE FOLLOW</button>");
  //client->print("<button class='off' type='submit' onmousedown='location.href=\"/LEFT2\"'>LEFT</button>");
  //client->print("<button class='off' type='submit' onmousedown='location.href=\"/RIGHT2\"'>RIGHT</button>");
  if(calib) client->print("<button class='off' type='submit' onmousedown='location.href=\"/START\"'>START</button>");
  client->print("</div>");

  // The HTTP response ends with another blank line:
  client->println();
  // break out of the while loop:
}

int calibrate(){

  //read the initial value
  valueIR_R = analogRead(IR_PIN_R);
  valueIR_C = analogRead(IR_PIN_C);
  valueIR_L = analogRead(IR_PIN_L);
  
  minValue[0] = valueIR_R;
  maxValue[0] = valueIR_R;

  minValue[1] = valueIR_C;
  maxValue[1] = valueIR_C;

  minValue[2] = valueIR_L;
  maxValue[2] = valueIR_L;

  for(int i = 0; i < 3100; i++){
    turn_right_inplace();

    valueIR_R = analogRead(IR_PIN_R);
    valueIR_C = analogRead(IR_PIN_C);
    valueIR_L = analogRead(IR_PIN_L);

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
    valueIR_R = analogRead(IR_PIN_R);
    valueIR_C = analogRead(IR_PIN_C);
    valueIR_L = analogRead(IR_PIN_L);
    
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





//commento porta fortuna :)
