/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-door-sensor
 */

#define DOOR_SENSOR_PIN  35  // ESP32 pin GPIO19 connected to door sensor's pin
#include <BLEServer.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLECharacteristic.h>

int doorState;


#define bleServerName "ESP32_Smoke_Sensor"
#define SERVICE_UUID "d9ef7f1a-9f13-4762-ad46-0ca444dc0733"
#define smokeCharacteristicUUID "9f8230a6-3921-427a-99de-d675cf1352aa"

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
  Serial.begin(9600);                     // initialize serial
  pinMode(DOOR_SENSOR_PIN, INPUT_PULLUP); // set ESP32 pin to input pull-up mode
  
  // Create the BLE Device
  BLEDevice::init(bleServerName);

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  pService = pServer->createService(SERVICE_UUID);

  // Create BLE Characteristics and corresponding Descriptors
  smokeCharacteristic = pService->createCharacteristic( 
                                smokeCharacteristicUUID,
                                BLECharacteristic::PROPERTY_READ ||
                                BLECharacteristic::PROPERTY_WRITE ||
                                BLECharacteristic::PROPERTY_NOTIFY );
  BLEDescriptor smokeDescriptor(BLEUUID((uint16_t)0x2901));
  smokeDescriptor.setValue("Smoke level number.");
  smokeCharacteristic->addDescriptor(&smokeDescriptor);

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
  doorState = digitalRead(DOOR_SENSOR_PIN); // read state

  if (doorState == HIGH) {
    Serial.println("The door is open");
  } else {
    Serial.println("The door is closed");
  }
  delay(1000);
}
