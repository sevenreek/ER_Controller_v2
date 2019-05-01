#pragma once
#include "GameStateInterface.h"
#include "MegaGameStates.h"
class GameStateInterface;
class BoardCoordinator {
private:
	GameStateInterface * currentInterface;
	CommunicationController * comms;
	WirelessController * wireless;
	GPIOController * gpio;
	unsigned int currentInterfaceIndex;
public:
	BoardCoordinator(CommunicationController * comms, WirelessController * wireless, GPIOController * gpio);
	void loadNextInterface();
	void loadInterface(int indentifier);
	void onUpdate();
};