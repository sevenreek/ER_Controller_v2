#include "BoardCoordinator.h"
//(BoardCoordinator* coordinator, CommunicationController* comms, GPIOController* gpio, WirelessController* wireless)
BoardCoordinator::BoardCoordinator(CommunicationController * comms, WirelessController * wireless, GPIOController * gpio)
{
	//Serial.println("Board coordinator loaded.");
	//delay(1000);
	this->comms = comms;
	this->wireless = wireless;
	this->gpio = gpio;
	currentInterfaceIndex = 0;
}
void BoardCoordinator::loadInterface(int identifier, bool forceStart)
{
	startedCurrentInterface = forceStart;
	Serial.print("Loading interface ");
	Serial.println(identifier);
	if (currentInterface)
	{
		currentInterface->onEnd();
		delete currentInterface;
	}
	currentInterfaceIndex = identifier;
	switch (identifier)
	{
		case MEGASTATE_0_EnteredCells:
			currentInterface = new GS0_EnteredCells(this, comms, gpio, wireless);
		break;
		case MEGASTATE_1_LockedCells:
			currentInterface = new GS1_LockedCells(this, comms, gpio, wireless);
		break;
		case MEGASTATE_2_UnlockedCells:
			currentInterface = new GS2_UnlockedCells(this, comms, gpio, wireless);
		break;
		case MEGASTATE_3_OpenedChest:
			currentInterface = new GS3_OpenedChest(this, comms, gpio, wireless);
		break;
		case MEGASTATE_4_LoweredCoffin:
			currentInterface = new GS4_LoweredCoffin(this, comms, gpio, wireless);
		break;
		case MEGASTATE_5_UnlockedCoffin:
			currentInterface = new GS5_UnlockedCoffin(this, comms, gpio, wireless);
		break;
		case MEGASTATE_6_SolvedCoffin:
			currentInterface = new GS6_SolvedCoffin(this, comms, gpio, wireless);
		break;
		case MEGASTATE_7_TakenBook:
			currentInterface = new GS7_TakenBook(this, comms, gpio, wireless);
		break;
		case MEGASTATE_R_RestoreRoom:
			currentInterface = new GSR_RestoreRoom(this, comms, gpio, wireless);
		break;
		default:
			currentInterface = new GS_NoState(this, comms, gpio, wireless);
		break;
	}
	currentInterface->onInit();
	if(forceStart)
		currentInterface->onStart();
}
void BoardCoordinator::loadNextInterface()
{
	loadInterface(currentInterfaceIndex+1, false);
}
void BoardCoordinator::onUpdate()
{
	Message * msg = 0;
	if (comms->hasMessage(msg))
	{
		if (msg->sender == SNDR_PC)
		{
			Serial.println("PC sent something");
			if (msg->type == MTYPE_STATE)
			{
				Serial.print("PC sent "); Serial.println(msg->command);
				switch (msg->command)
				{
					case CMD_NOSTATE: default:
					break;
					case CMD_CELLS_LOCKED:
						loadInterface(MEGASTATE_1_LockedCells,true);
					break;
					case CMD_CELLS_UNLOCKED:
						loadInterface(MEGASTATE_2_UnlockedCells,true);
						break;
					case CMD_CHEST_UNLOCKED:
						loadInterface(MEGASTATE_3_OpenedChest,true);
						break;
					case CMD_COFFIN_LOWERED:
						loadInterface(MEGASTATE_4_LoweredCoffin,true);
						break;
					case CMD_COFFIN_UNLOCKED:
						loadInterface(MEGASTATE_5_UnlockedCoffin,true);
						break;
					case CMD_COFFIN_COMPLETED: case CMD_HANGMAN_DOWN:
						loadInterface(MEGASTATE_6_SolvedCoffin,true);
						break;
					case CMD_BOOK_TAKEN:
						loadInterface(MEGASTATE_7_TakenBook,true);
						break;
					case CMD_RESTORE_ROOM:
						loadInterface(MEGASTATE_R_RestoreRoom,true);
						break;
					case CMD_ZERO_STATE:
						loadInterface(MEGASTATE_0_EnteredCells,true);
						break;
				}
				startCurrentInterface();
			}
			else if (msg->type == MTYPE_EVENT)
			{
				if (currentInterface)
				{
					if (msg->command == CMD_ACKNOWLEDGE_STATE_CHANGE && !startedCurrentInterface)
					{
						startCurrentInterface();
					}
					else if (!startedCurrentInterface);
					else
						currentInterface->onMessageRecieved(msg);
				}
				if (msg->command == CMD_FOG_RUN)
				{
					Serial.print("Running fogger for "); Serial.println(msg->argument);
					gpio->fogmachine.run(msg->argument * gpio->fogmachine.ARGUMENT_TIME_MULTIPLIER); // mult by 100
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
	//Serial.print("Update coordinator");
	//Serial.println((int)currentInterface);
	if (currentInterface && startedCurrentInterface)
		currentInterface->onUpdate();
}
void BoardCoordinator::startCurrentInterface()
{
	Serial.print("Starting "); Serial.print(currentInterfaceIndex);
	currentInterface->onStart();
	startedCurrentInterface = true;
}
