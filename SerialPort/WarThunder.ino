#include"LIS3DHTR.h"
LIS3DHTR<TwoWire> lis;

void setup() {

  
  Serial.begin(115200);
  lis.begin(Wire1);
 
  if (!lis) {
    Serial.println("ERROR");
    while(1);
  }
  lis.setOutputDataRate(LIS3DHTR_DATARATE_25HZ); //Data output rate
  lis.setFullScaleRange(LIS3DHTR_RANGE_2G); //Scale range set to 2g
}
void sendData(float num1, float num2, float num3) {
  char buffer[20];
  sprintf(buffer, "%.2f,%.2f,%.2f", num1, num2, num3);
  Serial.write(buffer);
}


void loop() {
  float x_values, y_values, z_values;
  x_values = lis.getAccelerationX();
  y_values = lis.getAccelerationY();
  z_values = lis.getAccelerationZ();

  
  sendData(x_values, y_values, z_values);
 


  delay(50);
}
