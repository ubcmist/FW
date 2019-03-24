#ifndef DATA_HANDLER_INCLUDED
#define DATA_HANDLER_INCLUDED

#define TIMER_COUNTER_MAX 5 // Timer1 has a minimum frequency, hence we need this 

#include "bt_init.h"

// Identifiers for our various datatypes
#define GSR_Data_ID "GSR"	// Galvanic Skin Resistance datatype
#define HR_Data_ID "HeartRate"	// Heart Rate datatype

#define NUM_DATA_PER_PACKET 6 // Data processed once per 5 second, packets sent every minute

// Error Codes
enum error_code{
	DATA_OKAY = 0x0,				// Data is fine 
	DATA_MALFORMED = 0x1,			// At least one or more readings are malformed or error-ed, NOT due to disconnection
	DATA_INITIALIZING = 0x2,		// I.C might still be initializing, ignore data (UNSURE IF NEEDED)
	DEVICE_DISCONNECTED = 0xF		// Suspecting that the device is disconnected due to an erroneous value
};

typedef struct{
	String data_identifier;					
	uint8_t current_index;		
	int data[NUM_DATA_PER_PACKET];	// Array holding all readings for the packet (maybe consider using a vector)
	int timestamp = TIMER_COUNTER_MAX;					// Timestamp (TODO: figure out what Antoine wants for this)
	bool ok_to_send;				    // Defines if the packet is fully formed 
	error_code data_status;  		// Error status 
} datatype_packet;

class Data_Handler{
	public:
		Data_Handler();	// Default constructor
		~Data_Handler();	// Default destructor

		void Update_Data(int data, String data_identifier);	// Adds to datatype when new data arrives
	private:
		datatype_packet gsr_packet;	// GSR data packet
		datatype_packet hr_packet;	// HR data packet

		error_code Format_Error_Code(String data_identifier, int data);	// Performs some rudimentary data validation, formats the error code 

    // Function to send the formatted packet 
    void Send_Packet(datatype_packet &packet);
		
		// For now keep two functions for each separate datatype in case we want to handle things differently.
    void Append_Data(int data, datatype_packet &packet);
		//void Append_Data_GSR(int data);
		//void Append_Data_HR(int data);
};

#endif 
