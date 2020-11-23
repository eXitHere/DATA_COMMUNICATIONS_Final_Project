//https://github.com/dwblair/arduino_crc8 DOWNLOAD THIS LIB FOR CRC8

#include <Arduino.h>
#include "protocol_lib.h"

ProtocolControl::ProtocolControl(String srcName, String destName, float freq)
{
	this->srcName = srcName;
	this->destName = destName;
	this->STARTFLAG = "o";
	this->allReceiving = "";
	this->ackNo = "0";
	this->rx = new FM_RX(freq);
	this->tx = new FM_TX();
	this->crc8 = new CRC8();
	Serial.println("Init completed");
}
ProtocolControl::~ProtocolControl()
{
}

String ProtocolControl::makeDataFrame(String textData, String frameNo, String ENDFLAG, String destName)
{
	/*
    User must shift the data manually!

    This code will generate a frame of 2 bytes/frame
    Will return a frame of data in this format
    START,dest,src,frameNo,data,data,Check,END

  */
	String toSend = ""; //Frame will be store here
	//Header
	toSend += STARTFLAG;
	toSend += destName;
	toSend += srcName;
	toSend += frameNo;

	//Body
	String data = textData;
	while (data.length() < 2) //Add padding: DIFF FROM OLD CODE
	{
		data += "~"; //type with ASCII 126
	}
	toSend += data;

	//Trailer: CRC8
	int str_len = toSend.length() + 1;
	unsigned char char_array[str_len];		 // prepare a character array (the buffer)
	toSend.toCharArray(char_array, str_len); // copy it over
	uint8_t checksum = crc8->get_crc8(char_array, str_len);

	toSend += char(checksum);

	toSend += ENDFLAG;

	return toSend;
}
bool ProtocolControl::approveDataFrame(String frame) //TODO: CHANGE TO CRC
{
	/*
    Analyze checker and return boolean true if the data is correct
  */
	if (frame.length() != 8)
	{
		return false;
	}

	String toCheck = frame.substring(0, 6);
	int str_len = toCheck.length() + 1;		  // calculate length of message (with one extra character for the null terminator)
	unsigned char char_array[str_len];		  // prepare a character array (the buffer)
	toCheck.toCharArray(char_array, str_len); // copy it over
	uint8_t checksum = crc8->get_crc8(char_array, str_len);

	if (char(checksum) == frame[6])
	{
		return true;
	}
	else
	{
		return false;
	}
}

String ProtocolControl::makeAckFrame(String ackNo, String ENDFLAG, String destName)
{
	/*
    User must shift the data manually!

    This code will generate am acknowledge frame
    Will return a frame in this format
    START,dest,src,frameNo,Check,END
  */
	String toSend = ""; //Frame will be store here
	//Header
	toSend += STARTFLAG;
	toSend += destName;
	toSend += srcName;

	//Ack Number
	toSend += ackNo;

	//Trailer: CRC
	int str_len = toSend.length() + 1;
	unsigned char char_array[str_len];		 // prepare a character array (the buffer)
	toSend.toCharArray(char_array, str_len); // copy it over
	uint8_t checksum = crc8->get_crc8(char_array, str_len);
	toSend += char(checksum);

	toSend += ENDFLAG;

	return toSend;
}

