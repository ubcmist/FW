#include <SoftwareSerial.h>

// Board connections
// RX (HC-05) -> Pin11
// TX (HC-05) -> Pin10
// En (HC-05) -> Pin9
// BT module current configurations
// Mode = 1 (Master)
// Name = MIST_BT_MASTER_1
// Addr = 98:D3:61:FD:6D:87

/*
AT Command procedure:
 - AT                 (check response)
 - AT+ORGL            (factory reset)
 - AT+NAME=<set_name> (set name of device)
 - AT+RMAAD           (remove previous pair conditions)
 - AT+CLASS=0         (set class to 0)
 - AT+ROLE=1          (set role to master)
 - AT+RESET           (optional, but preferred for initial setup)
 - AT+INIT            (initialize SPP profile lib, will return error(17) if already init)
 - AT+PSWD=<pswd>     (set password if needed)
 - AT+INQM=<param1,param2,param3> (set param1 to 1, param2 = number of devices to respond to, param3 = timeout [from 1-48])
 - AT+CMODE=1         (connect to any device, set to 0 for fixed addr)
 - AT+ADCN?           (display device count)
 - AT+STATE?          (determine module working state)
 - AT+INQ             (inquire for nearby bluetooth devices)
 - AT+INQC            (cancel inquiry if needed)
 - AT+RNAME?<addr>    (check name of BT device)
 - AT+BIND=<addr>     (bind to bluetooth address)
 - AT+PAIR=<addr>     (pair to address)
 - AT+LINK=<addr>     (link to device)
 */
#define EnPin 9          // Drives enable pin of bluetooth module to configure the operating mode
#define NUM_START_CMDS 9 // Defines the number of startup commands defined in our array of AT startup commands 
#define MAX_RESP_SIZE 15 // Defines the maximum size of the response to the AT command

SoftwareSerial BT_Master(10,11);  // RX/TX

// Note: If key pin is pulled high it is in AT mode, else if its low then its in data mode

void setup() {
  Pinout_Setup();     
  BTMasterSetup();
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

  bool cmd_sent         = false;
  bool cmd_success      = false;
  int num_cmds_received = 0;

  String read_resp = "";
  
  String Startup_Full_Reset[] = {"AT","AT+ORGL","AT+NAME=MIST_BT_MASTER_1","AT+RMAAD","AT+CLASS=0","AT+ROLE=1"
                                 "AT+RESET","AT+INIT","AT+INQM=1,20,48"};
  Serial.println("Starting initialization");
  
  while(num_cmds_received != NUM_START_CMDS){
    if(Serial.available() && !cmd_sent){
      BT_Master.println(Startup_Full_Reset[num_cmds_received]);
      cmd_sent = true;   // Command sent
    }

    if(cmd_sent == true){   // Only check for serial response if the command was sent
      if(BT_Master.available()){   
        char inChar = BT_Master.read();  
        
        if(inChar != '\r')
          read_resp += inChar;
        else{       // Received end of string
          Serial.println(read_resp);
          
          if(read_resp == "OK"){  // If response was OK then AT command was successful, else need to resend the command 
            cmd_success = true;   
            read_resp = "";
          }
          else
            cmd_sent = false;     // TODO: Should also check which response failed and the type of error (i.e Init has a different error profile then CLASS/ROLE)
        }
      }
    }

    if(cmd_success){    // Move to next commands and clear flags if command received successfully 
      Serial.println(Startup_Full_Reset[num_cmds_received]); // Echo the command that was just sent and received
      
      num_cmds_received += 1;
      cmd_sent = false;
      cmd_success = false;
    }
  }

  Serial.println("Init Done");
}

// Setup pinouts
void Pinout_Setup(void){
  pinMode(10, INPUT);     
  pinMode(11, OUTPUT);
  pinMode(EnPin, OUTPUT);
  digitalWrite(EnPin, HIGH);  // Pull enable to high
  Serial.begin(9600);         // Baudrate TBD
  Serial.println("Test AT commands:");
}

