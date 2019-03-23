#include "Arduino.h"
#include <SoftwareSerial.h>

#define EnPin 9           // Drives enable pin of bluetooth module to configure the operating mode
#define NUM_START_CMDS 10 // Defines the number of startup commands defined in our array of AT startup commands 
#define MAX_RESP_SIZE 15  // Defines the maximum size of the response to the AT command

SoftwareSerial BT_Master(10,11);  // RX/TX

void BTMasterSetup()
void BT_Pinout_Setup(void)
void PairToDevice(void)
