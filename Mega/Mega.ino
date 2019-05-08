/*
 Name:		Mega.ino
 Created:	2019-04-30 10:43:20 PM
 Author:	Dickbutt
*/

// the setup function runs once when you press reset or power the board


#include <NeoICSerial.h>

#include <RH_ASK.h>
#include <SPI.h>
#include <PinChangeInterrupt.h>
#include "BoardCoordinator.h"

const static int RF_SPEED = 2000;
const static int RF_RX = 52;
const static int RF_TX = 53;
const static int RF_PTT = 10;
const static bool RF_PTT_INV = false;
BoardCoordinator * coordinator = NULL;
CommunicationController * nanoComms = NULL;
WirelessController * wireless = NULL;
NeoICSerial * ICSerial = NULL;
GPIOController * gpio = NULL;
void setup() {
	Serial.begin(9600);
	ICSerial = new NeoICSerial();
	ICSerial->begin(9600);
	Serial.println("Mega booting...");
	nanoComms = new CommunicationController(ICSerial);
	gpio = new GPIOController();
	wireless = new WirelessController(RF_SPEED, RF_RX, RF_TX, RF_PTT, RF_PTT_INV);
	coordinator = new BoardCoordinator(nanoComms, wireless, gpio);
	//coordinator->loadInterface(MEGASTATE_4_LoweredCoffin);
	
	Serial.println("Succesfully started.");
}

// the loop function runs over and over again until power down or reset
void loop() {
	
	coordinator->onUpdate();
}
