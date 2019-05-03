#include "BoardCoordinator.h"
//(BoardCoordinator* coordinator, CommunicationController* comms, GPIOController* gpio, WirelessController* wireless)
BoardCoordinator::BoardCoordinator(CommunicationController * comms, WirelessController * wireless, GPIOController * gpio)
{
	this->comms = comms;
	this->wireless = wireless;
	this->gpio = gpio;
	currentInterfaceIndex = 0;
	currentInterface = new  GS0_EnteredCells(this,  comms, gpio, wireless);
	currentInterface->onStart();
}
void BoardCoordinator::loadInterface(int identifier)
{
	Serial.print("Loading new interface ");
	Serial.println(identifier);
	currentInterface->onEnd();
	delete currentInterface;
	GameStateInterface * newInterface = 0;
	currentInterfaceIndex = identifier;
	switch (identifier)
	{
		case MEGASTATE_0_EnteredCells:
			newInterface = new GS0_EnteredCells(this, comms, gpio, wireless);
		break;
		case MEGASTATE_1_LockedCells:
			newInterface = new GS1_LockedCells(this, comms, gpio, wireless);
		break;
		case MEGASTATE_2_UnlockedCells:
			newInterface = new GS2_UnlockedCells(this, comms, gpio, wireless);
		break;
		case MEGASTATE_3_OpenedChest:
			newInterface = new GS3_OpenedChest(this, comms, gpio, wireless);
		break;
		case MEGASTATE_4_LoweredCoffin:
			newInterface = new GS4_LoweredCoffin(this, comms, gpio, wireless);
		break;
		case MEGASTATE_5_UnlockedCoffin:
			newInterface = new GS5_UnlockedCoffin(this, comms, gpio, wireless);
		break;
		case MEGASTATE_6_SolvedCoffin:
			newInterface = new GS6_SolvedCoffin(this, comms, gpio, wireless);
		break;
		case MEGASTATE_7_TakenBook:
			newInterface = new GS7_TakenBook(this, comms, gpio, wireless);
		break;

		default:
			newInterface = new GS_NoState(this, comms, gpio, wireless);
		break;
	}
	newInterface->onStart();
}
void BoardCoordinator::loadNextInterface()
{
	loadInterface(currentInterfaceIndex+1);
}
void BoardCoordinator::onUpdate()
{
	Message * msg = 0;
	if (comms->hasMessage(msg))
	{
		if (msg->sender == SNDR_PC)
		{
			//Serial.println("PC sent something");
			if (msg->type == MTYPE_STATE)
			{
				switch (msg->command)
				{
					case CMD_NOSTATE: default:
					break;
					case CMD_CELLS_LOCKED:
						loadInterface(MEGASTATE_1_LockedCells);
					break;
					case CMD_CELLS_UNLOCKED:
						loadInterface(MEGASTATE_2_UnlockedCells);
						break;
					case CMD_CHEST_UNLOCKED:
						loadInterface(MEGASTATE_3_OpenedChest);
						break;
					case CMD_COFFIN_LOWERED:
						loadInterface(MEGASTATE_4_LoweredCoffin);
						break;
					case CMD_COFFIN_UNLOCKED:
						loadInterface(MEGASTATE_5_UnlockedCoffin);
						break;
					case CMD_COFFIN_COMPLETED:
						loadInterface(MEGASTATE_6_SolvedCoffin);
						break;
					case CMD_BOOK_TAKEN:
						loadInterface(MEGASTATE_7_TakenBook);
						break;

				}
			}
			else if (msg->type == MTYPE_EVENT)
			{
				
				switch (msg->command)
				{
					case CMD_NOEVENT: default:
					break;
					case CMD_SPELL_CAST_BEGIN:
					case CMD_SPELL_CAST_CORRECTLY:
						//Serial.print("pointer passed:");
						//Serial.println((int)msg);
						if(currentInterface)
							currentInterface->onMessageRecieved(msg);
					break;
				}
			}
		}
		delete msg;
	}
	if (wireless->hasMessage(msg))
	{
		currentInterface->onMessageRecieved(msg);
		delete msg;
	}
	if (currentInterface)
		currentInterface->onUpdate();
}
