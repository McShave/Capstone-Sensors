/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-motion-sensor
 */
#include <BLEServer.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLECharacteristic.h>

const int PIN_TO_SENSOR = 19; // GPIO19 pin connected to OUTPUT pin of sensor
int pinStateCurrent   = LOW;  // current state of pin
int pinStatePrevious  = LOW;  // previous state of pin


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
  Serial.begin(9600);            // initialize serial
  pinMode(PIN_TO_SENSOR, INPUT); // set ESP32 pin to input mode to read value from OUTPUT pin of sensor

  
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
  pinStatePrevious = pinStateCurrent; // store old state
  pinStateCurrent = digitalRead(PIN_TO_SENSOR);   // read new state

  if (pinStatePrevious == LOW && pinStateCurrent == HIGH) {   // pin state change: LOW -> HIGH
    Serial.println("Motion detected!");
    // TODO: turn on alarm, light or activate a device ... here
  }
  else
  if (pinStatePrevious == HIGH && pinStateCurrent == LOW) {   // pin state change: HIGH -> LOW
    Serial.println("Motion stopped!");
    // TODO: turn off alarm, light or deactivate a device ... here
  }
}
