#pragma once
#include "RF24.h"
#include <SPI.h>
#include "Message.h"
#include "Arduino.h"
class WirelessController {
private:
	RF24 * driver;
public:
	const static uint8_t MEGA_ADDRESS[];
	const static uint8_t COFFIN_ADDRESS[];
	WirelessController(int cePin, int csPin);
	void sendMessage(Message * m, int repeatCount=1);
	bool hasMessage(Message *& m);
};
