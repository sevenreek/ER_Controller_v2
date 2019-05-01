#include "MegaGameStates.h"
// GS0_EnteredCells
void GS0_EnteredCells::onStart()
{
}
void GS0_EnteredCells::onUpdate()
{
}
void GS0_EnteredCells::onMessageRecieved(Message *  message)
{
	delete message;
}
void GS0_EnteredCells::onEnd()
{
}
// END GS0_EnteredCells
// GS1_LockedCells
void GS1_LockedCells::onStart()
{
	comms->sendMessage(new Message(SNDR_MEGA, MTYPE_STATE, CMD_CELLS_LOCKED, 0));
}
void GS1_LockedCells::onUpdate()
{
	if (!gpio->cells.areLocked())
	{
		coordinator->loadNextInterface();
	}
}
void GS1_LockedCells::onMessageRecieved(Message *  message)
{
	delete message;
}
void GS1_LockedCells::onEnd()
{
	gpio->cells.free();
}
// END GS1_LockedCells
// GS2_UnlockedCells
void GS2_UnlockedCells::onStart()
{
	comms->sendMessage(new Message(SNDR_MEGA, MTYPE_STATE, CMD_CELLS_UNLOCKED, 0));
	gpio->chest.init();
}
void GS2_UnlockedCells::onUpdate()
{
	if (!gpio->chest.isLocked())
	{
		coordinator->loadNextInterface();
	}
}
void GS2_UnlockedCells::onMessageRecieved(Message *  message)
{
	delete message;
}
void GS2_UnlockedCells::onEnd()
{
	gpio->chest.free();
}
// END GS2_UnlockedCells
// GS3_OpenedChest
void GS3_OpenedChest::onStart()
{
	comms->sendMessage(new Message(SNDR_MEGA, MTYPE_STATE, CMD_CHEST_UNLOCKED, 0));
	gpio->coffin.init();
}
void GS3_OpenedChest::onUpdate()
{

	if (gpio->coffin.isLowered())
	{
		coordinator->loadNextInterface();
	}
}
void GS3_OpenedChest::onMessageRecieved(Message *  message)
{
	delete message;
}
void GS3_OpenedChest::onEnd()
{
	gpio->coffin.free(); // this will be triggered once coffin is lowered so we do not it monitored anymore
}
// END GS3_OpenedChest
// GS4_LoweredCoffin
void GS4_LoweredCoffin::onStart()
{
	comms->sendMessage(new Message(SNDR_MEGA, MTYPE_STATE, CMD_COFFIN_LOWERED, 0));
	gpio->buttons.init();
}
void GS4_LoweredCoffin::onUpdate()
{
	if (gpio->buttons.isCorrect())
	{
		coordinator->loadNextInterface(); // coffin is unlocked when the next state loads.
	}
}
void GS4_LoweredCoffin::onMessageRecieved(Message *  message)
{
	delete message;
}
void GS4_LoweredCoffin::onEnd()
{
	gpio->buttons.free();
}
// END GS4_LoweredCoffin
// GS5_UnlockedCoffin
void GS5_UnlockedCoffin::onStart()
{
	comms->sendMessage(new Message(SNDR_MEGA, MTYPE_STATE, CMD_COFFIN_UNLOCKED, 0));
	wireless->sendMessage(new Message(SNDR_MEGA, MTYPE_EVENT, CMD_COFFIN_UNLOCKED, 0), WirelessController::REPEAT_COUNT);
}
void GS5_UnlockedCoffin::onUpdate()
{

}
void GS5_UnlockedCoffin::onMessageRecieved(Message *  message)
{
	if (message->sender == SNDR_COFFIN && message->type == MTYPE_STATE && message->command == CMD_COFFIN_COMPLETED)
	{
		delete message;
		coordinator->loadNextInterface();
	}

}
void GS5_UnlockedCoffin::onEnd()
{

}
// END GS5_UnlockedCoffin
// GS6_SolvedCoffin
void GS6_SolvedCoffin::onStart()
{
	comms->sendMessage(new Message(SNDR_MEGA, MTYPE_STATE, CMD_COFFIN_COMPLETED, 0));
	gpio->hangman.init();
	gpio->hangman.drop();
}
void GS6_SolvedCoffin::onUpdate()
{
	if (gpio->hangman.isBookTaken())
	{
		coordinator->loadNextInterface();
	}
}
void GS6_SolvedCoffin::onMessageRecieved(Message *  message)
{
	delete message;
}
void GS6_SolvedCoffin::onEnd()
{
	gpio->hangman.free();
}
// END GS6_SolvedCoffin
// GS7_TakenBook
void GS7_TakenBook::onStart()
{
	comms->sendMessage(new Message(SNDR_MEGA, MTYPE_STATE, CMD_BOOK_TAKEN, 0));
	gpio->rings.init();
	gpio->devil.init();
}
void GS7_TakenBook::onUpdate()
{

}
void GS7_TakenBook::onMessageRecieved(Message *  message)
{
	if (message->sender == SNDR_PC && message->type == MTYPE_EVENT)
	{
		if (message->type == CMD_SPELL_CAST_BEGIN)
		{
			gpio->rings.pulse();
		}
		else if (message->type == CMD_SPELL_CAST_CORRECTLY)
		{
			switch (message->argument)
			{

			case 0: // spell 0 no need to send sounds as the message is sourced from PC
				gpio->devil.lightLed();
				break;
			case 1: // spell 1

				break;
			case 2: // spell 2
				gpio->devil.dropCurtain();
				break;
			case 3: // spell 3
				gpio->devil.beginMoveUp();
				break;

			default:break;
			}
			delete message;
		}
	}
}
void GS7_TakenBook::onEnd()
{
	// probably get devil's arm back up or something
}
// END GS7_TakenBook
// GS_NoState
void GS_NoState::onStart()
{
	Serial.println("ERROR: NoState started!");
}
void GS_NoState::onUpdate()
{
	Serial.println("ERROR: NoState updated!");
}
void GS_NoState::onMessageRecieved(Message *  message)
{
	Serial.println("ERROR: NoState recieved message!");
}
void GS_NoState::onEnd()
{
	Serial.println("ERROR: NoState ended!");
}
// END GS_NoState
// GSR_RestoreRoom
void GSR_RestoreRoom::onStart()
{
	Serial.println("Restoring room state!");
}
void GSR_RestoreRoom::onUpdate()
{
	Serial.println("ERROR: NoState updated!");
}
void GSR_RestoreRoom::onMessageRecieved(Message *  message)
{
	Serial.println("ERROR: NoState recieved message!");
}
void GSR_RestoreRoom::onEnd()
{
	Serial.println("ERROR: NoState ended!");
}
// END GSR_RestoreRoom