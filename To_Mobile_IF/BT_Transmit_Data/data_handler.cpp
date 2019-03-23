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
void Data_Handler::Update_Data(int data, int data_identifier){
  BT_Master.println("IN FUNC");
	switch(data_identifier){
		case GSR_Data_ID:
			Append_Data_GSR(data);
			break;
		case HR_Data_ID:
			Append_Data_HR(data);
		default: break;
	}

	return;
}

// Private function to append data to one of our data packets
void Data_Handler::Append_Data_GSR(int data){
	if (gsr_packet.current_index >= NUM_DATA_PER_PACKET)	// Overflowed. (TODO: routine to avert stack corruption)
		return;							// Currently just returns

	gsr_packet.data[gsr_packet.current_index] = data;	// Replace data
	
	if(gsr_packet.current_index == (NUM_DATA_PER_PACKET - 1)){
		gsr_packet.ok_to_send = true;				// Packet is done formattin 
	}
	gsr_packet.current_index += 1;

	gsr_packet.data_status = Format_Error_Code(GSR_Data_ID, data);


  if(gsr_packet.ok_to_send){
    Send_Packet(gsr_packet);
  }
  
	return;
}

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
}

// Private function for data validation of received data
// Currently just returns OKAY 
error_code Data_Handler::Format_Error_Code(int data_identifier, int data){
	error_code data_status;

	data_status = DATA_OKAY;
	return data_status;
}

// Private function to transmit packets
// TODO: Rewrite this, its pretty ugly...
void Data_Handler::Send_Packet(datatype_packet &packet){
  BT_Master.print(packet.data_identifier);
  BT_Master.print(",");
  BT_Master.print(packet.data_status);
  BT_Master.print(",");
  for(int i = 0; i< NUM_DATA_PER_PACKET; i++){
    BT_Master.print(packet.data[i]);
    BT_Master.print(",");
  }
  BT_Master.print("");

  // Reset some variables
  packet.ok_to_send = false;
  packet.current_index = 0;
}

