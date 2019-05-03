#include "CommunicationController.h"
CommunicationController::CommunicationController(Stream * stream)
{
	this->stream = stream;
}
bool CommunicationController::hasMessage(Message * &message)
{
	while (stream->available())
	{
		
		char readChar = stream->read();
		//Serial.print(readChar, HEX);
		//Serial.print(' ');
		if (readChar == '\n')
		{
			message = Message::fromByteArray(messageArray);
			//Serial.print("Returning pointer: "); Serial.println((int)message);
			pos = 0;
			memset(messageArray, 0, TOTAL_LENGTH * sizeof(uint8_t));
			return true;
		}
		else if (pos >= TOTAL_LENGTH)
		{
			pos = 0;
			memset(messageArray, 0, TOTAL_LENGTH);
		}
		else
		{
			messageArray[pos++] = readChar;
		}
		return false;
	}
}
void CommunicationController::sendMessage(Message * message)
{
	uint8_t* arr = Message::toByteArray(message);
	/*Serial.println("Writing:");
	for (int i = 0; i < TOTAL_LENGTH; i++)
	{
		Serial.print(arr[i]); Serial.print(' ');
	}*/
	for(int i = 0; i<TOTAL_LENGTH; i++)
	{
		stream->write(arr[i]);
	}
		
	
	delete[] arr;
	
}