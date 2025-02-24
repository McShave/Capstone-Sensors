#include "esp_camera.h"
#include <WiFi.h>
#include <eloquent_esp32cam.h>
#include <eloquent_esp32cam/motion/detection.h>
#include <esp_wpa2.h>
extern void motion_detection_task(void *pvParameters);
extern void triggerCapture();

//
// WARNING!!! PSRAM IC required for UXGA resolution and high JPEG quality
//            Ensure ESP32 Wrover Module or other board with PSRAM is selected
//            Partial images will be transmitted if image exceeds buffer size
//
//            You must select partition scheme from the board menu that has at least 3MB APP space.
//            Face Recognition is DISABLED for ESP32 and ESP32-S2, because it takes up from 15 
//            seconds to process single frame. Face Detection is ENABLED if PSRAM is enabled as well

// ===================
// Select camera model
// ===================
//#define CAMERA_MODEL_WROVER_KIT // Has PSRAM
#define CAMERA_MODEL_ESP_EYE // Has PSRAM
//#define CAMERA_MODEL_ESP32S3_EYE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_PSRAM // Has PSRAM
//#define CAMERA_MODEL_M5STACK_V2_PSRAM // M5Camera version B Has PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE // Has PSRAM
//#define CAMERA_MODEL_M5STACK_ESP32CAM // No PSRAM
//#define CAMERA_MODEL_M5STACK_UNITCAM // No PSRAM
//#define CAMERA_MODEL_AI_THINKER // Has PSRAM
//#define CAMERA_MODEL_TTGO_T_JOURNAL // No PSRAM
//#define CAMERA_MODEL_XIAO_ESP32S3 // Has PSRAM
// ** Espressif Internal Boards **
//#define CAMERA_MODEL_ESP32_CAM_BOARD
//#define CAMERA_MODEL_ESP32S2_CAM_BOARD
//#define CAMERA_MODEL_ESP32S3_CAM_LCD
//#define CAMERA_MODEL_DFRobot_FireBeetle2_ESP32S3 // Has PSRAM
//#define CAMERA_MODEL_DFRobot_Romeo_ESP32S3 // Has PSRAM
#include "camera_pins.h"

// ===========================
// Enter your WiFi credentials
// ===========================
const char* ssid = "CU-Wireless"; // Change this to your network SSID
const char* username = ""; // Change this to your network username
const char* password = "";

void startCameraServer();
void setupLedFlash(int pin);

using eloq::camera;
using eloq::motion::detection;

void setup() {
    Serial.begin(115200);
    delay(3000);

    setupLedFlash(LED_GPIO_NUM);

    camera.pinout.aithinker(); 
    camera.resolution.qvga();   
    camera.quality.high();
    camera.brownout.disable();

    while (!camera.begin().isOk()) {
        Serial.println(camera.exception.toString());
        delay(500);
    }
    Serial.println("Camera ready");

    
    detection.stride(1);
    detection.threshold(5);
    detection.ratio(0.2);
    detection.rate.atMostOnceEvery(5).seconds();

    xTaskCreatePinnedToCore(motion_detection_task, "MotionDetection", 4096, NULL, 1, NULL, 1);

    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)username, strlen(username));
    esp_wifi_sta_wpa2_ent_set_username((uint8_t *)username, strlen(username));
    esp_wifi_sta_wpa2_ent_set_password((uint8_t *)password, strlen(password));
    esp_wifi_sta_wpa2_ent_enable();

    WiFi.begin(ssid);
    
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(1000);
    }

    Serial.println("\nConnected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // Start Camera Web Server
    startCameraServer();

    Serial.println("Camera server started.");
}



void loop() {
    // capture the current frame
    if (!camera.capture().isOk()) {
        Serial.println(camera.exception.toString());
        return;
    }

    // run the detection on the captured frame
    if (!detection.run().isOk()) {
        Serial.println(detection.exception.toString());
        return;
    }

    // check if motion was triggered
    if (detection.triggered()) {
        Serial.println("Motion detected!");
        // do something here (capture & save an image)
        triggerCapture();
    }
}


