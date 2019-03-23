#include "data_handler.h"

// Global variables
volatile int timer1_count = 0;    // Timer1 counter
Data_Handler data_handler;        // Data_Handler object

int test_var;

void setup() {
  // Setup for bluetooth information
  BT_Pinout_Setup();     
  BTMasterSetup();
  //PairToDevice(); // currently unused, will connect to a fixed device
  // Your setup goes here. **DO NOT MODIFY THE BAUD RATE FOR THE BLUETOOTH MONITOR OR CHANGE PINOUTS**
  Init_Interrupts();
}

// Program will run main loop (or main sensor code) once Bluetooth connection with between the slave and master device has been established
// and setup for the IC code is done

// BUG: Enters main loop, however trying to send any data to the BT device fails at this point....
void loop() {
  // Your code goes here.....
  // call BT_Master.write() or BT_Master.println() to send your data over bluetooth 
  // Example code will send commands over terminal to BT

  // Keep reading from HC-05 and send to Arduino Serial Monitor
  //if (BT_Master.available())
    //Serial.write(BT_Master.read());

  // Keep reading from Arduino Serial Monitor and send to HC-05
  //if (Serial.available())
    //BT_Master.write(Serial.read());
  test_var=random(500);
}

// Timer1 Interrupt Service Routine
ISR(TIMER1_COMPA_vect){
  timer1_count += 1;                      // Counter for the ISR due to limitations in its minimum frequency 
  //Serial.println("DEBUG: IN ISR");        // For Debug
  BT_Master.println("DEBUG: IN ISR");     // For Debug
  if(timer1_count == TIMER_COUNTER_MAX){  // At the 5s timer we execute the ISR
    //BT_Master.println("SEND DATA");       // For Debug
    data_handler.Update_Data(test_var,GSR_Data_ID);
    data_handler.Update_Data(test_var,HR_Data_ID);
    timer1_count = 0;
  }
  else                                    // Else immediately exit
    return;
}

// Formats timers and interrupts
// Currently only using timer1 interrups formatted at 1Hz 
void Init_Interrupts(){
  cli(); // stop interrupts

  // Set timer1 interrupt at 1Hz (we are somewhat capped at the minimum low frequency we can set)
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  
  sei(); // allow interrupts
}

