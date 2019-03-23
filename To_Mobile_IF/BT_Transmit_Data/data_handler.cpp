#include "data_handler.h"

// Default constructor
// Designate our data packets with their identifier
Data_Handler::Data_Handler(){
	gsr_packet.data_identifier = GSR_Data_ID;
	gsr_packet.ok_to_send = false;
	hr_packet.data_identifier = HR_Data_ID; 
	hr_packet.ok_to_send = false;
}

// Default destructor
Data_Handler::~Data_Handler(){
}

// Public function to update one of the existing data packet types with new data 
void Data_Handler::Update_Data(int data, String data_identifier){
  //BT_Master.println("IN FUNC"); // For debug
  // TODO: if we have a mismatching type, then we still need to fill out the data, but with an error 
	if(data_identifier == GSR_Data_ID){
			Append_Data(data, gsr_packet);
	}
	if(data_identifier == HR_Data_ID){
      Append_Data(data, hr_packet);
	}

	return;
}

// Private function to append data to one of our data packets
void Data_Handler::Append_Data(int data, datatype_packet &packet){
	if (packet.current_index >= NUM_DATA_PER_PACKET){	// Overflowed. (TODO: routine to avert stack corruption)
    Serial.println("Overflow Issue");     // For debug 
		return;							// Currently just returns
	}
 
	packet.data[packet.current_index] = data;	// Replace data
	
	if(packet.current_index == (NUM_DATA_PER_PACKET - 1)){
		packet.ok_to_send = true;				// Packet is done formatting 
	}
	packet.current_index += 1;

	packet.data_status = Format_Error_Code(GSR_Data_ID, data);

  if(packet.ok_to_send){
    Send_Packet(packet);
  }
  
	return;
}

/*
void Data_Handler::Append_Data_HR(int data){
	if (hr_packet.current_index >= NUM_DATA_PER_PACKET)	// Overflowed. (TODO: routine to avert stack corruption)
		return;							// Currently just returns

	hr_packet.data[hr_packet.current_index] = data;	// Replace data
	
	if(hr_packet.current_index == (NUM_DATA_PER_PACKET - 1)){
		hr_packet.ok_to_send = true;				// Packet is done formatting 
	}
	hr_packet.current_index += 1;

	hr_packet.data_status = Format_Error_Code(HR_Data_ID, data);

  if(hr_packet.ok_to_send){
    Send_Packet(hr_packet);
  }
  
	return;
} */

// Private function for data validation of received data
// Currently just returns OKAY 
// TODO: Actual data handling and validation 
error_code Data_Handler::Format_Error_Code(String data_identifier, int data){
	error_code data_status;

	data_status = DATA_OKAY;
	return data_status;
}

// Private function to transmit packets
// TODO: Rewrite this, its pretty ugly...
void Data_Handler::Send_Packet(datatype_packet &packet){
  BT_Master.print(packet.data_identifier);
  BT_Master.print(",");
  BT_Master.print(packet.timestamp);      // Timestamp 
  for(int i = 0; i< NUM_DATA_PER_PACKET; i++){
    BT_Master.print(",");
    BT_Master.print(packet.data[i]);
  }
  BT_Master.println("");

  // Reset some variables
  packet.ok_to_send = false;
  packet.current_index = 0;
}

