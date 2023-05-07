#ifndef MY_ADVERTISED_DEVICE_CALLBACKS_H
#define MY_ADVERTISED_DEVICE_CALLBACKS_H

#define DEBUG   //If you comment this line, the DPRINT & DPRINTLN lines are defined as blank.
#ifdef DEBUG    //Macros are usually in all capital letters.
  #define DPRINT(...)    Serial.print(__VA_ARGS__)     //DPRINT is a macro, debug print
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__)   //DPRINTLN is a macro, debug print with new line
#else
  #define DPRINT(...)     //now defines a blank line
  #define DPRINTLN(...)   //now defines a blank line
#endif

#include "NimBLEDevice.h"

//Globals
#define bleServerName     "Greenhouse" //Server name
#define BLE_BUFFER_SIZE  500 // BLE payload size for pData
//Address of the peripheral device. Address will be found during scanning...
static BLEAddress* pServerAddress;
static BLEAdvertisedDevice* myDevice;
static BLEScanResults pScanResults;

// UUIDs of services
extern const char* ServiceName;

// The remote service we wish to connect to.
static BLEUUID ServiceUUID("2c9d0585-09f0-4e8f-9bf3-ddc13f9cd36d");
// The characteristics of the remote service we are interested in.
static BLEUUID    tempCharUUID("0a459982-c317-4bad-b234-b6c2ff69f055");
static BLEUUID    tempWarnCharUUID("ef0d5dfd-f466-435e-8bd9-b069f8e3a5db");
static BLEUUID    notifyDliCharUUID("137baafd-8dc6-46b5-985b-0bd88ee83f82");
static BLEUUID    cumDliUUID("befc0ce1-a854-46e4-a4b3-77db0d208998");
static BLEUUID    humidityCharUUID("2516cd1f-feee-457d-bc5b-3b6c7d7c37a3");
static BLEUUID    parCharUUID("cb2693fc-83fd-4f81-8ac3-4ab23e6d4797");

 
//Characteristics that we want to read
static BLERemoteCharacteristic* temperatureCharacteristic;
static BLERemoteCharacteristic* humidityCharacteristic;
static BLERemoteCharacteristic* cumulativeDliCharacteristic; // total daily light dosage in greenhouse
static BLERemoteCharacteristic* parCharacteristic; // current photon flux from light sensor


//Variables to store readings from characteristics
extern char* temperatureChar;
extern char* humidityChar;
extern char* dliChar;
extern char* parChar;

//Flags to check whether new readings are available
extern boolean newTemperature;
extern boolean newHumidity;
extern boolean newDli;
extern boolean newPar;

// House-keeping flags
extern boolean doConnect;
extern boolean connected;
extern boolean doScan;
//Activate notify
const uint8_t notificationOn[] = {0x1, 0x0};
const uint8_t notificationOff[] = {0x0, 0x0};

//Printing sensor values
bool printReadings(void *);

// Methods needed for BLE communication
bool manageConnection(void *);
bool connectToServer();
class MyAdvertisedDeviceCallbacks : public NimBLEAdvertisedDeviceCallbacks {
  public:
    void onResult(NimBLEAdvertisedDevice* advertisedDevice) override;
};
BLEScanResults scanForBLEserver(int duration);
void genericNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, 
                           uint8_t* pData, size_t length, bool isNotify,
                           char** charPtr, bool* newFlag);
static void temperatureNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify);
static void humidityNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify);
static void cumulativeDliNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify);
static void parNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify);

void printHexToASCII(uint8_t* pData, size_t length); // print HEX payloads to console for debugging
//void printBLEServerAddress(BLEAddress* address);

#endif  // MY_ADVERTISED_DEVICE_CALLBACKS_H end of include guard