#include "GPIOController.h"
// CELLS
const uint8_t Cells::PIN_CELL0 = A9;
const uint8_t Cells::PIN_CELL1 = A8;
void Cells::init()
{
	pinMode(PIN_CELL0, INPUT_PULLUP);
	pinMode(PIN_CELL1, INPUT_PULLUP);
}
void Cells::free()
{

}
bool Cells::areLocked()
{
	return (digitalRead(PIN_CELL0) == HIGH && digitalRead(PIN_CELL1) == HIGH);
}
// END CELLS
// CHEST
const uint8_t Chest::PIN_CHEST_REED = A14;
void Chest::init()
{
	pinMode(PIN_CHEST_REED, INPUT_PULLUP);
}
void Chest::free()
{

}
bool Chest::isLocked()
{
	return (digitalRead(PIN_CHEST_REED) == HIGH);
}
// END CHEST
// COFFIN
const int Coffin::PIN_CRANK_REED = A15;
const int Coffin::ROTATIONS_TO_LOWER = 10;
const int Coffin::DEBOUNCE_TIME = 2000;
unsigned int Coffin::rotationCount = 0;
unsigned int Coffin::debouncer = 0;
void Coffin::init()
{
	pinMode(PIN_CRANK_REED, INPUT_PULLUP);
	attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(PIN_CRANK_REED), handleISR, FALLING);
}
void Coffin::handleISR()
{
	if ( (digitalRead(PIN_CRANK_REED) == LOW) && ( (debouncer + DEBOUNCE_TIME) < millis() ) )
	{
		debouncer = millis();
		rotationCount++;
	}
}
void Coffin::free()
{
	detachPinChangeInterrupt(digitalPinToPinChangeInterrupt(PIN_CRANK_REED));
}
bool Coffin::isLowered()
{
	return (rotationCount >= ROTATIONS_TO_LOWER);
}
bool Coffin::open(WirelessController * wireless)
{
	uint8_t command = CMD_COFFIN_UNLOCKED;
	int arg = 0;
	Message * m = new Message(SNDR_MEGA, MTYPE_EVENT, command, arg);
	wireless->sendMessage(m, WirelessController::REPEAT_COUNT);
	delete m;
}
// END COFFIN


