#pragma once
#include "GameStateInterface.h"
#include "MegaGameStates.h"
class GameStateInterface;
class BoardCoordinator {
private:
	GameStateInterface * currentInterface = NULL;
	CommunicationController * comms = NULL;
	WirelessController * wireless = NULL;
	GPIOController * gpio = NULL;
	unsigned int currentInterfaceIndex;
	bool initializedCurrentInterface = false;
public:
	BoardCoordinator(CommunicationController * comms, WirelessController * wireless, GPIOController * gpio);
	void loadNextInterface();
	void loadInterface(int indentifier);
	void onUpdate();
	void initializeCurrentInterface();
};