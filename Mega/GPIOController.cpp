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
	//Serial.print(digitalRead(PIN_CELL0));Serial.println(digitalRead(PIN_CELL1));
	return (digitalRead(PIN_CELL0) == LOW && digitalRead(PIN_CELL1) == LOW);
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
	return (digitalRead(PIN_CHEST_REED) == LOW);
}
// END CHEST
// COFFIN
const int Coffin::PIN_CRANK_REED = A15;
const int Coffin::ROTATIONS_TO_LOWER = 10;
const int Coffin::DEBOUNCE_TIME = 100;
unsigned int Coffin::rotationCount = 0;
unsigned long Coffin::debouncer = 0;
void Coffin::init()
{
	pinMode(PIN_CRANK_REED, INPUT_PULLUP);
	attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(PIN_CRANK_REED), handleISR, FALLING);
}
void Coffin::handleISR()
{
	Serial.println("Coffin ISR");
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
	Serial.print(rotationCount);Serial.print('/');Serial.println(ROTATIONS_TO_LOWER);
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
unsigned int ButtonMatrix::millisPulseStart[BUTTON_COUNT] = {0};
uint8_t ButtonMatrix::shouldPulse[BUTTON_COUNT] = {0};
unsigned int ButtonMatrix::lastUpdate = 0;
const uint8_t ButtonMatrix::CORRECT_SEQUENCE[SEQUENCE_LENGTH] = { 3,4,1,2,2,4 };//{4,4,4,4,4,4};
const int ButtonMatrix::PIN_BUTTONS[BUTTON_COUNT] = {A13,A12,A11,A10};
const int ButtonMatrix::PIN_BUTTONS_PWM[BUTTON_COUNT] = {5,6,7,4};
const unsigned int ButtonMatrix::PULSE_DURATION = 200;
const unsigned int ButtonMatrix::DEBOUNCE_TIME = 500;
const uint8_t ButtonMatrix::UPDATE_DELAY = 15;
const uint8_t ButtonMatrix::BASE_LEVEL = 25;
const uint8_t ButtonMatrix::PULSE_LEVEL = 200;
uint8_t ButtonMatrix::sequence[SEQUENCE_LENGTH] = {0};
uint8_t ButtonMatrix::position = 0;
unsigned long ButtonMatrix::buttonDebouncers[BUTTON_COUNT] = {0};
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
	Serial.print(CORRECT_SEQUENCE[0]);Serial.print(" vs. ");Serial.print(sequence[0]);Serial.println();
	Serial.print(CORRECT_SEQUENCE[1]);Serial.print(" vs. ");Serial.print(sequence[1]);Serial.println();
	Serial.print(CORRECT_SEQUENCE[2]);Serial.print(" vs. ");Serial.print(sequence[2]);Serial.println();
	Serial.print(CORRECT_SEQUENCE[3]);Serial.print(" vs. ");Serial.print(sequence[3]);Serial.println();
	Serial.print(CORRECT_SEQUENCE[4]);Serial.print(" vs. ");Serial.print(sequence[4]);Serial.println();
	Serial.print(CORRECT_SEQUENCE[5]);Serial.print(" vs. ");Serial.print(sequence[5]);Serial.println();
	if (memcmp(sequence, CORRECT_SEQUENCE, sizeof(uint8_t)*SEQUENCE_LENGTH) == 0)
	{
		Serial.println("Correct button sequence detected");
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
					shouldPulse[i]--;
					millisPulseStart[i] = millis();
					analogWrite(PIN_BUTTONS_PWM[i], BASE_LEVEL);
					continue;
				}
				double sinValue = sin(((double)timeDifference / (double)PULSE_DURATION) * PI);
				analogWrite(PIN_BUTTONS_PWM[i], BASE_LEVEL + round(sinValue * PULSE_LEVEL));
			}
			else
				analogWrite(PIN_BUTTONS_PWM[i], BASE_LEVEL);
		}
		lastUpdate = millis();
	}
}
void ButtonMatrix::pulse(int button, uint8_t count)
{
	shouldPulse[button] = count;
	millisPulseStart[button] = millis();
}
void ButtonMatrix::pulseAll(uint8_t count)
{
	pulse(0, count);
	pulse(1, count);
	pulse(2, count);
	pulse(3, count);
}
void ButtonMatrix::disableISR()
{
	disablePinChangeInterrupt(digitalPinToPinChangeInterrupt(PIN_BUTTONS[0]));
	disablePinChangeInterrupt(digitalPinToPinChangeInterrupt(PIN_BUTTONS[1]));
	disablePinChangeInterrupt(digitalPinToPinChangeInterrupt(PIN_BUTTONS[2]));
	disablePinChangeInterrupt(digitalPinToPinChangeInterrupt(PIN_BUTTONS[3]));
}
void ButtonMatrix::enableISR()
{
	enablePinChangeInterrupt(digitalPinToPinChangeInterrupt(PIN_BUTTONS[0]));
	enablePinChangeInterrupt(digitalPinToPinChangeInterrupt(PIN_BUTTONS[1]));
	enablePinChangeInterrupt(digitalPinToPinChangeInterrupt(PIN_BUTTONS[2]));
	enablePinChangeInterrupt(digitalPinToPinChangeInterrupt(PIN_BUTTONS[3]));
}
void ButtonMatrix::pulseRotate()
{
	//analogWrite(PIN_BUTTONS_PWM[0], PULSE_LEVEL);
	for (int k = 0; k < 3;k++);
	{
		for (int i = 1; i < BUTTON_COUNT + 1; i++)
		{
			analogWrite(PIN_BUTTONS_PWM[i - 1], 0);
			analogWrite(PIN_BUTTONS_PWM[i % BUTTON_COUNT], PULSE_LEVEL);
			delay(250);
		}
		analogWrite(PIN_BUTTONS_PWM[0], 0);
	}
}
void ButtonMatrix::handleISR0()
{
	disableISR();
	if (buttonDebouncers[0] + DEBOUNCE_TIME < millis() && digitalRead(PIN_BUTTONS[0])==LOW)
	{
		Serial.print(buttonDebouncers[0]);Serial.print('+');Serial.print(DEBOUNCE_TIME);Serial.print('<');Serial.println(millis());
		if (position < SEQUENCE_LENGTH)
		{
			sequence[position++] = 1;
			pulse(0,1);
		}
	}
	enableISR();
	buttonDebouncers[0] = millis();
}
void ButtonMatrix::handleISR1()
{
	disableISR();
	if (buttonDebouncers[1] + DEBOUNCE_TIME < millis() && digitalRead(PIN_BUTTONS[1]) == LOW)
	{
		Serial.print(buttonDebouncers[1]);Serial.print('+');Serial.print(DEBOUNCE_TIME);Serial.print('<');Serial.println(millis());
		if (position < SEQUENCE_LENGTH)
		{
			sequence[position++] = 2;
			pulse(1,1);
		}
		
	}
	enableISR();
	buttonDebouncers[1] = millis();
}
void ButtonMatrix::handleISR2()
{
	disableISR();
	if (buttonDebouncers[2] + DEBOUNCE_TIME < millis() && digitalRead(PIN_BUTTONS[2]) == LOW)
	{
		Serial.print(buttonDebouncers[2]);Serial.print('+');Serial.print(DEBOUNCE_TIME);Serial.print('<');Serial.println(millis());
		if (position < SEQUENCE_LENGTH)
		{
			sequence[position++] = 3;
			pulse(2,1);
		}
		
	}
	enableISR();
	buttonDebouncers[2] = millis();
}
void ButtonMatrix::handleISR3()
{
	disableISR();
	if (buttonDebouncers[3] + DEBOUNCE_TIME < millis() && digitalRead(PIN_BUTTONS[3]) == LOW)
	{
		Serial.print(buttonDebouncers[3]);Serial.print('+');Serial.print(DEBOUNCE_TIME);Serial.print('<');Serial.println(millis());
		if (position < SEQUENCE_LENGTH)
		{
			sequence[position++] = 4;
			pulse(3,1);
		}
	}
	enableISR();
	buttonDebouncers[3] = millis();
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
const uint8_t SpellRings::PWM_LEVEL_LOW = 1;
const uint8_t SpellRings::PWM_LEVEL_HIGH = 110;
const uint8_t SpellRings::PWM_LEVEL_PULSE = 100;
const uint8_t SpellRings::PULSE_COUNT = 7;
const uint8_t SpellRings::PIN_RELAY = 28;
const uint8_t SpellRings::UPDATE_DELAY = 5;
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
	if (shouldPulse && (lastUpdate + UPDATE_DELAY) < millis())
	{
		unsigned long timeDiff = millis() - pulseStartMillis;
		if (timeDiff > PWM_PULSE_TIME)
		{
			shouldPulse = false;
		}
		double trigVal = -cos( ( (double)timeDiff / (double)PWM_PULSE_TIME )*PULSE_COUNT * 2 * PI );
		uint8_t valShift = round(PWM_LEVEL_PULSE*trigVal);
		analogWrite(PIN_RING_SMALL, PWM_LEVEL_HIGH + valShift);
		analogWrite(PIN_RING_LARGE, PWM_LEVEL_HIGH + valShift);
		lastUpdate = millis();
	}
	else if(!shouldPulse)
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
const uint8_t Devil::PIN_LED_PWM = 8;
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
	attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(PIN_LSWITCH_ARM_LOW), handleArmDownISR, RISING);
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
	//digitalWrite(PIN_ARM_GO_LOW, LOW);
}
void Devil::handleArmDownISR()
{
	analogWrite(PIN_PWM, PWM_LEVEL_HIGH);
	//digitalWrite(PIN_ARM_GO_UP, LOW);
	digitalWrite(PIN_ARM_GO_LOW, LOW);
}
void Devil::free()
{
	beginMoveUp();
}
// END DEVIL
