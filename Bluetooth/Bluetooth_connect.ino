
#include <rpcBLEDevice.h>
#include <BLEServer.h>
#include"LIS3DHTR.h"


// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "180f"
#define CHARACTERISTIC_UUID_X "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_Y "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_Z "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
#define DESCRIPTOR_UUID     "4545"

LIS3DHTR<TwoWire> lis;
BLECharacteristic *xCharacteristic;
BLECharacteristic *yCharacteristic;
BLECharacteristic *zCharacteristic;

float x_values, y_values, z_values;
char val_x[8];
char val_y[8];
char val_z[8];

void setup() {
  Serial.begin(115200);
  //while(!Serial){};
  Serial.println("Starting BLE work!");

  lis.begin(Wire1);

  lis.setOutputDataRate(LIS3DHTR_DATARATE_25HZ); //Data output rate
  lis.setFullScaleRange(LIS3DHTR_RANGE_2G); //Scale range set to 2g
  
  BLEDevice::init("Long name works now");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  xCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID_X,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  xCharacteristic->setAccessPermissions(GATT_PERM_READ | GATT_PERM_WRITE);
  BLEDescriptor *pDescriptor = xCharacteristic->createDescriptor(
                                         DESCRIPTOR_UUID,
                                          ATTRIB_FLAG_VOID | ATTRIB_FLAG_ASCII_Z,
                                         GATT_PERM_READ | GATT_PERM_WRITE,
                                         2
                                         );
                                         
  yCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID_Y,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );                                       
  yCharacteristic->setAccessPermissions(GATT_PERM_READ | GATT_PERM_WRITE);  

                                         
  zCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID_Z,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  zCharacteristic->setAccessPermissions(GATT_PERM_READ | GATT_PERM_WRITE);  
                                      

  pService->start();

  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}



void loop() { 
  // put your main code here, to run repeatedly:

  x_values = lis.getAccelerationX();
  y_values = lis.getAccelerationY();
  z_values = lis.getAccelerationZ();

  dtostrf(x_values, 6, 2, val_x);
  dtostrf(y_values, 6, 2, val_y);
  dtostrf(z_values, 6, 2, val_z);
  
  xCharacteristic->setValue(val_x);  
  yCharacteristic->setValue(val_y);
  zCharacteristic->setValue(val_z);
  //Serial.println("1");
  //delay(50);
}