bool ProtocolControl::approveAckFrame(String frame) //TODO: CHANGE TO CRC
{
	/*
    Analyze checker and return boolean true if the ack is correct
  */
	if (frame.length() != 6)
	{
		return false;
	}

	String toCheck = frame.substring(0, 4);
	int str_len = toCheck.length() + 1;		  // calculate length of message (with one extra character for the null terminator)
	unsigned char char_array[str_len];		  // prepare a character array (the buffer)
	toCheck.toCharArray(char_array, str_len); // copy it over
	uint8_t checksum = crc8->get_crc8(char_array, str_len);

	if (char(checksum) == frame[4])
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ProtocolControl::transmitter()
{
	if (Serial.available()) //Read data from serial
	{
		String frameNo = "0";
		String textData = "";
		const long TIMEOUT = 200;

		this->ackNo = "0";						 //reset ackNo
		this->allReceiving = "";				 //reset receiver
		textData = Serial.readStringUntil('\n'); //read data from serial
		//Serial.println(textData);

		while (textData.length() > 0) //Send All Data. Frame by Frame
		{
			//Make Data Frame
			String frame = "";
			if (textData.length() > 2)
				frame = this->makeDataFrame(textData.substring(0, 2), frameNo, "1", this->destName);
			else
				frame = this->makeDataFrame(textData.substring(0, 2), frameNo, "0", this->destName);

			while (true) //Send & "Wait"
			{
				for (int i = 0; i < frame.length(); i++) //Send
				{
					tx->sendFM(frame[i]);
				}
				long timer = millis();
				bool okAck = false;

				while (millis() - timer < TIMEOUT) //Wait for ACK
				{
					String ackFrame = rx->receiveStringFM(6); //receive ACK

					if (this->approveAckFrame(ackframe)) //Prep to exit the "Wait"
					{
						frameNo == "0" ? frameNo = "1" : frameNo = "0"; //change frame number
						textData = textData.substring(2);
						okAck = true;
						break;
					}
				}

				if (okAck) //Exit "Wait" part
				{
					break;
				}
			}
		}
	}
}

void ProtocolControl::receiver()
{
	bool corrupt = false, resend = false;
	bool okFrame = false;
	char endFlag = '1';
	const long TOOLONG = 2000;

	int temp = rx->receiveFM();

	if (temp != -1) //Wait for incoming data
	{
		Serial.println("Start Receiving");
		//Serial.print(temp);
		long timer = millis();

		while (endFlag != '0' && millis() - timer <= TOOLONG)
		{
			String receivedFrame = "";
			long tooLong = millis();
			while (receivedFrame.length() < 8) //receive and construct frame
			{
				int temp = rx->receiveFM(); //wait for 40ms return -1
				if (temp == -1 || millis() - tooLong >= 80)
				{
					receivedFrame = "";
					break;
				}
				receivedFrame += char(temp);
				tooLong = millis();
			}
			if (receivedFrame.equals(""))
			{
				continue;
			}

			if (this->approveDataFrame(receivedFrame))
			{
				char dest = receivedFrame[1];
				if (dest == this->srcName[0])
				{
					char frameNumber = receivedFrame[3];
					if (frameNumber == this->ackNo[0])
					{
						this->allReceiving += receivedFrame.substring(4, 6);
						Serial.println("OK: " + allReceiving);
						endFlag = receivedFrame[5];
						ackNo == "0" ? ackNo = "1" : ackNo = "0";
						String resAckFrame = this->makeAckFrame(ackNo, "0", destName); //TODO: CHANGE VALUE
						Serial.println(resAckFrame);
						for (int i = 0; i < resAckFrame.length(); i++)
						{
							tx->sendFM(resAckFrame[i]); //FM Response Ack
						}
						timer = millis();
					}
					else
					{
						Serial.println("Discard Old Frame");
						String resAckFrame = this->makeAckFrame(ackNo, "0", this->destName); //TODO: CHANGE VALUE
						resend = !resend;
						for (int i = 0; i < resAckFrame.length(); i++)
						{
							tx->sendFM(resAckFrame[i]); //FM Resend Response Ack
						}
					}
				}
			}
			else
			{
				Serial.println("Corrupted Frame");
				corrupt = !corrupt;
			}
		}

		if (endFlag == '0' && !corrupt && !resend)
		{
			Serial.println(this->allReceiving);
			ackNo = "0";
			this->allReceiving = "";
			Serial.print("-------End Of Receiving----------\n\n");
		}
		else if (millis() - timer > TOOLONG)
		{
			Serial.println("Too Long, Stop Receiving: " + this->allReceiving);
			ackNo = "0";
			this->allReceiving = "";
		}
	}
}
