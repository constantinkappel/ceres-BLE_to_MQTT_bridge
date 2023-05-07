#include "espClientBLE.h"
#include <Wire.h>

const char* ServiceName = "greenhouseSensorService";
char* temperatureChar = (char*)"0";
char* humidityChar = (char*)"0";
char* dliChar = (char*)"0";
char* parChar = (char*)"0";
//Flags to check whether new readings are available
boolean newTemperature = false;
boolean newHumidity = false;
boolean newDli = false;
boolean newPar = false;


// Handling scanning, connection and reconnection to server
BLEScanResults scanForBLEserver(int duration) {
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  return pBLEScan->start(duration, false);  
}


bool manageConnection(void *) {
  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
      //Activate the Notify property of each Characteristic
      /*
      temperatureCharacteristic->getDescriptor(tempCharUUID)->writeValue((uint8_t*)notificationOn, 2, true);
      humidityCharacteristic->getDescriptor(humidityCharUUID)->writeValue((uint8_t*)notificationOn, 2, true);
      cumulativeDliCharacteristic->getDescriptor(cumDliUUID)->writeValue((uint8_t*)notificationOn, 2, true);
      parCharacteristic->getDescriptor(parCharUUID)->writeValue((uint8_t*)notificationOn, 2, true);*/
      connected = true;
    } else {
      doScan = true;
      pScanResults = scanForBLEserver(0);
      Serial.print("Scan Results: "); Serial.println(pScanResults.getCount());
      Serial.println("We have failed to connect to the server; Restart your device to scan for nearby BLE server again.");
    }
    doConnect = false;
  }
  if (connected) {    
    // Set the characteristic's value to be the array of bytes that is actually a string.
    // Note: write / read value now returns true if successful, false otherwise - try again or disconnect
    //pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
  } else if(doScan){
    pScanResults = scanForBLEserver(0);  // this is just eample to start scan after disconnect, most likely there is better way to do it in arduino
    Serial.print("Scan Results: "); Serial.println(pScanResults.getCount());
  }
  return true;
} // manageConnection()

bool connectToServer() {
  //Connect to the BLE Server that has the name, Service, and Characteristics
  DPRINTLN("Creating pClient");
  BLEClient* pClient = BLEDevice::createClient();
  Serial.print("Address of myDevice "); Serial.println(myDevice->getAddress().toString().c_str());
  if (myDevice != nullptr) {
    pClient->connect(myDevice); // overloaded connect() method can get address from myDevice
  } else {
    return false;
  }
  Serial.println(" - Connected to server");
 
  // Obtain a reference to the service we are after in the remote BLE server.
  if (pClient == nullptr) {
    return false;
  }
  BLERemoteService* pRemoteService = pClient->getService(ServiceUUID);
  
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(ServiceUUID.toString().c_str());
    return (false);
  }
 
  // Obtain a reference to the characteristics in the service of the remote BLE server.
  temperatureCharacteristic = pRemoteService->getCharacteristic(tempCharUUID);
  humidityCharacteristic = pRemoteService->getCharacteristic(humidityCharUUID);
  cumulativeDliCharacteristic = pRemoteService->getCharacteristic(cumDliUUID);
  parCharacteristic = pRemoteService->getCharacteristic(parCharUUID);

  if (temperatureCharacteristic == nullptr || humidityCharacteristic == nullptr || cumulativeDliCharacteristic == nullptr || parCharacteristic == nullptr) {
    Serial.print("Failed to find our characteristic UUID");
    return false;
  }
  Serial.println(" - Found our characteristics");
 
  //Assign callback functions for the Characteristics
  temperatureCharacteristic->subscribe(true, temperatureNotifyCallback);
  humidityCharacteristic->subscribe(true, humidityNotifyCallback);
  cumulativeDliCharacteristic->subscribe(true, cumulativeDliNotifyCallback);
  parCharacteristic->subscribe(true, parNotifyCallback);
  return true;
} // connectToServer

