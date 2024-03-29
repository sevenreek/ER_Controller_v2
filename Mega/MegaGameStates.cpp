#include "MegaGameStates.h"
// GS0_EnteredCells
GS0_EnteredCells::GS0_EnteredCells(BoardCoordinator* coordinator, CommunicationController* comms, GPIOController* gpio, WirelessController* wireless)
{

	this->coordinator = coordinator;
	this->comms = comms;
	this->gpio = gpio;
	this->wireless = wireless;
}
void GS0_EnteredCells::onInit()
{

	Message* m = new Message(SNDR_MEGA, MTYPE_STATE, CMD_ZERO_STATE, 0);
	comms->sendMessage(m);
	delete m;
}
void GS0_EnteredCells::onStart()
{

	Message* m = new Message(SNDR_MEGA, MTYPE_STATE, CMD_ZERO_STATE, 1);
	comms->sendMessage(m);
	delete m;
	gpio->cells.init();
	gpio->fogmachine.init();
	
}
void GS0_EnteredCells::onUpdate()
{
	//Serial.println("Interface onUpdate");
	if (gpio->cells.areLocked())
	{
		//Serial.println("Cells are locked");
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
void GS1_LockedCells::onInit()
{

	Message* m = new Message(SNDR_MEGA, MTYPE_STATE, CMD_CELLS_LOCKED, 0);
	comms->sendMessage(m);
	delete m;
}	
void GS1_LockedCells::onStart()
{
	Message* m = new Message(SNDR_MEGA, MTYPE_STATE, CMD_CELLS_LOCKED, 1 );
	comms->sendMessage(m);
	delete m;
	gpio->chest.init();
	gpio->coffin.init();
}
void GS1_LockedCells::onUpdate()
{
	if (gpio->cells.areUnlocked())
	{
		coordinator->loadNextInterface();
	}/*
	if (!gpio->chest.isLocked())
	{
		coordinator->loadInterface(MEGASTATE_3_OpenedChest, false);
	}*/
	if (gpio->coffin.isLowered())
	{
		coordinator->loadInterface(MEGASTATE_4_LoweredCoffin, false);
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

void GS2_UnlockedCells::onInit()
{
	Message* m = new Message(SNDR_MEGA, MTYPE_STATE, CMD_CELLS_UNLOCKED, 0);
	comms->sendMessage(m);
	delete m;

}
void GS2_UnlockedCells::onStart()
{
	Message* m = new Message(SNDR_MEGA, MTYPE_STATE, CMD_CELLS_UNLOCKED, 1 );
	comms->sendMessage(m);
	delete m;
	
}
void GS2_UnlockedCells::onUpdate()
{
	/*
	if (!gpio->chest.isLocked())
	{
		coordinator->loadNextInterface();
	}
	*/
	if (gpio->coffin.isLowered())
	{
		coordinator->loadInterface(MEGASTATE_4_LoweredCoffin, false);
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

void GS3_OpenedChest::onInit()
{

	Message* m = new Message(SNDR_MEGA, MTYPE_STATE, CMD_CHEST_UNLOCKED, 0);
	comms->sendMessage(m);
	delete m;
}
void GS3_OpenedChest::onStart()
{

	Message* m = new Message(SNDR_MEGA, MTYPE_STATE, CMD_CHEST_UNLOCKED, 1 );
	comms->sendMessage(m);
	delete m;
	

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

void GS4_LoweredCoffin::onInit()
{
	Message* m = new Message(SNDR_MEGA, MTYPE_STATE, CMD_COFFIN_LOWERED, 0);
	comms->sendMessage(m);
	delete m;
}
void GS4_LoweredCoffin::onStart()
{
	Message* m = new Message(SNDR_MEGA, MTYPE_STATE, CMD_COFFIN_LOWERED,  1);
	comms->sendMessage(m);
	delete m;
	gpio->buttons.init();

}
void GS4_LoweredCoffin::onUpdate()
{	
	gpio->buttons.updatePWMs();
	if (gpio->buttons.isCorrect())
	{
		gpio->buttons.pulseRotate();
		coordinator->loadNextInterface(); // coffin is unlocked when the next state loads.
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

void GS5_UnlockedCoffin::onInit()
{
	Message *  m = new Message(SNDR_MEGA, MTYPE_STATE, CMD_COFFIN_UNLOCKED, 0);
	comms->sendMessage(m);
	delete m;

}
void GS5_UnlockedCoffin::onStart()
{
	
	gpio->coffin.open(wireless);
	Message* m;
	m = new Message(SNDR_MEGA, MTYPE_STATE, CMD_COFFIN_UNLOCKED, 1);
	comms->sendMessage(m);
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
	Serial.println("Got message");

}
void GS5_UnlockedCoffin::onEnd()
{
	gpio->buttons.free();
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

void GS6_SolvedCoffin::onInit()
{

	Message* m = new Message(SNDR_MEGA, MTYPE_STATE, CMD_COFFIN_COMPLETED, 0);
	comms->sendMessage(m);
	delete m;
}
void GS6_SolvedCoffin::onStart()
{

	Message* m = new Message(SNDR_MEGA, MTYPE_STATE, CMD_COFFIN_COMPLETED, 1 );
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
static const uint16_t LIGHTS_OFF_TIME = 5000; // time for which the spellrings turn off after the devil is discovered; max is (2^16-1) approx. 65000
GS7_TakenBook::GS7_TakenBook(BoardCoordinator* coordinator, CommunicationController* comms, GPIOController* gpio, WirelessController* wireless)
{
	this->coordinator = coordinator;
	this->comms = comms;
	this->gpio = gpio;
	this->wireless = wireless;
}

void GS7_TakenBook::onInit()
{

	Message* m = new Message(SNDR_MEGA, MTYPE_STATE, CMD_BOOK_TAKEN, 0);
	comms->sendMessage(m);
	delete m;
}
void GS7_TakenBook::onStart()
{

	Message* m = new Message(SNDR_MEGA, MTYPE_STATE, CMD_BOOK_TAKEN, 1 );
	comms->sendMessage(m);
	delete m;
	gpio->rings.init();
	gpio->devil.init();
	gpio->rings.enable();
}
void GS7_TakenBook::onUpdate()
{
	gpio->rings.updatePWMs(); 
}
void GS7_TakenBook::onMessageRecieved(Message *  message)
{
	Serial.println("Recieved message in 7");
	//Serial.print("pointer recieved:");
	//Serial.println((int)message);
	if (message->sender == SNDR_PC && message->type == MTYPE_EVENT)
	{
		Serial.println("Event coming from PC recognized.");
		Serial.print("Command is "); Serial.println(message->command);
		switch (message->command)
		{
		case CMD_SPELL_CAST_BEGIN:
			{
				gpio->rings.pulse();
			}
			break;
		case CMD_SPELL_CAST_END:
			{
				gpio->rings.stopPulse();
			}
			break;
		case CMD_FOG_RUN:
			{
				gpio->fogmachine.run(message->argument);
			}
			break;
		case CMD_RINGS_KILL:
			{
				gpio->rings.kill();
			}
			break;
		case CMD_RINGS_ENABLE:
			{
				gpio->rings.enable();
			}
			break;
		case CMD_SPELL_CAST_CORRECTLY:
			{
				gpio->rings.stopPulse();
				gpio->rings.kill();
				Serial.println("Recieved spellcastcorrect");
				switch (message->argument)
				{

				case 0: // spell 0 no need to send sounds as the message is sourced from PC
					gpio->devil.lightLed();
					//Serial.println("Recieved first spell");
					break;
				case 1: // spell 1

					break;
				case 2: // spell 2
					gpio->devil.dropCurtain();
					break;
				case 3: // spell 3
					Serial.println("Spell cast to move up.");
					gpio->devil.beginMoveUp();
					break;

				default:break;
				}
			}
			break;
		default:
			Serial.println("Command not recognized");
		}
	}
}
void GS7_TakenBook::onEnd()
{
	gpio->rings.free();
	gpio->devil.free();
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

void GS_NoState::onInit()
{
	Serial.println("ERROR: NoState started!");
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

void GSR_RestoreRoom::onInit()
{
}
void GSR_RestoreRoom::onStart()
{
	Serial.println("Restoring room state!");
  gpio->devil.init();
	gpio->devil.beginMoveDown();
	gpio->buttons.init();
	gpio->hangman.init();
}
void GSR_RestoreRoom::onUpdate()
{
	if (digitalRead(gpio->hangman.PIN_BOOK_REED) == LOW)
	{
		for (int i = 0; i < 4; i++)
			analogWrite(gpio->buttons.PIN_BUTTONS_PWM[i], 100);
	}
	else
	{
		for (int i = 0; i < 4; i++)
			analogWrite(gpio->buttons.PIN_BUTTONS_PWM[i], 0);
	}
	//Serial.println(digitalRead(gpio->hangman.PIN_BOOK_REED) == LOW ? "BOOK DETECTED" : "NO BOOK");
}
void GSR_RestoreRoom::onMessageRecieved(Message *  message)
{
	
}
void GSR_RestoreRoom::onEnd()
{
	gpio->buttons.free();
	gpio->devil.free();
	gpio->hangman.free();
}
// END GSR_RestoreRoom
