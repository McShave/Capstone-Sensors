// ESP32 with BME Sensor - MAC address: 3C:8A:1F:A4:65:8A

/***
  Read Our Complete Guide: https://RandomNerdTutorials.com/esp32-bme680-sensor-arduino/
  Designed specifically to work with the Adafruit BME680 Breakout ----> http://www.adafruit.com/products/3660 These sensors use I2C or SPI to communicate, 2 or 4 pins are required to interface. Adafruit invests time and resources providing this open source code, please support Adafruit and open-source hardware by purchasing products from Adafruit! Written by Limor Fried & Kevin Townsend for Adafruit Industries. BSD license, all text above must be included in any redistribution
***/

// SCL - D22
// SDA - D21
// PWR - 3.3v
// GND - GND


#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"

#include <BLEServer.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLECharacteristic.h>

/*#define BME_SCK 18
#define BME_MISO 19
#define BME_MOSI 23
#define BME_CS 5*/


#define bleServerName "ESP32_BME_Sensor"
#define SERVICE_UUID "9a8b5685-334b-46ed-bf2b-975113dcdd34"
#define temperatureCharacteristicUUID "d82312ea-1422-43c7-8931-408812a8f32b"
#define humidityCharacteristicUUID "b2106683-c2c3-47ab-a6ef-9ca7268a8b7b"
#define pressureCharacteristicUUID "12bba2b5-1097-437b-9eeb-826e2eb48f0a"
#define gasCharacteristicUUID "3bc5829d-5ea3-4182-99cf-566970062b9f"

BLEServer *pServer;
BLEService *pService;
BLECharacteristic *temperatureCharacteristic;
BLECharacteristic *humidityCharacteristic;
BLECharacteristic *pressureCharacteristic;
BLECharacteristic *gasCharacteristic;
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


#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME680 bme; // I2C
//Adafruit_BME680 bme(BME_CS); // hardware SPI
//Adafruit_BME680 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK);

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println(F("BME680 async test"));

  if (!bme.begin()) {
    Serial.println(F("Could not find a valid BME680 sensor, check wiring!"));
    while (1);
  }

  // Set up oversampling and filter initialization
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150); // 320*C for 150 ms

  
  // Create the BLE Device
  BLEDevice::init(bleServerName);

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  pService = pServer->createService(SERVICE_UUID);

  // Create BLE Characteristics and corresponding Descriptors
  temperatureCharacteristic = pService->createCharacteristic( 
                                temperatureCharacteristicUUID,
                                BLECharacteristic::PROPERTY_NOTIFY );
  BLEDescriptor temperatureDescriptor2901(BLEUUID((uint16_t)0x2901));
  BLEDescriptor temperatureDescriptor2902(BLEUUID((uint16_t)0x2902));
  temperatureDescriptor2901.setValue("Temperature");
  temperatureCharacteristic->addDescriptor(&temperatureDescriptor2901);
  temperatureCharacteristic->addDescriptor(&temperatureDescriptor2902);

  humidityCharacteristic = pService->createCharacteristic( 
                                humidityCharacteristicUUID,
                                BLECharacteristic::PROPERTY_NOTIFY );
  BLEDescriptor humidityDescriptor2901(BLEUUID((uint16_t)0x2901));
  BLEDescriptor humidityDescriptor2902(BLEUUID((uint16_t)0x2902));
  humidityDescriptor2901.setValue("Humidity level.");
  humidityCharacteristic->addDescriptor(&humidityDescriptor2901);
  humidityCharacteristic->addDescriptor(&humidityDescriptor2902);

  // pressureCharacteristic = pService->createCharacteristic( 
  //                               pressureCharacteristicUUID,
  //                               BLECharacteristic::PROPERTY_NOTIFY );
  // BLEDescriptor pressureDescriptor2901(BLEUUID((uint16_t)0x2901));
  // BLEDescriptor pressureDescriptor2902(BLEUUID((uint16_t)0x2902));
  // pressureDescriptor2901.setValue("Atmospheric Pressure");
  // pressureCharacteristic->addDescriptor(&pressureDescriptor2901);
  // pressureCharacteristic->addDescriptor(&pressureDescriptor2902);

  // gasCharacteristic = pService->createCharacteristic( 
  //                               gasCharacteristicUUID,
  //                               BLECharacteristic::PROPERTY_NOTIFY );
  // BLEDescriptor gasDescriptor2901(BLEUUID((uint16_t)0x2901));
  // BLEDescriptor gasDescriptor2902(BLEUUID((uint16_t)0x2902));
  // gasDescriptor2901.setValue("gas level number.");
  // gasCharacteristic->addDescriptor(&gasDescriptor2901);
  // gasCharacteristic->addDescriptor(&gasDescriptor2902);


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
  // Tell BME680 to begin measurement.
  unsigned long endTime = bme.beginReading();
  if (endTime == 0) {
    Serial.println(F("Failed to begin reading :("));
    return;
  }
  Serial.print(F("Reading started at "));
  Serial.print(millis());
  Serial.print(F(" and will finish at "));
  Serial.println(endTime);

  Serial.println(F("You can do other work during BME680 measurement."));
  delay(50); // This represents parallel work.
  // There's no need to delay() until millis() >= endTime: bme.endReading()
  // takes care of that. It's okay for parallel work to take longer than
  // BME680's measurement time.

  // Obtain measurement results from BME680. Note that this operation isn't
  // instantaneous even if milli() >= endTime due to I2C/SPI latency.
  if (!bme.endReading()) {
    Serial.println(F("Failed to complete reading :("));
    return;
  }
  Serial.print(F("Reading completed at "));
  Serial.println(millis());

  Serial.print(F("Temperature = "));
  Serial.print(bme.temperature);
  Serial.println(F(" *C"));

  Serial.print(F("Pressure = "));
  Serial.print(bme.pressure / 100.0);
  Serial.println(F(" hPa"));

  Serial.print(F("Humidity = "));
  Serial.print(bme.humidity);
  Serial.println(F(" %"));

  Serial.print(F("Gas = "));
  Serial.print(bme.gas_resistance / 1000.0);
  Serial.println(F(" KOhms"));

  Serial.print(F("Approx. Altitude = "));
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(F(" m"));

  temperatureCharacteristic->setValue(String(bme.temperature));
  humidityCharacteristic->setValue(String(bme.humidity));
  // pressureCharacteristic->setValue(String(bme.pressure));
  // gasCharacteristic->setValue(String(bme.gas_resistance));
  
  temperatureCharacteristic->notify();
  humidityCharacteristic->notify();
  // pressureCharacteristic->notify();
  // gasCharacteristic->notify();


  Serial.println();
  delay(2000);
}