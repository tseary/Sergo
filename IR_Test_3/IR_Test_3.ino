/*
	Name:       IR_Test_3.ino
	Created:	9/21/2019 10:29:40 AM
	Author:     SCROLLSAW\Thomas
*/

#include <Servo.h>

const uint8_t IR_RX_PIN = 8;	// Arduino pin 8 is PCINT0 on ATMEGA328P

// Channel
const uint8_t myChannelAddress = 0 << 1;	// [0-3] << 1

// Message receiving
const uint8_t MESSAGE_LENGTH = 16;
volatile uint32_t
lastMicros = 0,
pulseLength = 0;
// The MSB is the first bit received
volatile uint16_t volMessage = 0x0000;
volatile uint8_t volMessageLength = 0;
volatile bool messageReady = false;

// Message parsing
bool lastToggleBit = false;
const bool verifyToggleBit = false;	// Ignore the toggle bit

/*const uint8_t
MODE_NOT_USED = 0b000,
MODE_COMBO_DIRECT = 0b001,
MODE_SINGLE_PIN_CONTINUOUS = 0b010,
MODE_SINGLE_PIN_TIMEOUT = 0b010,
MODE_SINGLE_OUTPUT = 0b100;*/

const uint8_t
COMBO_DIRECT_FLOAT = 0b00,
COMBO_DIRECT_FWD = 0b01,
COMBO_DIRECT_REV = 0b10,
COMBO_DIRECT_BRAKE = 0b11;

// Servo
const uint8_t SERVO_OUTPUT_PIN = 2;
Servo myServo;  // create servo object to control a servo

// Timeout
const uint32_t TIMEOUT_MILLIS = 2000;
uint32_t lastCommandMillis = 0;
bool doTimeout = false;

void setup() {
	Serial.begin(250000);

	// Pilot light
	pinMode(LED_BUILTIN, OUTPUT);

	// Set up servo
	myServo.attach(SERVO_OUTPUT_PIN);
	myServo.write(90);	// Position from 0-180 degrees

	// Set up pin change interrupts on RX
	pinMode(IR_RX_PIN, INPUT_PULLUP);
	// See https://thewanderingengineer.com/2014/08/11/pin-change-interrupts-on-attiny85/
	PCICR |= bit(PCIE0);	// turns on pin change interrupts
	//PCMSK0 |= (1 << PCINT0);   // set PCINT0 to trigger an interrupt on state change 
	PCMSK0 |= bit(PCINT0);   // set PCINT0 to trigger an interrupt on state change 

	sei();                     // turn on interrupts
}

void loop() {
	checkMessage();
	checkTimeout();
}

void checkMessage() {
	if (!messageReady) return;

	// Make a copy of the volatile variable
	cli();
	messageReady = false;
	uint16_t message = volMessage;
	sei();

	/**************************************************************************
	 * Message Parsing
	 *
	 * The message structure is 16 bits:
	 * TECC aMMM DDDD LLLL
	 *
	 * T    - toggles for every new command
	 * E    - 0 = use MMM below to determine mode
	 *        1 = Combo PWM mode (timeout)
	 * CC   - Channel 0 to 3
	 * a    - 0 = RC receiver address space
	 *        1 = extra address space
	 * MMM  - 000 = Not used in PF RC Receiver
	 *        001 = Combo direct(timeout)
	 *        010 = Single pin continuous(no timeout)
	 *        011 = Single pin timeout
	 *        1xx = Single output
	 * DDDD - Data, meaning depends on mode
	 * LLLL - Checksum, 0xf ^ nibble1 ^ nibble2 ^ nibble3
	 **************************************************************************/

	 // Checksum
	uint16_t checksum = ~((message >> 12) ^
		(message >> 8) ^ (message >> 4)) & 0x0f;
	bool correct = (message & 0x0f) == checksum;
	// Fail if the checksum is wrong
	if (!correct) {
		return;
	}

	// Toggle bit
	if (verifyToggleBit) {
		bool toggleBit = (message >> 15) & 1;
		// Ignore if the toggle bit is the same as previously
		if (toggleBit == lastToggleBit) {
			return;
		}
		lastToggleBit = toggleBit;
	}

	// Channel
	uint8_t channelAddress = (message >> 11) & 0b111;
	// Ignore if the message is for a different channel
	if (channelAddress != myChannelAddress) {
		return;
	}

	// Mode and data
	uint8_t mode = (message >> 8) & 0b111,
		data = (message >> 4) & 0b1111;
	//Serial.print("mode\t");
	//Serial.println(mode, BIN);
	//Serial.print("data\t");
	//Serial.println(data, BIN);
	switch (mode) {
	case 0b000:	// Not used in PF RC receiver
		break;
	case 0b001:	// Combo direct (timeout)
		comboDirect(data);
		break;
	case 0b010:	// Single pin continuous
		singlePinContinuous(data);
		break;
	case 0b011:	// Single pin timeout
		singlePinTimeout(data);
		break;
	default: // Single output
		singleOutput(data);
		break;
	}

	Serial.print("correct\t");
	Serial.println(correct ? 1 : 0);

	Serial.print("message\t");
	for (uint8_t i = 0; i < MESSAGE_LENGTH; i++) {
		Serial.print((message >> i) & 1);
		if ((i % 4) == 3) Serial.print(' ');
	}
	Serial.println();
}

/*******************************************************************************
* Power Functions modes
*******************************************************************************/

void comboDirect(uint8_t data) {
	digitalWrite(LED_BUILTIN, HIGH);

	uint8_t motorA = data & 0b11;
	uint8_t motorB = (data >> 2) & 0b11;

	uint16_t position;
	switch (motorA) {
	default:
		position = 90;
		break;
	case COMBO_DIRECT_FWD:
		position = 135;
		break;
	case COMBO_DIRECT_REV:
		position = 45;
		break;
	}
	myServo.write(position);

	startTimeout();

	digitalWrite(LED_BUILTIN, LOW);
}

void singlePinContinuous(uint8_t data) {

}

void singlePinTimeout(uint8_t data) {

}

void singleOutput(uint8_t data) {

}

/*******************************************************************************
* Timeout
*******************************************************************************/

void checkTimeout() {
	if (!doTimeout) return;
	if ((millis() - lastCommandMillis) >= TIMEOUT_MILLIS) {
		floatMotors();
	}
	doTimeout = false;
}

void startTimeout() {
	lastCommandMillis = millis();
	doTimeout = true;
}

void clearTimeout() {
	doTimeout = false;
}

/*******************************************************************************
* Low-Level motor commands
*******************************************************************************/

void floatMotors() {
	// DEBUG
	myServo.write(90);
}

/*******************************************************************************
* Interrupts
*******************************************************************************/

ISR(PCINT0_vect) {
	// Ignore rising edges (end of IR mark)
	if (digitalRead(IR_RX_PIN)) {
		return;
	}

	uint32_t microsNow = micros();
	pulseLength = microsNow - lastMicros;

	if (pulseLength < 316) {
		// Invalid, ignore
		return;
	} else if (pulseLength < 526) {
		// Low bit
		volMessageLength++;
	} else if (pulseLength < 947) {
		// High bit
		volMessage |= 1 << (MESSAGE_LENGTH - ++volMessageLength);
	} else {
		// Start bit
		// If pulseLength < 1579, this might be a stop bit for the first of two back-to-back messages.
		volMessage = 0;
		volMessageLength = 0;
	}

	lastMicros = microsNow;
	messageReady = volMessageLength == MESSAGE_LENGTH;
}