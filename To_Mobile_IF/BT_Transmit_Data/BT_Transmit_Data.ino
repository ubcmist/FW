#include "data_handler.h"

// Defines 
#define GSR_RDS_PER_DATA_PT 512

// Sensor pin definitions
const int GSR_Pin = A0;           // GSR on analog pin0
// Global variables
volatile int timer1_count = 0;    // Timer1 counter
Data_Handler data_handler;        // Data_Handler object

volatile uint32_t gsr_sum_raw = 0;    // Raw gsr summed value, prior to conversion to resistance
volatile uint16_t gsr_read_count = 0;  // Keeps count of GSR values read
volatile double gsr_avg = 0;          // Average of the gsr value
volatile bool gsr_data_rdy_flag = false;  // Tells data handler if data is good to go

int test_var;

void setup() {
  // Setup for bluetooth information
  BT_Pinout_Setup();     
  BTMasterSetup();
  //PairToDevice(); // currently unused, will connect to a fixed device
  // NOTE: Do we need to pull the enable pin down to load and repower the device for it to operate in data mode?? (even though it sends data just fine)
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

// Timer1 Interrupt Service Routine (1s)
ISR(TIMER1_COMPA_vect){
  timer1_count += 1;                      // Counter for the ISR due to limitations in its minimum frequency 
  //Serial.println("DEBUG: IN ISR");        // For Debug
  BT_Master.println("DEBUG: IN ISR");     // For Debug
  if(timer1_count == TIMER_COUNTER_MAX){  // At the 5s timer we execute the ISR
    //BT_Master.println("SEND DATA");       // For Debug
    data_handler.Update_Data(gsr_avg,GSR_Data_ID);
    data_handler.Update_Data(test_var,HR_Data_ID);
    Serial.println(gsr_avg);
    // Reset variables and flags
    timer1_count = 0;
    gsr_data_rdy_flag = false;
  }
  else                                    // Else immediately exit
    return;
}

// Timer2 Interupt Service Routine (9ms)
ISR(TIMER2_COMPA_vect){
  if(!gsr_data_rdy_flag){ 
    gsr_sum_raw += analogRead(GSR_Pin);     // Read the GSR pin and add to sum
    gsr_read_count += 1;
    if(gsr_read_count == GSR_RDS_PER_DATA_PT){  // Check if we read 512 numbers
      gsr_data_rdy_flag = true;                 // Set flag
      gsr_avg = (double)(gsr_sum_raw >> 9);     // Divide by 512 (avg) then cast 
      gsr_sum_raw = 0;                          // Reset sum 
      gsr_read_count = 0;
    }
  }
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

  // Set timer2 interrupt at 9ms (since we want to take about 512 readings over the 5 secons)
  TCCR2A = 0;// set entire TCCR2A register to 0
  TCCR2B = 0;// same for TCCR2B
  TCNT2  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR2A = 139;// = (16*10^6) / (139*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR2B |= (1 << WGM12);
  // Set CS10 and CS12 bits for 1024 prescaler
  TCCR2B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE1A);
  
  sei(); // allow interrupts
}

