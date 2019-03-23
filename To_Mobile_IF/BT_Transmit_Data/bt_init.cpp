// Board connections. NOTE: RX, TX and En pins can be reassigned to any other digital IO pins provided they meet the voltage requirements of the BT breakout board.
// GND -> GND, 5V -> 5V
// RX (HC-05) -> Pin11
// TX (HC-05) -> Pin10
// En (HC-05) -> Pin9
// Mode = 1 (Master)
// Name = MIST_BT_MASTER_1
// Addr = 98:D3:61:FD:6D:87
// Password: 1234 (by default)
// Pins on board from left to right (left has push button) - EN -> VCC -> GND -> TX -> RX 

// NOTE: Chip CANNOT connect with iOS devices 
// For now there are some issues with with scanning for devices, though if we know the bluetooth address we should be able to connect
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
 - AT+CMODE=1         (connect to any device, set to 0 for fixed addr, set to 1 for any addr)
 - AT+ADCN?           (display device count)
 - AT+STATE?          (determine module working state)
 - AT+INQ             (inquire for nearby bluetooth devices) (format of address is described below in RNAME command) 
 - AT+INQC            (cancel inquiry if needed)
 ======= The above commands are run automatically in BTMasterSetup(), the below the user will need to run to connect with their device========= 
 - AT+RNAME?<addr>    (check name of BT device) (if the BT address is AA:BB:CC:DD:EE:FF, syntax for addr is <AABB,CC,DDEEFF>)
 - AT+PAIR=<addr,timeout>     (pair to address)
 - AT+BIND=<addr>     (bind to bluetooth address)
 - AT+LINK=<addr>     (link to device)
 */
#include "bt_init.h"

// Note: If key pin is pulled high it is in AT mode, else if its low then its in data mode

// TODO: Write function to display list of nearby device names, user then can select device name for BT to connect 
// TODO: Integrate this code with any other of our IC code so ICs can directly communicate to BT device 
// Function to configure the bluetooth module as the master
// Refer to datasheets on drive for list of AT commands 
// NOTE: Still not 100% consistent on initialization 
SoftwareSerial BT_Master(BT_TX_PIN,BT_RX_PIN);

void BTMasterSetup(){
  BT_Master.begin(38400); // HC-05 default speed in AT command mode

  bool cmd_sent         = false;
  bool cmd_success      = false;
  int num_cmds_received = 0;

  String read_resp = "";
  
  String Startup_Full_Reset[] = {"AT","AT+ORGL","AT+NAME=MIST_BT_MASTER_1","AT+RMAAD","AT+CLASS=0","AT+ROLE=1",
                                 "AT+RESET","AT+INIT","AT+INQM=1,20,48","AT+CMODE=1","AT+PAIR=9465,2D,DB573D,50","AT+BIND=9465,2D,DB573D","AT+LINK=9465,2D,DB573D"};
  Serial.println("Starting initialization");

  delay(500); // Small delay before init
  
  while(1){
    if(!cmd_sent){    // If no commands have been sent out yet, try sending a cmd
      Serial.println(Startup_Full_Reset[num_cmds_received]); // Echo the command that was just sent
      delay(750);                                            // Give time for print command ISR to execute (TODO: test to see how far down we can bring this time)
      BT_Master.println(Startup_Full_Reset[num_cmds_received]); // Need to print with both NL & CR hence println 
      cmd_sent = true;   // Command sent
    }

    // BUG: Occasionally will hang and never print anything in the following look (i.e cmd sent but no resp for BT)
    if(cmd_sent == true){   // Only check for serial response if the command was sent
      if(BT_Master.available()){    // Some response was received from the bluetooth, parse it
        char inChar = BT_Master.read();  
        Serial.write(inChar); // Echo to terminal
       
        if(inChar != '\r' && inChar != '\n'){
          read_resp += inChar;
        }
        else{       // Received end of string
          while(BT_Master.available()){     // Resolve and flush the incoming buffer
            Serial.write(BT_Master.read());
          }
          
          if(read_resp == "OK"){  // If response was OK then AT command was successful, else need to resend the command 
            cmd_success = true;   
            read_resp = "";       // Clear the previous response
          }
          else{
            cmd_sent = false;     // TODO: Should also check which response failed and the type of error (i.e Init has a different error profile then CLASS/ROLE)
            read_resp = "";       // and re-init for example if fail
          }
        }
      }
    }
    
    if(cmd_success){    // Move to next commands and clear flags if command received successfully 
      num_cmds_received += 1;
      cmd_sent = false;
      cmd_success = false;
 
      if(num_cmds_received >= NUM_START_CMDS) // All cmds have been sent
        break;    
    }
  }

  Serial.println("Init Done");  
  delay(750);                  // Wait a bit and try to clear any existing buffers or commands 
  Serial.write("....");
}

