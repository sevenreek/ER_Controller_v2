#include "CommunicationController.h"
CommunicationController::CommunicationController(Stream * stream)
{
	this->stream = stream;
}
bool CommunicationController::hasMessage(Message * message)
{
	while (stream->available())
	{
		char readChar = stream->read();
		
		if (readChar == '\n')
		{
			message = new Message(messageArray);
		}
		else if(pos>=TOTAL_LENGTH)
		{
			pos = 0;
			memset(messageArray, 0, TOTAL_LENGTH);
		}
		else
		{
			messageArray[pos++] = readChar;
		}
		
	}
}
void CommunicationController::sendMessage(Message * message)
{
	uint8_t* arr = Message::toByteArray(message);
	stream->write((char*)arr);
	delete[] arr;
	
}