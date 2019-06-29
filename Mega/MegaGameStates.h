#pragma once


#include "GameStateInterface.h"
enum MegaStates {
	MEGASTATE_0_EnteredCells,
	MEGASTATE_1_LockedCells,
	MEGASTATE_2_UnlockedCells,
	MEGASTATE_3_OpenedChest,
	MEGASTATE_4_LoweredCoffin,
	MEGASTATE_5_UnlockedCoffin,
	MEGASTATE_6_SolvedCoffin,
	MEGASTATE_7_TakenBook,
	MEGASTATE_R_RestoreRoom,
};
class GS_NoState : public GameStateInterface {
public:
	GS_NoState(BoardCoordinator* coordinator, CommunicationController* comms, GPIOController* gpio, WirelessController* wireless);
	void onUpdate();
	void onStart();
	void onInit();
	void onMessageRecieved(Message *  message);
	void onEnd();
};
class GS0_EnteredCells : public GameStateInterface {
public:
	GS0_EnteredCells(BoardCoordinator* coordinator, CommunicationController* comms, GPIOController* gpio, WirelessController* wireless);
	void onUpdate();
	void onInit();
	void onStart();
	void onMessageRecieved(Message *  message);
	void onEnd();
};
class GS1_LockedCells : public GameStateInterface {
public:
	GS1_LockedCells(BoardCoordinator* coordinator, CommunicationController* comms, GPIOController* gpio, WirelessController* wireless);
	void onUpdate();
	void onInit();
	void onStart();
	void onMessageRecieved(Message *  message);
	void onEnd();
};
class GS2_UnlockedCells : public GameStateInterface {
public:
	GS2_UnlockedCells(BoardCoordinator* coordinator, CommunicationController* comms, GPIOController* gpio, WirelessController* wireless);
	void onUpdate();
	void onInit();
	void onStart();
	void onMessageRecieved(Message *  message);
	void onEnd();
};
class GS3_OpenedChest : public GameStateInterface {
public:
	GS3_OpenedChest(BoardCoordinator* coordinator, CommunicationController* comms, GPIOController* gpio, WirelessController* wireless);
	void onUpdate();
	void onStart();
	void onInit();
	void onMessageRecieved(Message *  message);
	void onEnd();
};
class GS4_LoweredCoffin : public GameStateInterface {
public:
	GS4_LoweredCoffin(BoardCoordinator* coordinator, CommunicationController* comms, GPIOController* gpio, WirelessController* wireless);
	void onUpdate();
	void onStart();
	void onInit();
	void onMessageRecieved(Message *  message);
	void onEnd();
};
class GS5_UnlockedCoffin : public GameStateInterface {
public:
	GS5_UnlockedCoffin(BoardCoordinator* coordinator, CommunicationController* comms, GPIOController* gpio, WirelessController* wireless);
	void onUpdate();
	void onStart();
	void onInit();
	void onMessageRecieved(Message *  message);
	void onEnd();
};
class GS6_SolvedCoffin : public GameStateInterface {
public:
	GS6_SolvedCoffin(BoardCoordinator* coordinator, CommunicationController* comms, GPIOController* gpio, WirelessController* wireless);
	void onUpdate();
	void onStart();
	void onInit();
	void onMessageRecieved(Message *  message);
	void onEnd();
};
class GS7_TakenBook : public GameStateInterface {
public:
	GS7_TakenBook(BoardCoordinator* coordinator, CommunicationController* comms, GPIOController* gpio, WirelessController* wireless);
	void onUpdate();
	void onStart();
	void onInit();
	void onMessageRecieved(Message *  message);
	void onEnd();
};
class GSR_RestoreRoom : public GameStateInterface {
public:
	GSR_RestoreRoom(BoardCoordinator* coordinator, CommunicationController* comms, GPIOController* gpio, WirelessController* wireless);
	void onUpdate();
	void onStart();
	void onInit();
	void onMessageRecieved(Message *  message);
	void onEnd();
};
