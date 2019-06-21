#include "GPIOController.h"
// CELLS
const uint8_t Cells::PIN_CELL0 = A9; // pin of the reed switch of the cell door(probably left); reed grounds the pin when closed
const uint8_t Cells::PIN_CELL1 = A8; // pin of the reed switch of the cell door(probably right); reed grounds the pin when closed
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
bool Cells::areUnlocked()
{
	//Serial.print(digitalRead(PIN_CELL0));Serial.println(digitalRead(PIN_CELL1));
	return (digitalRead(PIN_CELL0) == HIGH && digitalRead(PIN_CELL1) == HIGH);
}
// END CELLS
// CHEST
const uint8_t Chest::PIN_CHEST_REED = A14; // pin of the reed switch of the chest; reed grounds the pin when closed(so when chest is closed); 
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
const uint8_t Coffin::PIN_CRANK_REED = A15; // pin of the reed switch of the crank that lowers the coffin;
const uint16_t Coffin::ROTATIONS_TO_LOWER = 90; // rotation count that is considered as the coffin being surely lowered
const uint16_t Coffin::ROTATIONS_TO_LOWER_POSSIBLE = 70; // rotation count at which the coffin might have been lowered; a timer is started if no more rotations occur
const uint16_t Coffin::DEBOUNCE_TIME = 250; // crank reed switch debounce time; this should probably set to the lowest time that a player is able to rotate the crank in
											// after detecting a state change on the crank reed further changes occuring in this time will be ignored
uint16_t Coffin::rotationCount = 0;
const uint16_t Coffin::UNCHANGED_FOR_TIME = 4000; // when the ROTATIONS_TO_LOWER_POSSIBLE is exceeded and no more rotations occur during this time frame
												  // the coffin is considered to have been lowered and buttons are turned on
