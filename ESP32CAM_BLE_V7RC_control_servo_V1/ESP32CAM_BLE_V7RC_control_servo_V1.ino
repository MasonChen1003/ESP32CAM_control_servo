// Author : Mason, masonchen1003@gmail.com
// FB : https://www.facebook.com/mason.chen.1420
// Using ESP32CAM to control 4 servo. 

// Reference : ESP32 BLE example 
// Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp

// V7RC protocol setting 
// Company: V7 IDEA TECHNOLOGY LTD.
// Author: Louis Chuang

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "soc/soc.h" //disable brownout problems
#include "soc/rtc_cntl_reg.h"  //disable brownout problems
#include <EEPROM.h>
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "driver/rtc_io.h"

BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;
bool run_status = true; 

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

String receive_data = ""; 

void servo_angle(int channel, int angle)
{
 // regarding the datasheet of sg90 servo, pwm period is 20 ms and duty is 1->2ms
  int SERVO_RESOLUTION = 16;
  float range = (pow(2,SERVO_RESOLUTION)-1)/10; 
  float minDuty = (pow(2,SERVO_RESOLUTION)-1)/40; 
  
  uint32_t duty = (range)*(float)angle/180.0 + minDuty;
  ledcWrite(channel, duty);
  delay(30);
}

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();
         receive_data =""; 
        for (int i = 0; i < rxValue.length(); i++) {
           receive_data = receive_data + rxValue[i]; }
         Serial.println(receive_data);
   }
};

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
    Serial.begin(115200);
 
   ledcSetup(4, 50, 16);
   ledcAttachPin(12, 4);
   ledcSetup(5, 50, 16);
   ledcAttachPin(13, 5);
   ledcSetup(6, 50, 16);
   ledcAttachPin(15, 6);
   ledcSetup(7, 50, 16);
   ledcAttachPin(14, 7);

  servo_angle(4, 90);   
  servo_angle(5, 90);   
  servo_angle(6, 90);   
  servo_angle(7, 90);   

  // Create the BLE Device
  BLEDevice::init("");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
                    CHARACTERISTIC_UUID_TX,
                    BLECharacteristic::PROPERTY_NOTIFY
                  );
                      
  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
                       CHARACTERISTIC_UUID_RX,
                      BLECharacteristic::PROPERTY_WRITE
                    );

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  // https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/BLEAdvertising.cpp
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
 
  BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
  BLEUUID uuid("6E400001");
  //Device Name
  String device_name = "ESP_servo";
  oAdvertisementData.setName(device_name.c_str());
  pAdvertising->setAdvertisementData(oAdvertisementData);
 
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06); 
  pAdvertising->setMinPreferred(0x12);

   pAdvertising->start();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void loop() {  
  
    if (deviceConnected) {
        pTxCharacteristic->setValue(&txValue, 1);
        pTxCharacteristic->notify();
        txValue++;
    delay(10); // bluetooth stack will go into congestion, if too many packets are sent
  }

    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
         BLEDevice::startAdvertising(); // restart advertising
        
    //    Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }

     // decode V7RC protocol // 
       if (receive_data.startsWith("SRT")&&(receive_data.length()==20)){
          run_status = false; 
          int ch1_data=(receive_data.substring(3,7)).toInt(); 
          int ch2_data=(receive_data.substring(7,11)).toInt();
          int ch3_data=(receive_data.substring(11,15)).toInt(); 
          int ch4_data=(receive_data.substring(15,19)).toInt();
          receive_data ="";
          int angle_1 = floor(map(ch1_data, 1000, 2000, 0, 180));
          int angle_2 = floor(map(ch2_data, 1000, 2000, 0, 180));
          int angle_3 = floor(map(ch3_data, 1000, 2000, 0, 180));
          int angle_4 = floor(map(ch4_data, 1000, 2000, 0, 180));
          servo_angle(4, (angle_1));   
          servo_angle(5, (angle_2));   
          servo_angle(6, (angle_3));   
          servo_angle(7, (angle_4));   
     }
     //      Serial.println(receive_data);
    delay(10);
}
