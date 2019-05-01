#pragma once
#include "GameStateInterface.h"
#include "MegaGameStates.h"
class GameStateInterface;
class BoardCoordinator {
private:
	GameStateInterface * currentInterface;
	CommunicationController * pc;
	CommunicationController * mega;
	WirelessController * wireless;
	GPIOController * gpio;
	unsigned int currentInterfaceIndex;
public:
	BoardCoordinator(CommunicationController * pc, CommunicationController * mega, WirelessController * wireless, GPIOController * gpio);
	void loadNextInterface();
	void loadInterface(int indentifier);
	void onUpdate();
};