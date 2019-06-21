#include "CommunicationController.h"
const unsigned int CommunicationController::MESSAGE_TIMEOUT = 2500;
const char CommunicationController::FLUSHING_CHARACTER = 127;
CommunicationController::CommunicationController(Stream * stream)
{
	this->stream = stream;
}
void CommunicationController::flushBuffer()
{
	Serial.println("Flushing...");
	pos = 0;
	memset(messageArray, 0, TOTAL_LENGTH);
}

void CommunicationController::pushCommsCleaner()
{
	stream->write(FLUSHING_CHARACTER);
}
bool CommunicationController::hasMessage(Message * &message)
{
	if(pos > 0 && lastRecieved + MESSAGE_TIMEOUT < millis())
		flushBuffer();
	while (stream->available())
	{
		lastRecieved = millis();
		char readChar = stream->read();
		Serial.print(readChar, HEX);
		Serial.print(' ');
		if (readChar == '\n')
		{
			Serial.print("pos = "); Serial.println(pos);
			message = Message::fromByteArray(messageArray);
			//Serial.print("Returning pointer: "); Serial.println((int)message);
			flushBuffer();
			return true;
		}
		else if (readChar == FLUSHING_CHARACTER)
		{
			flushBuffer();
		}
		else if (pos >= TOTAL_LENGTH)
		{
			flushBuffer();
		}
		else if (pos < 3 && !isAlphaNumeric(readChar))
		{
			//flushBuffer();
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
	stream->write(FLUSHING_CHARACTER);
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