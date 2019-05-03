#include "WirelessController.h"

WirelessController::WirelessController(int speed, int rx, int tx, int ptt, bool ptt_inv)
{
	driver = new RH_ASK(speed, rx, tx, ptt, ptt_inv);
	//Serial.print("Initialized with ");Serial.print(speed);
	//Serial.print(rx);Serial.print(tx);Serial.println(ptt);
	driver->init();
}
void WirelessController::sendMessage(Message *m, int repeatCount)
{
	uint8_t * mes = Message::toByteArray(m);
	//Serial.print("Sending:");
	//Serial.println((char*)mes);
	uint8_t len = TOTAL_LENGTH-1;
	while (repeatCount--)
	{
		//Serial.print("Called send...");
		driver->send(mes,len);
		driver->waitPacketSent();
	}
	delete[] mes;
}
bool WirelessController::hasMessage(Message *&m)
{
	uint8_t buffer[TOTAL_LENGTH];
	uint8_t len = TOTAL_LENGTH;
	if ( driver->recv(buffer, &len) )
	{
		
		if (len == TOTAL_LENGTH-1)
		{
			//Serial.println(len);
			m = Message::fromByteArray(buffer);
			return true;
		}
		else
			return false;
	}
	else 
		return false;
}
