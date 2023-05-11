#include "WirelessController.h"

const uint8_t WirelessController::MEGA_ADDRESS[5] = "mega";
const uint8_t WirelessController::COFFIN_ADDRESS[5] = "cffn";
WirelessController::WirelessController(int cePin, int csPin)
{
	driver = new RF24(cePin, csPin);
	driver->begin();
	driver->setPALevel(RF24_PA_LOW);
	driver->setAddressWidth(4);
	driver->setPayloadSize(TOTAL_LENGTH);
	driver->setRetries(15, 15);
	driver->openWritingPipe(WirelessController::COFFIN_ADDRESS);
	driver->openReadingPipe(1, WirelessController::MEGA_ADDRESS);
	driver->startListening();
}
void WirelessController::sendMessage(Message *m, int repeatCount)
{
	uint8_t * mes = Message::toByteArray(m);
	Serial.println("Sending wireless");
	driver->stopListening();
	do {
		driver->write(mes, TOTAL_LENGTH);
	} while(--repeatCount > 0);
	driver->startListening();
	delete[] mes;
}
bool WirelessController::hasMessage(Message *&m)
{
	uint8_t buffer[TOTAL_LENGTH];
	uint8_t len = TOTAL_LENGTH;
	uint8_t pipe;
	if (driver->available(&pipe))
	{
		Serial.println("Got wireless");
		driver->read(&buffer, len);
		m = Message::fromByteArray(buffer);
		return true;
	}
	else 
		return false;
}
