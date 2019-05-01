#include "WirelessController.h"

WirelessController::WirelessController(int speed, int rx, int tx, int ptt, bool ptt_inv)
{
	driver = new RH_ASK(speed, rx, tx, ptt, ptt_inv);
	driver->init();
}
void WirelessController::sendMessage(Message *m, int repeatCount)
{
	char * mes = Message::toCharArray(m);
	int len = strlen(mes);
	while (repeatCount--)
	{
		driver->send((uint8_t*)mes,len);
		driver->waitPacketSent();
	}
}
bool WirelessController::hasMessage(Message *m)
{
	uint8_t buffer[TOTAL_LENGTH];
	uint8_t len = TOTAL_LENGTH;
	if ( driver->recv(buffer, &len) )
	{
		if (len == TOTAL_LENGTH)
		{
			Message * m = new Message((char*)buffer);
			return true;
		}
		else
			return false;
	}
	else 
		return false;
}