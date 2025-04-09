/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-water-sensor
 */

#include <BLEServer.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLECharacteristic.h>

#define POWER_PIN  17 // ESP32 pin GPIO17 connected to sensor's VCC pin
#define SIGNAL_PIN 36 // ESP32 pin GPIO36 (ADC0) connected to sensor's signal pin
int value = 0; // variable to store the sensor value

#define bleServerName "ESP32_Water_Sensor"
#define SERVICE_UUID "4ecb82c7-60f9-405e-96d1-731f0e97c012"
#define waterCharacteristicUUID "f94f7e98-6e5e-4f44-9cfb-1e9f53a437f0"

BLEServer *pServer;
BLEService *pService;
BLECharacteristic *smokeCharacteristic;
BLEAdvertising *pAdvertising;

bool deviceConnected = false;

class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  };
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    pServer->startAdvertising();
  }
};


void setup() {
  Serial.begin(9600);
  // set the ADC attenuation to 11 dB (up to ~3.3V input)
  analogSetAttenuation(ADC_11db);
  pinMode(POWER_PIN, OUTPUT);   // configure pin as an OUTPUT
  digitalWrite(POWER_PIN, LOW); // turn the sensor OFF

  
  // Create the BLE Device
  BLEDevice::init(bleServerName);

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  pService = pServer->createService(SERVICE_UUID);

  // Create BLE Characteristics and corresponding Descriptors
  smokeCharacteristic = pService->createCharacteristic( 
                                waterCharacteristicUUID,
                                BLECharacteristic::PROPERTY_NOTIFY );
  BLEDescriptor waterDescriptor2901(BLEUUID((uint16_t)0x2901));
  BLEDescriptor waterDescriptor2902(BLEUUID((uint16_t)0x2902));
  waterDescriptor2901.setValue("Smoke level number.");
  waterCharacteristic->addDescriptor(&waterDescriptor2901);
  waterCharacteristic->addDescriptor(&waterDescriptor2902);

  pService->start();
  // Start advertising
  pAdvertising = BLEDevice::getAdvertising();
  
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  pServer->getAdvertising()->start();

  Serial.println("Characteristic defined!");

}

void loop() {
  digitalWrite(POWER_PIN, HIGH);  // turn the sensor ON
  delay(10);                      // wait 10 milliseconds
  value = analogRead(SIGNAL_PIN); // read the analog value from sensor
  digitalWrite(POWER_PIN, LOW);   // turn the sensor OFF

  Serial.print("The water sensor value: ");
  Serial.println(value);

  waterCharacteristic->setValue(String(value));
  waterCharacteristic->notify();
  
  delay(5000);
}