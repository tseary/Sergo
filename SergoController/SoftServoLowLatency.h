// This is an ultra simple software servo driver. For best
// results, use with a timer0 interrupt to refresh() all
// your servos once every 20 milliseconds!
// Written by Limor Fried for Adafruit Industries, BSD license

// Modified by Thomas Seary, Sept 2019

#pragma once

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class SoftServoLowLatency {
public:
	// Lower maxLatency allows more time-sensitive tasks outside of this class.
	// Higher maxLatency may improve servo accuracy. Default is 250 us.
	SoftServoLowLatency(uint16_t maxLatency = 250);
	void attach(uint8_t pin);
	void detach();
	boolean attached();
	void write(uint8_t degrees);

	// Returns true if a pulse is active.
	// If it returns true, refresh() should be called continuously.
	// If it returns false, the next call should be made within 20 milliseconds.
	bool refresh(void);
private:
	bool isAttached = false;
	uint8_t servoPin = 255;	// The digital pin number
	uint8_t angle = 90;		// Servo angle in degrees
	uint16_t pulseMicros;	// The length of the pulse in microseconds

	bool pulseOn = false;
	uint32_t pulseStartMillis = 0;	// The time in millis to start the next pulse
	uint32_t pulseEndMicros = 0;	// The time in micros to end the current pulse
	uint16_t _maxLatency = 200;
	static const uint16_t PULSE_PERIOD_MILLIS = 20;
};
