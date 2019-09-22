// This is an ultra simple software servo driver. For best
// results, use with a timer0 interrupt to refresh() all
// your servos once every 20 milliseconds!
// Written by Limor Fried for Adafruit Industries, BSD license

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "SoftServoLowLatency.h"

SoftServoLowLatency::SoftServoLowLatency(uint16_t maxLatency) {
	_maxLatency = maxLatency;
}

void SoftServoLowLatency::attach(uint8_t pin) {
	servoPin = pin;
	angle = 90;
	isAttached = true;
	pinMode(servoPin, OUTPUT);
}

void SoftServoLowLatency::detach(void) {
	isAttached = false;
	pinMode(servoPin, INPUT);
}

boolean  SoftServoLowLatency::attached(void) {
	return isAttached;
}

void SoftServoLowLatency::write(uint8_t degrees) {
	angle = degrees;
	if (!isAttached) return;
	pulseMicros = map(degrees, 0, 180, 1000, 2000);
}

bool SoftServoLowLatency::refresh(void) {

	// TODO Maybe pause interrupts during this entire function

	if (pulseOn) {
		// We are partway through a pulse
		// Check the remainging time
		int remaining = pulseEndMicros - micros();
		if (remaining <= 0) {
			// Pulse end is overdue, so end it immediately
			digitalWrite(servoPin, pulseOn = false);
		} else if (remaining <= _maxLatency) {
			// Pulse end is soon, so pause interrupts and finish it now
			cli();
			delayMicroseconds(remaining);
			digitalWrite(servoPin, pulseOn = false);
			sei();
		}
	} else {
		// A pulse hasn't been started
		if (millis() >= pulseStartMillis) {
			// Time to start
			digitalWrite(servoPin, pulseOn = true);
			pulseEndMicros = micros() + pulseMicros;
			pulseStartMillis += PULSE_PERIOD_MILLIS;
		}
	}

	return pulseOn;
}
