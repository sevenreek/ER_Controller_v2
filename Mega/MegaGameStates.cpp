#include "MegaGameStates.h"
// GS0_EnteredCells
GS0_EnteredCells::GS0_EnteredCells(BoardCoordinator* coordinator, CommunicationController* comms, GPIOController* gpio, WirelessController* wireless)
{
	this->coordinator = coordinator;
	this->comms = comms;
	this->gpio = gpio;
	this->wireless = wireless;
}
void GS0_EnteredCells::onStart()
{
	gpio->cells.init();
}
void GS0_EnteredCells::onUpdate()
{
	if (gpio->cells.areLocked())
	{
		Serial.println("Cells are locked");
		coordinator->loadNextInterface();
	}
}
void GS0_EnteredCells::onMessageRecieved(Message *  message)
{
}
void GS0_EnteredCells::onEnd()
{
}
// END GS0_EnteredCells
// GS1_LockedCells
GS1_LockedCells::GS1_LockedCells(BoardCoordinator* coordinator, CommunicationController* comms, GPIOController* gpio, WirelessController* wireless)
{
	this->coordinator = coordinator;
	this->comms = comms;
	this->gpio = gpio;
	this->wireless = wireless;
}
void GS1_LockedCells::onStart()
{
	Message* m = new Message(SNDR_MEGA, MTYPE_STATE, CMD_CELLS_LOCKED, 0);
	comms->sendMessage(m);
	delete m;
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
}
void GS1_LockedCells::onEnd()
{
	gpio->cells.free();
}
// END GS1_LockedCells
// GS2_UnlockedCells
GS2_UnlockedCells::GS2_UnlockedCells(BoardCoordinator* coordinator, CommunicationController* comms, GPIOController* gpio, WirelessController* wireless)
{
	this->coordinator = coordinator;
	this->comms = comms;
	this->gpio = gpio;
	this->wireless = wireless;
}
void GS2_UnlockedCells::onStart()
{
	Message* m = new Message(SNDR_MEGA, MTYPE_STATE, CMD_CELLS_UNLOCKED, 0);
	comms->sendMessage(m);
	delete m;
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
}
void GS2_UnlockedCells::onEnd()
{
	gpio->chest.free();
}
// END GS2_UnlockedCells
// GS3_OpenedChest
GS3_OpenedChest::GS3_OpenedChest(BoardCoordinator* coordinator, CommunicationController* comms, GPIOController* gpio, WirelessController* wireless)
{
	this->coordinator = coordinator;
	this->comms = comms;
	this->gpio = gpio;
	this->wireless = wireless;
}
void GS3_OpenedChest::onStart()
{
	Message* m = new Message(SNDR_MEGA, MTYPE_STATE, CMD_CHEST_UNLOCKED, 0);
	comms->sendMessage(m);
	delete m;
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
}
void GS3_OpenedChest::onEnd()
{
	gpio->coffin.free(); // this will be triggered once coffin is lowered so we do not it monitored anymore
}
// END GS3_OpenedChest
// GS4_LoweredCoffin
GS4_LoweredCoffin::GS4_LoweredCoffin(BoardCoordinator* coordinator, CommunicationController* comms, GPIOController* gpio, WirelessController* wireless)
{
	this->coordinator = coordinator;
	this->comms = comms;
	this->gpio = gpio;
	this->wireless = wireless;
}
void GS4_LoweredCoffin::onStart()
{
	Message* m = new Message(SNDR_MEGA, MTYPE_STATE, CMD_COFFIN_LOWERED, 0);
	comms->sendMessage(m);
	delete m;
	gpio->buttons.init();
}
void GS4_LoweredCoffin::onUpdate()
{
	gpio->buttons.updatePWMs();
	if (gpio->buttons.position >= SEQUENCE_LENGTH)
	{
		if (gpio->buttons.isCorrect())
		{
			gpio->buttons.pulseRotate();
			coordinator->loadNextInterface(); // coffin is unlocked when the next state loads.
		}
		else
		{
			gpio->buttons.pulseAll(3);
			gpio->buttons.position = 0;
			memset(gpio->buttons.sequence, 0, SEQUENCE_LENGTH * sizeof(uint8_t));
		}

	}
}
void GS4_LoweredCoffin::onMessageRecieved(Message *  message)
{
}
void GS4_LoweredCoffin::onEnd()
{
	gpio->buttons.free();
}
// END GS4_LoweredCoffin
// GS5_UnlockedCoffin
GS5_UnlockedCoffin::GS5_UnlockedCoffin(BoardCoordinator* coordinator, CommunicationController* comms, GPIOController* gpio, WirelessController* wireless)
{
	this->coordinator = coordinator;
	this->comms = comms;
	this->gpio = gpio;
	this->wireless = wireless;
}
void GS5_UnlockedCoffin::onStart()
{
	Message* m = new Message(SNDR_MEGA, MTYPE_STATE, CMD_COFFIN_UNLOCKED, 0);

	comms->sendMessage(m);
	wireless->sendMessage(m, WirelessController::REPEAT_COUNT);

	delete m;
}
void GS5_UnlockedCoffin::onUpdate()
{

}
void GS5_UnlockedCoffin::onMessageRecieved(Message *  message)
{
	if (message->sender == SNDR_COFFIN && message->type == MTYPE_STATE && message->command == CMD_COFFIN_COMPLETED)
	{
		coordinator->loadNextInterface();
	}

}
void GS5_UnlockedCoffin::onEnd()
{

}
// END GS5_UnlockedCoffin
// GS6_SolvedCoffin
GS6_SolvedCoffin::GS6_SolvedCoffin(BoardCoordinator* coordinator, CommunicationController* comms, GPIOController* gpio, WirelessController* wireless)
{
	this->coordinator = coordinator;
	this->comms = comms;
	this->gpio = gpio;
	this->wireless = wireless;
}
void GS6_SolvedCoffin::onStart()
{
	Message* m = new Message(SNDR_MEGA, MTYPE_STATE, CMD_COFFIN_COMPLETED, 0);
	comms->sendMessage(m);
	delete m;
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
}
void GS6_SolvedCoffin::onEnd()
{
	gpio->hangman.free();
}
// END GS6_SolvedCoffin
// GS7_TakenBook
GS7_TakenBook::GS7_TakenBook(BoardCoordinator* coordinator, CommunicationController* comms, GPIOController* gpio, WirelessController* wireless)
{
	this->coordinator = coordinator;
	this->comms = comms;
	this->gpio = gpio;
	this->wireless = wireless;
}
void GS7_TakenBook::onStart()
{
	Message* m = new Message(SNDR_MEGA, MTYPE_STATE, CMD_BOOK_TAKEN, 0);
	comms->sendMessage(m);
	delete m;
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
		}
	}
}
void GS7_TakenBook::onEnd()
{
	// probably get devil's arm back up or something
}
// END GS7_TakenBook
// GS_NoState
GS_NoState::GS_NoState(BoardCoordinator* coordinator, CommunicationController* comms, GPIOController* gpio, WirelessController* wireless)
{
	this->coordinator = coordinator;
	this->comms = comms;
	this->gpio = gpio;
	this->wireless = wireless;
}
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
GSR_RestoreRoom::GSR_RestoreRoom(BoardCoordinator* coordinator, CommunicationController* comms, GPIOController* gpio, WirelessController* wireless)
{
	this->coordinator = coordinator;
	this->comms = comms;
	this->gpio = gpio;
	this->wireless = wireless;
}
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