// Setup pinouts
void BT_Pinout_Setup(void){
  pinMode(10, INPUT);     
  pinMode(11, OUTPUT);
  pinMode(EnPin, OUTPUT);
  digitalWrite(EnPin, HIGH);  // Pull enable to high
  Serial.begin(9600);         // Baudrate for serial monitor 
}

// Simple while loop to allow user to manually connect using AT commands to their device of choice, exits loop when connection has been made
// TODO: Need to account for other conditions if any of the commands fail or the wrong order is sent
// TODO: Just make this smarter overall
// BUG: will occassionally fail to leave this function even when all three commands have been sent and OK-ed 
void PairToDevice(void){
  // *ISSUE: here theres an issue where the response from all the previous AT commands are echo-ed, don't think it re-sends the AT commands
  //  however one of the responses is an error so maybe something is re-sent....
  
  String sent_to_serial= "";    // Track what the user is typing to the serial monitor
  String serial_response= "";   // Track response to user command
  char inChar;

  bool LinkCmdRcvd = false;     // Flag for if the link command was sent by the user
  bool PairCmdRcvd = false;     // Flag for if the pair command was sent by the user
  bool BindCmdRcvd = false;     // Flag for if the bind command was sent by the user

  while(1){ // Should probably have some timer here to force exit if connection isn't happening 
    // Read the output of the HC-05 and send to the serial monitor of the Arduino 
    if (BT_Master.available()){
      char BTChar = BT_Master.read();
      Serial.write(BTChar);
    
      if(BTChar != '\r' && BTChar != '\n')
        serial_response += BTChar;       // Keep track of the response from the BT module 
      else{   // Response received
        while(BT_Master.available())  
          Serial.write(BT_Master.read());   // Flush the incoming buffer

        if(serial_response == "OK"){  // OK response
          if(LinkCmdRcvd && PairCmdRcvd && BindCmdRcvd) // Done pairing
            break;
          else{            // OK was received for an intermediate. Clear our character buffers 
            serial_response = "";
            sent_to_serial = "";
          }
        }
        else{  // Fail or error response
          LinkCmdRcvd = false;  // usually will fail on the bind command
        }
      }
    }

    // Read what was typed on the serial monitor of the Arduino and send to the HC-05
    if (Serial.available()){
      inChar = Serial.read();
      BT_Master.write(inChar);
      sent_to_serial += inChar;
    }

    if(inChar == '='){    // Once we reach this character we can check for the command entered by the user
      // Better if we can also check for ordering 
      if(sent_to_serial == "AT+LINK=")   // Check for the link command
        LinkCmdRcvd = true;   // Set flag 
      else if(sent_to_serial == "AT+PAIR=")
        PairCmdRcvd = true;
      else if(sent_to_serial == "AT+BIND=")
        BindCmdRcvd = true;
    }
  }

  delay(250);
  Serial.println("Device Connection Established");  // Display status on Serial terminal
  delay(250);
}
