#include <SoftwareSerial.h>

// Board connections
// RX (HC-05) -> Pin11
// TX (HC-05) -> Pin10
// En (HC-05) -> Pin9

#define EnPin 9      // Drives enable pin of bluetooth module to configure the operating mode
SoftwareSerial BT_Master(10,11);

// Note: If key pin is pulled high it is in AT mode, else if its low then its in data mode

void setup() {
  // put your setup code here, to run once:
  pinMode(RxD, INPUT);
  pinMode(TxD, OUTPUT);
  pinMode(EnPin, OUTPUT);
  digitalWrite(EnPin, HIGH);  // Pull enable to high
  Serial.begin(9600); // Baudrate TBD
  Serial.println("Test AT commands:");
  BTMasterSetup();
  //BTMasterSetup();
}

void loop() {
  // put your main code here, to run repeatedly:
  // Read the output of the HC-05 and send to the serial monitor of the Arduino 
  if (BT_Master.available()){
    Serial.write(BT_Master.read());
  }

  // Read what was typed on the serial monitor of the Arduino and send to the HC-05
  if (Serial.available()){
    BT_Master.write(Serial.read());
  }
}

// Function to configure the bluetooth module as the master
// Refer to datasheets on drive for list of AT commands 
void BTMasterSetup(){
  BT_Master.begin(38400); // HC-05 default speed in AT command mode
}

