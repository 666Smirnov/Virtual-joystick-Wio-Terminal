/*
 *  This sketch sends random data over UDP on a ESP32 device
 *
 */
#include <rpcWiFi.h>
#include <WiFiUdp.h>
#include"LIS3DHTR.h"

LIS3DHTR<TwoWire> lis;

// WiFi network name and password:
const char * networkName = "SSID";
const char * networkPswd = "Password";

//IP address to send UDP data to:
// either use the ip address of the server or 
// a network broadcast address
const char * udpAddress = "192.168.18.7"; // IP адрес на который передаем.
const int udpPort = 3333;

//Are we currently connected?
boolean connected = false;

//The udp library class
WiFiUDP udp;

float x_values, y_values, z_values;

char x[8];
char y[8];
char z[8];

void setup(){
  // Initilize hardware serial:
  Serial.begin(115200);
  
  //Connect to the WiFi network
  connectToWiFi(networkName, networkPswd);

  pinMode(WIO_KEY_A, INPUT_PULLUP);
  pinMode(WIO_KEY_B, INPUT_PULLUP);
  pinMode(WIO_KEY_C, INPUT_PULLUP);

  lis.begin(Wire1);
  lis.setOutputDataRate(LIS3DHTR_DATARATE_25HZ); //Data output rate
  lis.setFullScaleRange(LIS3DHTR_RANGE_2G); //Scale range set to 2g
}


void loop(){
  //only send data when connected
  if(connected){
    x_values = lis.getAccelerationX();
    y_values = lis.getAccelerationY();
    z_values = lis.getAccelerationZ();

    dtostrf(x_values, 6, 2, x);
    dtostrf(y_values, 6, 2, y);
    dtostrf(z_values, 6, 2, z);

    
    //Send XYZ
    udp.beginPacket(udpAddress,udpPort);
    udp.printf(x);
    udp.printf(y);
    udp.printf(z);
    //udp.printf("Seconds since boot: %lu", millis()/1000);
    udp.endPacket();
    
    //Send Buttons
    udp.beginPacket(udpAddress,udpPort);
    if (digitalRead(WIO_KEY_C) == LOW) {
    udp.printf("True ");
    } else {
      udp.printf("False ");
    }
    if (digitalRead(WIO_KEY_B) == LOW) {
    udp.printf("True ");
    } else {
      udp.printf("False ");
    }
    if (digitalRead(WIO_KEY_A) == LOW) {
    udp.printf("True ");
    } else {
      udp.printf("False ");
    }
    udp.endPacket();
  }
  //Wait for 1 second
  //delay(500);
}

void connectToWiFi(const char * ssid, const char * pwd){
  Serial.println("Connecting to WiFi network: " + String(ssid));

  // delete old config
  WiFi.disconnect(true);
  //register event handler
  WiFi.onEvent(WiFiEvent);
  
  //Initiate connection
  WiFi.begin(ssid, pwd);

  Serial.println("Waiting for WIFI connection...");
}

//wifi event handler
void WiFiEvent(WiFiEvent_t event){
    switch(event) {
      case SYSTEM_EVENT_STA_GOT_IP:
          //When connected set 
          Serial.print("WiFi connected! IP address: ");
          Serial.println(WiFi.localIP());  
          //initializes the UDP state
          //This initializes the transfer buffer
          udp.begin(WiFi.localIP(),udpPort);
          connected = true;
          break;
      case SYSTEM_EVENT_STA_DISCONNECTED:
          Serial.println("WiFi lost connection");
          connected = false;
          break;
      default: break;
    }
}
