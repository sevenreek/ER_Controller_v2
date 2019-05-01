#pragma once
#include "Arduino.h"
static const int ARGUMENT_LENGTH = 4;
static const int PREAMBLE_LENGTH = 2; // MessageSource + MessageType
static const int COMMAND_LENGTH = 1; 
static const int POSTAMBLE_LENGTH = 2; // \n\0
static const int TOTAL_LENGTH = PREAMBLE_LENGTH + COMMAND_LENGTH + ARGUMENT_LENGTH + POSTAMBLE_LENGTH;
static const int MESSAGE_SHIFT = 0x30; // messages are shifted by hex 0x30 since 0 is \0 and 10 is new line etc.
enum MessageType {
	MTYPE_UNDEFINED,
	MTYPE_DEBUG,
	MTYPE_EVENT,
	MTYPE_STATE,
};
enum MessageSource {
	SNDR_UNDEFINED,
	SNDR_MEGA,
	SNDR_PC,
	SNDR_NANO,
	SNDR_COFFIN,
};
enum StateCommands {
	CMD_NOSTATE,
	CMD_CELLS_LOCKED,
	CMD_CELLS_UNLOCKED,
	CMD_CHEST_UNLOCKED,
	CMD_COFFIN_LOWERED,
	CMD_COFFIN_UNLOCKED,
	CMD_COFFIN_COMPLETED,
	CMD_HANGMAN_DOWN,
	CMD_BOOK_TAKEN,
};
enum EventCommands {
	CMD_NOEVENT,
	CMD_TORCH_GLOW,
	CMD_TORCH_DIM,
	CMD_SPELL_CAST_BEGIN, // add args here
	CMD_SPELL_CAST_CORRECTLY,
};
class Message {
private:
public:
	Message();
	Message(char arr[TOTAL_LENGTH]);
	Message(MessageSource src, MessageType type, uint8_t command, int argument);
	MessageSource sender;
	MessageType type;
	uint8_t command;
	int argument;
	static char * toCharArray(Message * message);
};



