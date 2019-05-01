/*
 Name:		Mega.ino
 Created:	2019-04-30 10:43:20 PM
 Author:	Dickbutt
*/

// the setup function runs once when you press reset or power the board


#include <NeoICSerial.h>
#include <SPI.h>
#include <RH_ASK.h>
#include <PinChangeInterrupt.h>
#include "BoardCoordinator.h"

const static int RF_SPEED = 2000;
const static int RF_RX = 52;
const static int RF_TX = 52;
const static int RF_PTT = 10;
const static bool RF_PTT_INV = false;
BoardCoordinator * coordinator;
CommunicationController * nanoComms;
WirelessController * wireless;
NeoICSerial * ICSerial;
GPIOController * gpio;
void setup() {
	Serial.begin(9600);
	ICSerial = new NeoICSerial();
	ICSerial->begin(9600);
	nanoComms = new CommunicationController(ICSerial);
	gpio = new GPIOController();
	wireless = new WirelessController(RF_SPEED, RF_RX, RF_TX, RF_PTT, RF_PTT_INV);
	coordinator = new BoardCoordinator(nanoComms, wireless, gpio);
}

// the loop function runs over and over again until power down or reset
void loop() {
	coordinator->onUpdate();
}
