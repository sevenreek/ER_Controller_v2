/*
 Name:		Mega.ino
 Created:	2019-04-30 10:43:20 PM
 Author:	Dickbutt
*/

// the setup function runs once when you press reset or power the board


#include <NeoICSerial.h>

#include <SPI.h>
#include <PinChangeInterrupt.h>
#include "BoardCoordinator.h"

const static uint16_t RF_SPEED = 2000;
const static uint8_t RF_CE = 49;
const static uint8_t RF_CS = 53;
BoardCoordinator * coordinator = NULL;
CommunicationController * nanoComms = NULL;
WirelessController * wireless = NULL;
NeoICSerial * ICSerial = NULL;
GPIOController * gpio = NULL;
int freeRam();
int freeRam() {
	extern int __heap_start, * __brkval;
	int v;
	return (int)& v - (__brkval == 0 ? (int)& __heap_start : (int)__brkval);
}

void setup() {
	Serial.begin(9600);
	ICSerial = new NeoICSerial();
	ICSerial->begin(9600);
	Serial.println("Mega booting...");
	nanoComms = new CommunicationController(ICSerial);
	gpio = new GPIOController();
	gpio->fogmachine.init();
	wireless = new WirelessController(RF_CE, RF_CS);
	coordinator = new BoardCoordinator(nanoComms, wireless, gpio);
	coordinator->loadInterface(0,true);
	//coordinator->loadInterface(MEGASTATE_5_UnlockedCoffin, true);
	nanoComms->pushCommsCleaner();
}

// the loop function runs over and over again until power down or reset
void loop() {
	
	coordinator->onUpdate();
}