// BUTTONMATRIX
const uint8_t ButtonMatrix::CORRECT_SEQUENCE[SEQUENCE_LENGTH] = {4,4,4,4,4};
const int ButtonMatrix::PIN_BUTTONS[BUTTON_COUNT] = {A13,A12,A11,A10};
const int ButtonMatrix::PIN_BUTTONS_PWM[BUTTON_COUNT] = {5,6,7,4};
const unsigned int ButtonMatrix::PULSE_DURATION = 200;
const unsigned int ButtonMatrix::DEBOUNCE_TIME = 900;
const uint8_t ButtonMatrix::UPDATE_DELAY = 15;
const uint8_t ButtonMatrix::BASE_LEVEL = 50;
const uint8_t ButtonMatrix::PULSE_LEVEL = 100;
uint8_t ButtonMatrix::sequence[SEQUENCE_LENGTH] = {0};
uint8_t ButtonMatrix::position = 0;
unsigned int ButtonMatrix::buttonDebouncers[BUTTON_COUNT] = {0};
void ButtonMatrix::init()
{
	for (int i = 0; i < BUTTON_COUNT; i++)
	{
		pinMode(PIN_BUTTONS[i], INPUT_PULLUP);
		pinMode(PIN_BUTTONS_PWM[i], OUTPUT);
	}
	attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(PIN_BUTTONS[0]), handleISR0, FALLING);
	attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(PIN_BUTTONS[1]), handleISR1, FALLING);
	attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(PIN_BUTTONS[2]), handleISR2, FALLING);
	attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(PIN_BUTTONS[3]), handleISR3, FALLING);
}
void ButtonMatrix::free()
{
	detachPinChangeInterrupt(digitalPinToPinChangeInterrupt(PIN_BUTTONS[0]));
	detachPinChangeInterrupt(digitalPinToPinChangeInterrupt(PIN_BUTTONS[1]));
	detachPinChangeInterrupt(digitalPinToPinChangeInterrupt(PIN_BUTTONS[2]));
	detachPinChangeInterrupt(digitalPinToPinChangeInterrupt(PIN_BUTTONS[3]));
}
bool ButtonMatrix::isCorrect()
{
	if (position >= SEQUENCE_LENGTH && memcmp(sequence, CORRECT_SEQUENCE, sizeof(uint8_t)*SEQUENCE_LENGTH) == 0)
	{
		return true;
	}
	return false;
}
void ButtonMatrix::updatePWMs()
{
	if (lastUpdate + UPDATE_DELAY < millis())
	{
		for (int i = 0; i < BUTTON_COUNT; i++)
		{
			if (shouldPulse[i])
			{
				unsigned int timeDifference = millis() - millisPulseStart[i];
				if (timeDifference > PULSE_DURATION)
				{
					shouldPulse[i] = false;
					analogWrite(PIN_BUTTONS_PWM[i], BASE_LEVEL);
					continue;
				}
				double sinValue = sin(((double)timeDifference / (double)PULSE_DURATION) * PI);
				analogWrite(PIN_BUTTONS_PWM[i], BASE_LEVEL + round(sinValue * PULSE_LEVEL));
			}
		}
		lastUpdate = millis();
	}
}
void ButtonMatrix::pulse(int button)
{
	shouldPulse[button] = true;
	millisPulseStart[button] = millis();
}
void ButtonMatrix::handleISR0()
{
	if (buttonDebouncers[0] + DEBOUNCE_TIME < millis())
	{
		if (position < SEQUENCE_LENGTH)
		{
			sequence[position++] = 1;
		}
		buttonDebouncers[0] = millis();
	}
}
void ButtonMatrix::handleISR1()
{
	if (buttonDebouncers[1] + DEBOUNCE_TIME < millis())
	{
		if (position < SEQUENCE_LENGTH)
		{
			sequence[position++] = 2;
		}
		buttonDebouncers[1] = millis();
	}
}
void ButtonMatrix::handleISR2()
{
	if (buttonDebouncers[2] + DEBOUNCE_TIME < millis())
	{
		if (position < SEQUENCE_LENGTH)
		{
			sequence[position++] = 3;
		}
		buttonDebouncers[2] = millis();
	}
}
void ButtonMatrix::handleISR3()
{
	if (buttonDebouncers[3] + DEBOUNCE_TIME < millis())
	{
		if (position < SEQUENCE_LENGTH)
		{
			sequence[position++] = 4;
		}
		buttonDebouncers[3] = millis();
	}
}
// END BUTTONMATRIX
// HANGMAN
const uint8_t Hangman::PIN_MAGNET = 30;
const uint8_t Hangman::PIN_BOOK_REED = 10;
void Hangman::init()
{
	pinMode(PIN_MAGNET, OUTPUT);
	pinMode(PIN_BOOK_REED, INPUT_PULLUP);
}
void Hangman::drop()
{
	digitalWrite(PIN_MAGNET, HIGH);
	delay(200);
	digitalWrite(PIN_MAGNET, LOW);
}
bool Hangman::isBookTaken()
{
	return (digitalRead(PIN_BOOK_REED) == HIGH);
}
void Hangman::free()
{

}
// END HANGMAN
// RING
const uint8_t SpellRings::PIN_RING_LARGE = 3;
const uint8_t SpellRings::PIN_RING_SMALL = 2;
const uint8_t SpellRings::PWM_LEVEL_LOW = 10;
const uint8_t SpellRings::PWM_LEVEL_HIGH = 110;
const uint8_t SpellRings::PWM_LEVEL_PULSE = 100;
const uint8_t SpellRings::PIN_RELAY = 28;
const uint8_t SpellRings::UPDATE_DELAY = 15;
const unsigned int SpellRings::PWM_PULSE_TIME = 7000;
void SpellRings::init()
{
	pinMode(PIN_RELAY, OUTPUT);
	digitalWrite(PIN_RELAY, LOW);
	pinMode(PIN_RING_LARGE, OUTPUT);
	pinMode(PIN_RING_SMALL, OUTPUT);
	analogWrite(PIN_RING_SMALL, 0.25f*PWM_LEVEL_LOW);
	analogWrite(PIN_RING_LARGE, 0.25f*PWM_LEVEL_LOW);
	delay(50);
	analogWrite(PIN_RING_SMALL, 0.5f*PWM_LEVEL_LOW);
	analogWrite(PIN_RING_LARGE, 0.5f*PWM_LEVEL_LOW);
	delay(50);
	analogWrite(PIN_RING_SMALL, 0.75f*PWM_LEVEL_LOW);
	analogWrite(PIN_RING_LARGE, 0.75f*PWM_LEVEL_LOW);
	delay(50);
	analogWrite(PIN_RING_SMALL, PWM_LEVEL_LOW);
	analogWrite(PIN_RING_LARGE, PWM_LEVEL_LOW);
}
void SpellRings::dim()
{
	shouldPulse = false;
}
void SpellRings::updatePWMs()
{
	if (shouldPulse && lastUpdate + UPDATE_DELAY < millis())
	{
		unsigned int timeDiff = millis() - pulseStartMillis;
		if (timeDiff > PWM_PULSE_TIME)
		{
			shouldPulse = false;
		}
		double trigVal = -cos( ( (double)timeDiff / (double)PWM_PULSE_TIME ) * 2 * PI );
		uint8_t valShift = round(PWM_LEVEL_PULSE*trigVal);
		analogWrite(PIN_RING_SMALL, PWM_LEVEL_HIGH + valShift);
		analogWrite(PIN_RING_LARGE, PWM_LEVEL_HIGH + valShift);
		lastUpdate = millis();
	}
	else
	{
		analogWrite(PIN_RING_SMALL, PWM_LEVEL_LOW);
		analogWrite(PIN_RING_LARGE, PWM_LEVEL_LOW);
	}
}
void SpellRings::pulse()
{
	pulseStartMillis = millis();
	shouldPulse = true;
}
void SpellRings::free()
{

}
// END RING
// DEVIL
const uint8_t Devil::PIN_POWER = 22;
const uint8_t Devil::PIN_PWM = 11;
const uint8_t Devil::PWM_LEVEL_HIGH = 200;
const uint8_t Devil::PWM_LEVEL_LOW = 0;
const uint8_t Devil::PIN_LSWITCH_ARM_LOW = 12;
const uint8_t Devil::PIN_LSWITCH_ARM_HIGH = 13;
const uint8_t Devil::PIN_ARM_GO_UP = 23;
const uint8_t Devil::PIN_ARM_GO_LOW = 25;
const uint8_t Devil::PIN_CURTAIN = 32;
const uint8_t Devil::PIN_LED_PWM = 11;
const uint8_t Devil::PIN_LED_PWM_ON_LEVEL = 255;
void Devil::init()
{
	pinMode(PIN_CURTAIN, OUTPUT);
	pinMode(PIN_LED_PWM, OUTPUT);
	pinMode(PIN_POWER, OUTPUT);
	pinMode(PIN_PWM, OUTPUT);
	pinMode(PIN_ARM_GO_UP, OUTPUT);
	pinMode(PIN_ARM_GO_LOW, OUTPUT);
	pinMode(PIN_LSWITCH_ARM_LOW, INPUT_PULLUP);
	pinMode(PIN_LSWITCH_ARM_HIGH, INPUT_PULLUP);
	attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(PIN_LSWITCH_ARM_LOW), handleArmDownISR, FALLING);
	attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(PIN_LSWITCH_ARM_HIGH), handleArmUpISR, FALLING);
}
void Devil::beginMoveUp()
{
	analogWrite(PIN_PWM, PWM_LEVEL_HIGH);
	digitalWrite(PIN_ARM_GO_UP, HIGH);
	digitalWrite(PIN_ARM_GO_LOW, LOW);
}
void Devil::beginMoveDown()
{
	analogWrite(PIN_PWM, PWM_LEVEL_HIGH);
	digitalWrite(PIN_ARM_GO_UP, LOW);
	digitalWrite(PIN_ARM_GO_LOW, HIGH);
}
void Devil::lightLed()
{
	analogWrite(PIN_LED_PWM, PIN_LED_PWM_ON_LEVEL);
}
void Devil::dropCurtain()
{
	digitalWrite(PIN_CURTAIN, HIGH);
	delay(200);
	digitalWrite(PIN_CURTAIN, LOW);
}
void Devil::handleArmUpISR()
{
	analogWrite(PIN_PWM, PWM_LEVEL_HIGH);
	digitalWrite(PIN_ARM_GO_UP, LOW);
	digitalWrite(PIN_ARM_GO_LOW, LOW);
}
void Devil::handleArmDownISR()
{
	analogWrite(PIN_PWM, PWM_LEVEL_HIGH);
	digitalWrite(PIN_ARM_GO_UP, LOW);
	digitalWrite(PIN_ARM_GO_LOW, LOW);
}
void Devil::free()
{
	beginMoveUp();
}
// END DEVIL