//Callback function that gets called, when another device's advertisement has been received
void MyAdvertisedDeviceCallbacks::onResult(NimBLEAdvertisedDevice* advertisedDevice) {
  if (advertisedDevice->getName() == bleServerName) { //Check if the name of the advertiser matches
    Serial.print("BLE server name: "); Serial.println(advertisedDevice->getName().c_str());
    advertisedDevice->getScan()->stop(); //Scan can be stopped, we found what we are looking for
    myDevice = advertisedDevice; // This is the device we've been looking for 
    pServerAddress = new BLEAddress(advertisedDevice->getAddress()); //Address of advertiser is the one we need
    doConnect = true; //Set indicator, stating that we are ready to connect
    Serial.print("Device found. Connecting to device at address ");
    //printBLEServerAddress(pServerAddress);
    Serial.println(pServerAddress->toString().c_str());
  }
}




// Defining Callbacks with a generic mechanism to read sensor values from BLE
void genericNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, 
                           uint8_t* pData, size_t length, bool isNotify,
                           char** charPtr, bool* newFlag) {
  // print the payload and return if the length is wrong
  if (!pData || length > BLE_BUFFER_SIZE) {
    // Invalid data, ignore it, but print payload for debugging
    printHexToASCII(pData, length);
    return;
  }
  //allocate memory for value
  char* newValueChar = new char[length+1];
  memcpy(newValueChar, pData, length);
  newValueChar[length] = '\0'; //add null terminator
  *newFlag = true;
  *charPtr = newValueChar;
  // TODO: This was meant to free memory and avoid memory leaks, but is causing heap assertion errors. 
  /*
  const int ROWS = sizeof(charPtr) / sizeof(char*);
  for (int i = 0; i < ROWS; i++) {
    if (charPtr[i] != nullptr) {
      DPRINTLN("///////// Freeing memory in generic Callback /////////");
      delete[] charPtr[i];
    }
    DPRINTLN("///////// Assigning new values to charPtr /////////");
    charPtr[i] = newValueChar;
  }*/
  //DPRINTLN("///////// Generic Callback done. //////////");
}

// Read values from BLE through callbacks
static void temperatureNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, 
                                       uint8_t* pData, size_t length, bool isNotify) {
  //DPRINTLN("temperatureNotifyCallback was called.");
  genericNotifyCallback(pBLERemoteCharacteristic, pData, length, isNotify, &temperatureChar, &newTemperature);
}

static void humidityNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, 
                                    uint8_t* pData, size_t length, bool isNotify) {
  genericNotifyCallback(pBLERemoteCharacteristic, pData, length, isNotify, &humidityChar, &newHumidity);
}

static void cumulativeDliNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, 
                                    uint8_t* pData, size_t length, bool isNotify) {
  genericNotifyCallback(pBLERemoteCharacteristic, pData, length, isNotify, &dliChar, &newDli);
}

static void parNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, 
                                    uint8_t* pData, size_t length, bool isNotify) {
  genericNotifyCallback(pBLERemoteCharacteristic, pData, length, isNotify, &parChar, &newPar);
}


// print sensor readings to console
bool printReadings(void *) {
    Serial.print("Temperature [°C]: ");
    Serial.println(temperatureChar);
    Serial.print("Humidity [%]: ");
    Serial.println(humidityChar);
    Serial.print("DLI [mol/(m²*day)]: ");
    Serial.println(dliChar);
    Serial.print("PAR [µmol/(m²*s)]: ");
    Serial.println(parChar);
    return true; 
}

// print HEX data to console and convert to Unicode
void printHexToASCII(uint8_t* pData, size_t length) {
  char hexString[3];
  for (size_t i = 0; i < length; i++) {
    sprintf(hexString, "%02X", pData[i]);
    uint32_t hexValue;
    sscanf(hexString, "%x", &hexValue);
    Serial.print((char)hexValue);
  }
}