unsigned long Coffin::debouncer = 0;
unsigned long Coffin::unchangedFor = 0;
unsigned long Coffin::lastMillis = 0;
void Coffin::init()
{
	rotationCount = 0;
	unchangedFor = 0;
	pinMode(PIN_CRANK_REED, INPUT_PULLUP);
	attachPinChangeInterrupt(digitalPinToPinChangeInterrupt(PIN_CRANK_REED), handleISR, FALLING);
	unchangedFor = 0;
	lastMillis = millis();
}
void Coffin::handleISR()
{
	//Serial.println("Coffin ISR");
	if ( (digitalRead(PIN_CRANK_REED) == LOW) && ( (debouncer + DEBOUNCE_TIME) < millis() ) )
	{
		debouncer = millis();
		lastMillis = debouncer;
		rotationCount++;
		Serial.println(rotationCount);
		unchangedFor = 0;
	}
}
void Coffin::free()
{
	detachPinChangeInterrupt(digitalPinToPinChangeInterrupt(PIN_CRANK_REED));
	rotationCount = 0;
}
bool Coffin::isLowered()
{
	//Serial.print(rotationCount);Serial.print('/');Serial.println(ROTATIONS_TO_LOWER);
	unchangedFor += millis() - lastMillis;
	lastMillis = millis();
	if (rotationCount >= ROTATIONS_TO_LOWER_POSSIBLE && unchangedFor >= UNCHANGED_FOR_TIME)
		return true;
	else if (rotationCount >= ROTATIONS_TO_LOWER)
		return true;
	else
		return false;
	delay(5);
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
//unsigned long ButtonMatrix::millisPulseStart[BUTTON_COUNT] = {0};
uint8_t ButtonMatrix::shouldPulse[BUTTON_COUNT] = {0};
unsigned long ButtonMatrix::lastUpdate = 0;
const uint8_t ButtonMatrix::CORRECT_SEQUENCE[SEQUENCE_LENGTH] = { 3,4,1,2,2,4 }; // correct sequence of button inputs
/* 3 - cow; 4 - goat; 1 - sheep; 2 - bull
  4 goat  | 3 cow
 -----------------
  1 sheep | 2 bull
*/
const uint8_t ButtonMatrix::PIN_BUTTONS[BUTTON_COUNT] = {A13,A12,A11,A10}; // pins connected to the buttons that ground them;
const uint8_t ButtonMatrix::PIN_BUTTONS_PWM[BUTTON_COUNT] = {5,6,7,4}; // pins that control the pwm outputs controlling the LEDs behind each button
const unsigned int ButtonMatrix::PULSE_DURATION = 200; // duration in ~ms for which a button will pulse after being pressed
const unsigned int ButtonMatrix::DEBOUNCE_TIME = 200; // duration for which consecutive button presses(or contacts shaking) will be ignored
const uint8_t ButtonMatrix::ALLBUTTONMASK = 0b00111100; // mask of the port that the buttons are connected to; do not change unless PIN_BUTTONS changes;
const uint8_t ButtonMatrix::UPDATE_DELAY = 15; // delay between consecutive updates of the PWM(led brightness); could potentially be 0
const uint8_t ButtonMatrix::BASE_LEVEL = 15; // default PWM level for a button LED (max. 255)
bool ButtonMatrix::canISR = false;
const uint8_t ButtonMatrix::PULSE_LEVEL = 100; // half of the maximum PWM level a button will reach once pressed; the sum of 2*PULSE_LEVEL+BASE_LEVEL should be lower than 255
uint8_t ButtonMatrix::sequence[SEQUENCE_LENGTH] = {0};
uint8_t ButtonMatrix::position = 0;
unsigned long ButtonMatrix::buttonDebouncers[BUTTON_COUNT] = {0};
void ButtonMatrix::init()
{
	for (int i = 0; i < BUTTON_COUNT; i++)
	{
		pinMode(PIN_BUTTONS[i], INPUT_PULLUP);
		digitalWrite(PIN_BUTTONS[i], HIGH);
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
	for (int i = 0; i < BUTTON_COUNT; i++)
	{
		analogWrite(PIN_BUTTONS_PWM[i], 0);
	}
	clearSequence();
}
bool ButtonMatrix::isCorrect()
{
	if (memcmp(sequence, CORRECT_SEQUENCE, sizeof(uint8_t)*SEQUENCE_LENGTH) == 0)
	{
		//Serial.println("Correct button sequence detected");
		return true;
	}
	return false;
}
void ButtonMatrix::updatePWMs()
{
	//if (lastUpdate + UPDATE_DELAY < millis())
	//{
		for (int i = 0; i < BUTTON_COUNT; i++)
		{
			if (shouldPulse[i])
			{
				unsigned int timeDifference = millis() - /*millisPulseStart*/buttonDebouncers[i];
				if (timeDifference > PULSE_DURATION)
				{
					shouldPulse[i]--;
					/*millisPulseStart*/buttonDebouncers[i] = millis();
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
		//Serial.println(PINK, BIN);
		if(PINK && ALLBUTTONMASK == ALLBUTTONMASK) // if all buttons are high they can be pressed
			canISR = true;
	//}
}
void ButtonMatrix::clearSequence()
{
	memset(sequence, 0, SEQUENCE_LENGTH * sizeof(uint8_t));
}
void ButtonMatrix::pulse(int button, uint8_t count)
{
	shouldPulse[button] = count;
	//millisPulseStart[button] = millis();
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
	if (!canISR)
		return;
	canISR = false;
	//disableISR();
	//Serial.println("0");
	if ( DEBOUNCE_TIME < millis() - buttonDebouncers[0])
	{
		//Serial.print(buttonDebouncers[0]);Serial.print('+');Serial.print(DEBOUNCE_TIME);Serial.print('<');Serial.println(millis());
		buttonDebouncers[0] = millis();
		//Serial.println("0!");
		
		if (position < SEQUENCE_LENGTH)
		{
			sequence[position++] = 1;
			pulse(0,1);
		}
	}
	//enableISR();
	delay(50);
	return;
	
}
void ButtonMatrix::handleISR1()
{
	if (!canISR)
		return;
	canISR = false;
	//disableISR();
	//Serial.println("1");
	if ( DEBOUNCE_TIME < millis() - buttonDebouncers[1]  )
	{
		//Serial.print(buttonDebouncers[1]);Serial.print('+');Serial.print(DEBOUNCE_TIME);Serial.print('<');Serial.println(millis());
		buttonDebouncers[1] = millis();
		//Serial.println("1!");
		
		if (position < SEQUENCE_LENGTH)
		{
			sequence[position++] = 2;
			pulse(1,1);
		}
		
	}
	//enableISR();
	delay(50);
	return;
	
}
void ButtonMatrix::handleISR2()
{

	if (!canISR)
		return;
	canISR = false;
	//disableISR();
	//Serial.println("2");
	if (DEBOUNCE_TIME < millis() - buttonDebouncers[2])
	{
		//Serial.print(buttonDebouncers[2]);Serial.print('+');Serial.print(DEBOUNCE_TIME);Serial.print('<');Serial.println(millis());
		buttonDebouncers[2] = millis();
		//Serial.println("2!");
		
		if (position < SEQUENCE_LENGTH)
		{
			sequence[position++] = 3;
			pulse(2,1);
		}
		
	}
	//enableISR();
	delay(50);
	return;
}
void ButtonMatrix::handleISR3()
{
	if (!canISR)
		return;
	canISR = false;
	//disableISR();
	//Serial.println("3");
	if ( DEBOUNCE_TIME < (millis() - buttonDebouncers[3]) )
	{
		//Serial.print(buttonDebouncers[3]);Serial.print('+');Serial.print(DEBOUNCE_TIME);Serial.print('<');Serial.println(millis());
		buttonDebouncers[3] = millis();
		//Serial.println("3!");
		
		if (position < SEQUENCE_LENGTH)
		{
			sequence[position++] = 4;
			pulse(3,1);
		}
	}
	//enableISR();
	delay(50);
	return;
}
// END BUTTONMATRIX
// HANGMAN
const uint8_t Hangman::PIN_MAGNET = 30; // pin of the electromagnet that drops the hangman
const uint8_t Hangman::PIN_BOOK_REED = A0; // pin of the reed switch under the book magnet
const uint16_t Hangman::TIME_TO_BE_TAKEN = 500; // when the book is taken out(PIN_BOOK_REED is HIGH) a timer starts with this value; 
												// if the pin is HIGH for this time(the book is out for so long) it is considered to 
												// have been taken out and next room state loads;
void Hangman::init()
{
	pinMode(PIN_MAGNET, OUTPUT);
	pinMode(PIN_BOOK_REED, INPUT_PULLUP);
	digitalWrite(PIN_BOOK_REED, HIGH);
	currentTimeTaken = 0;
	lastMillis = 0;
}
void Hangman::drop()
{
	digitalWrite(PIN_MAGNET, HIGH);
	delay(50);
	digitalWrite(PIN_MAGNET, LOW);
}
bool Hangman::isBookTaken()
{
	digitalWrite(PIN_BOOK_REED, HIGH);
	Serial.println(digitalRead(PIN_BOOK_REED));
	if(lastMillis == 0)
		lastMillis = millis();
	if (digitalRead(PIN_BOOK_REED) == HIGH)
	{
		currentTimeTaken += millis() - lastMillis;
		Serial.println(currentTimeTaken);
		if (currentTimeTaken >= TIME_TO_BE_TAKEN)
			return true;
	}
	lastMillis = millis();
	delay(2);
	return false;
}
void Hangman::free()
{

}
// END HANGMAN
// RING
const uint8_t SpellRings::PIN_RING_LARGE = 3; // PWM pin that controls the larger ring ledstrip brightness
const uint8_t SpellRings::PIN_RING_SMALL = 2; // PWM pin that controls the smaller ring ledstrip brightness
const uint8_t SpellRings::PWM_LEVEL_LOW = 1; // PWM level for the default state of the ring(not pulsating)
const uint8_t SpellRings::PWM_LEVEL_HIGH = 110; // base PWM state when the ring is pulsating
const uint8_t SpellRings::PWM_LEVEL_PULSE = 100; // maximum deviation(amplitude) of brightness oscillation
const uint8_t SpellRings::PIN_RELAY = 28; // the pin that controlls the relay necessary to turn the LED strips on
const uint8_t SpellRings::UPDATE_DELAY = 5; // delay between consecutive updates of LEDs brightness
const unsigned int SpellRings::PWM_PULSE_PERIOD = 1000; // the period of a single pulse in ~ms
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
		double trigVal = -cos( ( (double)timeDiff / (double)PWM_PULSE_PERIOD ) * 2 * PI );
		uint8_t valShift = round(PWM_LEVEL_PULSE*trigVal);
		uint8_t glowLevel = shouldGlow ? PWM_LEVEL_HIGH + valShift : 0;
		analogWrite(PIN_RING_SMALL, glowLevel);
		analogWrite(PIN_RING_LARGE, glowLevel);
		lastUpdate = millis();
	}
	else if(!shouldPulse)
	{
		uint8_t glowLevel = shouldGlow ? PWM_LEVEL_LOW : 0;
		analogWrite(PIN_RING_SMALL, glowLevel);
		analogWrite(PIN_RING_LARGE, glowLevel);
	}
}
void SpellRings::pulse()
{
	pulseStartMillis = millis();
	shouldPulse = true;
}
void SpellRings::stopPulse()
{
	shouldPulse = false;
}
void SpellRings::free()
{
	analogWrite(PIN_RING_SMALL, 0);
	analogWrite(PIN_RING_LARGE, 0);
}
void SpellRings::enable()
{
	shouldGlow = true;
}
void SpellRings::kill()
{
	shouldGlow = false;
}
// END RING
// FOGMACHINE
const uint8_t FogMachine::PIN_FOG_RELAY = 35;
const uint8_t FogMachine::PIN_FOG_ON_STATE = LOW;
void FogMachine::init()
{
	pinMode(PIN_FOG_RELAY, OUTPUT);
	digitalWrite(PIN_FOG_RELAY, !PIN_FOG_ON_STATE);
}
void FogMachine::run(int time)
{
	digitalWrite(PIN_FOG_RELAY, PIN_FOG_ON_STATE);
	delay(time);
	digitalWrite(PIN_FOG_RELAY, !PIN_FOG_ON_STATE);
}
void FogMachine::free()
{
}
// END FOGMACHINE
// DEVIL
const uint8_t Devil::PIN_POWER = 22; // pin controlling the relay that puts power to the devil engine
const uint8_t Devil::PIN_PWM = 11; // pin that controlls the speed at which the engine moves(PWM)
const uint8_t Devil::PWM_LEVEL_HIGH = 200; // the PWM level to which set PIN_PWM when moving the arm; max is 255
const uint8_t Devil::PWM_LEVEL_LOW = 0; // PWM level to which set PIN_PWM when not moving; should probably be 0
const uint8_t Devil::PIN_LSWITCH_ARM_LOW = 12; // the pin that changes state when the arm is fully lowered
const uint8_t Devil::PIN_LSWITCH_ARM_HIGH = 13; // the pin that changes state when the arm is fully raised
const uint8_t Devil::PIN_ARM_GO_UP = 23; // pin that is to be HIGH when the H bridge is to move the devil's arm up
const uint8_t Devil::PIN_ARM_GO_LOW = 25; // pin that is to be HIGH when the H bridge is to move the devil's arm down
const uint8_t Devil::PIN_CURTAIN = 32; // pin that controlls the relay connected to the curtain electromagnet
const uint8_t Devil::PIN_LED_PWM = 8; // this is actually a relay now, so use digitalWrite
const uint8_t Devil::PIN_LED_PWM_ON_LEVEL = 255; // the led pin is no longer PWM so this has to be 255; see above
void Devil::init()
{
	pinMode(PIN_CURTAIN, OUTPUT);
	pinMode(PIN_LED_PWM, OUTPUT);
	//analogWrite(PIN_LED_PWM, 0);
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
	if (digitalRead(PIN_LSWITCH_ARM_HIGH) == LOW)
		return;
	analogWrite(PIN_PWM, PWM_LEVEL_HIGH);
	digitalWrite(PIN_ARM_GO_UP, HIGH);
	digitalWrite(PIN_ARM_GO_LOW, LOW);
}
void Devil::beginMoveDown()
{
	if (digitalRead(PIN_LSWITCH_ARM_LOW) == HIGH)
		return;
	analogWrite(PIN_PWM, PWM_LEVEL_HIGH);
	digitalWrite(PIN_ARM_GO_UP, LOW);
	digitalWrite(PIN_ARM_GO_LOW, HIGH);
}
void Devil::lightLed()
{
	
	digitalWrite(PIN_LED_PWM, HIGH);
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
	digitalWrite(PIN_ARM_GO_LOW, LOW);
	digitalWrite(PIN_ARM_GO_UP, LOW);
	analogWrite(PIN_PWM, 0);
	digitalWrite(PIN_LED_PWM, LOW);
	//beginMoveUp();
}
// END 
