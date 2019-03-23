#include "Arduino.h"
#include <SoftwareSerial.h>

#define EnPin 9           // Drives enable pin of bluetooth module to configure the operating mode
#define BT_TX_PIN 12      // Pin on the board that the BT device connects to 
#define BT_RX_PIN 13
#define NUM_START_CMDS 13 // Defines the number of startup commands defined in our array of AT startup commands 
#define MAX_RESP_SIZE 15  // Defines the maximum size of the response to the AT command

extern SoftwareSerial BT_Master;  // RX/TX connects to BT TX/RX

void BTMasterSetup();
void BT_Pinout_Setup(void);
void PairToDevice(void);
