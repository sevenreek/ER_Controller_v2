#include "Message.h"
Message::Message()
{
	sender = SNDR_UNDEFINED;
	type = MTYPE_UNDEFINED;
}
Message::Message(char arr[TOTAL_LENGTH])
{
	sender = (MessageSource)(arr[0] - MESSAGE_SHIFT);
	type = (MessageType)(arr[1] - MESSAGE_SHIFT);
	command = arr[2] - MESSAGE_SHIFT;
	int arg;
	sscanf(arr + PREAMBLE_LENGTH + COMMAND_LENGTH, "%d", &arg);
	argument = arg;
}
Message::Message(MessageSource src, MessageType type, uint8_t command, int arg)
{
	this->sender = src;
	this->type = type;
	this->command = command;
	this->argument = arg;
}
char * Message::toCharArray(Message * message)
{
	char arr[TOTAL_LENGTH] = {0};
	arr[0] = message->sender + MESSAGE_SHIFT;
	arr[1] = message->type + MESSAGE_SHIFT;
	arr[2] = message->command + MESSAGE_SHIFT;
	message->argument %= 9999;
	snprintf(arr + PREAMBLE_LENGTH + COMMAND_LENGTH, ARGUMENT_LENGTH, "%d", message->argument);
	arr[TOTAL_LENGTH-2] = '\n';
	arr[TOTAL_LENGTH-1] = '\0';
	return arr;
}