#include "BoardCoordinator.h"
BoardCoordinator::BoardCoordinator(CommunicationController * comms, WirelessController * wireless, GPIOController * gpio)
{
	this->comms = comms;
	this->wireless = wireless;
	this->gpio = gpio;
	currentInterfaceIndex = 0;
	currentInterface = new  GS0_EnteredCells();
	currentInterface->onStart();
}
void BoardCoordinator::loadInterface(int identifier)
{
	currentInterface->onEnd();
	delete currentInterface;
	GameStateInterface * newInterface;
	currentInterfaceIndex = identifier;
	switch (identifier)
	{
		case MEGASTATE_0_EnteredCells:
			newInterface = new GS0_EnteredCells();
		break;
		case MEGASTATE_1_LockedCells:
			newInterface = new GS1_LockedCells();
		break;
		case MEGASTATE_2_UnlockedCells:
			newInterface = new GS2_UnlockedCells();
		break;
		case MEGASTATE_3_OpenedChest:
			newInterface = new GS3_OpenedChest();
		break;
		case MEGASTATE_4_LoweredCoffin:
			newInterface = new GS4_LoweredCoffin();
		break;
		case MEGASTATE_5_UnlockedCoffin:
			newInterface = new GS5_UnlockedCoffin();
		break;
		case MEGASTATE_6_SolvedCoffin:
			newInterface = new GS6_SolvedCoffin();
		break;
		case MEGASTATE_7_TakenBook:
			newInterface = new GS7_TakenBook();
		break;

		default:
			newInterface = new GS_NoState();
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
	if (currentInterface)
		currentInterface->onUpdate();
}