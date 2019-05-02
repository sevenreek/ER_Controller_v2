#pragma once
#include "Arduino.h"
#include "WirelessController.h"
#include <PinChangeInterrupt.h>

class GameObject {
public: 
	virtual void init() = 0;
	virtual void free() = 0;
};
class Cells : public GameObject{
private:
	const static uint8_t PIN_CELL0;
	const static uint8_t PIN_CELL1;
public:
	void init();
	void free();
	bool areLocked();
};
class Chest : public GameObject {
private:
	const static uint8_t PIN_CHEST_REED;
public:
	void init();
	void free();
	bool isLocked();
};

class Coffin : public GameObject {
private:
public:
	static const int PIN_CRANK_REED;
	static const int ROTATIONS_TO_LOWER;
	static const int DEBOUNCE_TIME;
	static unsigned int rotationCount;
	static unsigned int debouncer;
	static void handleISR();
	void init();
	void free();
	bool isLowered();
	bool open(WirelessController * wireless);
};
static const int SEQUENCE_LENGTH = 6;
static const int BUTTON_COUNT = 4;
class ButtonMatrix : public GameObject {
private:
	static unsigned int millisPulseStart[BUTTON_COUNT];
	static uint8_t shouldPulse[BUTTON_COUNT];
	static unsigned int lastUpdate;
public:
	static const int PIN_BUTTONS[BUTTON_COUNT];
	static const int PIN_BUTTONS_PWM[BUTTON_COUNT];
	static const uint8_t CORRECT_SEQUENCE[SEQUENCE_LENGTH];
	static const uint8_t BASE_LEVEL;
	static const uint8_t PULSE_LEVEL;
	static const unsigned int PULSE_DURATION;
	static const unsigned int DEBOUNCE_TIME;
	static const uint8_t UPDATE_DELAY;
	static uint8_t sequence[SEQUENCE_LENGTH];
	static uint8_t position;
	static unsigned int buttonDebouncers[BUTTON_COUNT];
	static void handleISR0();
	static void handleISR1();
	static void handleISR2();
	static void handleISR3();
	static void pulse(int button, uint8_t count);
	static void pulseRotate();
	void pulseAll(uint8_t count);
	bool isCorrect();
	void updatePWMs();
	void init();
	void free();
};
class Hangman : public GameObject {
public:
	static const uint8_t PIN_MAGNET;
	static const uint8_t PIN_BOOK_REED;
	void init();
	void free();
	void drop();
	bool isBookTaken();
};
class SpellRings : public GameObject {
private:
	bool shouldPulse;
	unsigned int lastUpdate;
public:
	static const uint8_t PIN_RELAY;
	static const uint8_t PIN_RING_LARGE;
	static const uint8_t PIN_RING_SMALL;
	static const uint8_t PWM_LEVEL_LOW;
	static const uint8_t PWM_LEVEL_HIGH;
	static const uint8_t PWM_LEVEL_PULSE;
	static const uint8_t UPDATE_DELAY;
	static const unsigned int PWM_PULSE_TIME;
	unsigned int pulseStartMillis;
	void init();
	void free();
	void dim();
	void updatePWMs();
	void pulse();
};
class Devil : public GameObject {
public:
	static const uint8_t PIN_POWER;
	static const uint8_t PIN_PWM;
	static const uint8_t PWM_LEVEL_HIGH;
	static const uint8_t PWM_LEVEL_LOW;
	static const uint8_t PIN_LSWITCH_ARM_LOW;
	static const uint8_t PIN_LSWITCH_ARM_HIGH;
	static const uint8_t PIN_ARM_GO_UP;
	static const uint8_t PIN_ARM_GO_LOW;
	static const uint8_t PIN_CURTAIN;
	static const uint8_t PIN_LED_PWM;
	static const uint8_t PIN_LED_PWM_ON_LEVEL;
	void init();
	void beginMoveUp();
	void beginMoveDown();
	void lightLed();
	void dropCurtain();
	static void handleArmDownISR();
	static void handleArmUpISR();
	void free();
};

class GPIOController {
public:
	Cells cells;
	Chest chest;
	Coffin coffin;
	ButtonMatrix buttons;
	Hangman hangman;
	SpellRings rings;
	Devil devil;
};