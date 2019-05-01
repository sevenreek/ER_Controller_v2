#pragma once
#include "WirelessController.h"
#include "GPIOController.h"
#include "CommunicationController.h"
#include "BoardCoordinator.h"
class BoardCoordinator;
class GameStateInterface  {
protected:
	BoardCoordinator * coordinator;
	CommunicationController * comms;
	GPIOController * gpio;
	WirelessController * wireless;
public:
	virtual void onUpdate() = 0;
	virtual void onStart() = 0;
	virtual void onEnd() = 0;
	virtual void onMessageRecieved(Message *  message) = 0;
	//virtual ~GameStateInterface() = 0;

};