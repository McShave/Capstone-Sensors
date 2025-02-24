#include <DHT.h>
#include <BLEServer.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLECharacteristic.h>

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


DHT dht(26, DHT11);
void setup() {
  // put your setup code here, to run once:
  dht.begin();
  delay(2000);

  Serial.begin(115200);

  
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
  // put your main code here, to run repeatedly:
  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();

  Serial.print("temp: ");
  Serial.println(temp);
  
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print("\n");

  delay(2000);
}
