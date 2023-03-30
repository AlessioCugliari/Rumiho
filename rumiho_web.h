#pragma once

//check the received signal strength and notifies it with the color of the led: 
//BLUE ok 
//RED is 0
void rssi_led_notity();

void printWifiStatus();

void webPage(WiFiClient *client, int8_t calib);