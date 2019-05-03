#include "Message.h"
Message::Message()
{
	sender = SNDR_UNDEFINED;
	type = MTYPE_UNDEFINED;
}
Message* Message::fromByteArray(uint8_t* arr)
{
	Message* m = new Message();
	m->sender = (MessageSource)(arr[0] - MESSAGE_SHIFT);
	m->type = (MessageType)(arr[1] - MESSAGE_SHIFT);
	m->command = arr[2] - MESSAGE_SHIFT;
	int arg;
	sscanf((char*)(arr + PREAMBLE_LENGTH + COMMAND_LENGTH), "%d", &arg);
	m->argument = arg;
	return m;
}
Message::Message(uint8_t arr[TOTAL_LENGTH])
{
	sender = (MessageSource)(arr[0] - MESSAGE_SHIFT);
	type = (MessageType)(arr[1] - MESSAGE_SHIFT);
	command = arr[2] - MESSAGE_SHIFT;
	int arg;
	sscanf((char*)(arr + PREAMBLE_LENGTH + COMMAND_LENGTH), "%d", &arg);
	argument = arg;
}
Message::Message(MessageSource src, MessageType type, uint8_t command, int arg)
{
	this->sender = src;
	this->type = type;
	this->command = command;
	this->argument = arg;
}
uint8_t* Message::toByteArray(Message * message)
{

	uint8_t* arr = new uint8_t[TOTAL_LENGTH];
	memset(arr, 0, TOTAL_LENGTH * sizeof(uint8_t));
	arr[0] = (uint8_t)message->sender + MESSAGE_SHIFT;

	arr[1] = (uint8_t)message->type + MESSAGE_SHIFT;

	arr[2] = (uint8_t)message->command + MESSAGE_SHIFT;

	message->argument %= 9999;
	snprintf((char*)(arr + PREAMBLE_LENGTH + COMMAND_LENGTH), ARGUMENT_LENGTH, "%d", message->argument);
	arr[TOTAL_LENGTH - 1] = '\n';

	return arr;
}