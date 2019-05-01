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
};
class GS_NoState : public GameStateInterface {
	void onUpdate();
	void onStart();
	void onMessageRecieved(Message *  message);
	void onEnd();
};
class GS0_EnteredCells : public GameStateInterface {
	void onUpdate();
	void onStart();
	void onMessageRecieved(Message *  message);
	void onEnd();
};
class GS1_LockedCells : public GameStateInterface {
	void onUpdate();
	void onStart();
	void onMessageRecieved(Message *  message);
	void onEnd();
};
class GS2_UnlockedCells : public GameStateInterface {
	void onUpdate();
	void onStart();
	void onMessageRecieved(Message *  message);
	void onEnd();
};
class GS3_OpenedChest : public GameStateInterface {
	void onUpdate();
	void onStart();
	void onMessageRecieved(Message *  message);
	void onEnd();
};
class GS4_LoweredCoffin : public GameStateInterface {
	void onUpdate();
	void onStart();
	void onMessageRecieved(Message *  message);
	void onEnd();
};
class GS5_UnlockedCoffin : public GameStateInterface {
	void onUpdate();
	void onStart();
	void onMessageRecieved(Message *  message);
	void onEnd();
};
class GS6_SolvedCoffin : public GameStateInterface {
	void onUpdate();
	void onStart();
	void onMessageRecieved(Message *  message);
	void onEnd();
};
class GS7_TakenBook : public GameStateInterface {
	void onUpdate();
	void onStart();
	void onMessageRecieved(Message *  message);
	void onEnd();
};
class GSR_RestoreRoom : public GameStateInterface {
	void onUpdate();
	void onStart();
	void onMessageRecieved(Message *  message);
	void onEnd();
};
