
/*
 * ATTINY84 program to receive IR signals and control servos.
 */

#include <avr/sleep.h>

 // Servo signal output pins
const byte SERVO_A_PIN = 3;
const byte SERVO_B_PIN = 4;

// IR receiver pin
const byte IR_RX_PIN = 2;

void setup() {

	// Set the audio output pins
	pinMode(SERVO_A_PIN, OUTPUT);
	pinMode(SERVO_B_PIN, OUTPUT);

	// DEBUG blink at 1 Hz
	/*while (true) {
		digitalWrite(SERVO_B_PIN, HIGH);
		delay(500);
		digitalWrite(SERVO_B_PIN, LOW);
		delay(500);
	}*/

	pinMode(IR_RX_PIN, INPUT_PULLUP);
	//initializeCommand();

	// Set up low power sleep that can be interrupted by pin change
	// Note that sleep does not start immediately (but maybe it should?)
	/*ADCSRA &= ~(1 << ADEN);	// Turn off ADC
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);*/
}

void loop() {
	// Enter sleep mode (to be woken by interrupt)
	/*sleep_enable();
	sleep_cpu();
	sleep_disable();*/

	bool rxState = false,
		lastRxState = false;
	uint32_t lastMicros = micros();

	uint32_t message = 0;

	while (true) {
		lastRxState = rxState;
		rxState = !digitalRead(IR_RX_PIN);

		if (rxState && !lastRxState) {
			uint32_t del = micros() - lastMicros;
			digitalWrite(SERVO_B_PIN, del > 526);

			if (del < 1579) {
				if (del < 947) {
					if (del > 316) {
						// data bit
						message <<= 1;
						if (del > 526) {
							message |= 1;
						}
					} else {
						// noise/bounce
						continue;
					}
				} else {
					// start/stop bit
					if (message) {
						break;
					}
				}
			} else {
				// beginning of start bit
				message = 0;
			}

			lastMicros = micros();
		}

		delayMicroseconds(1);
	}

	// Flash message
	delay(5000);
	digitalWrite(SERVO_B_PIN, HIGH);
	delay(100);
	digitalWrite(SERVO_B_PIN, LOW);
	delay(100);
	for (uint8_t i = 0; i < 16; i++) {
		digitalWrite(SERVO_B_PIN, (message >> i) & 1);
		delay(500);
	}
	digitalWrite(SERVO_B_PIN, LOW);
	delay(100);
	digitalWrite(SERVO_B_PIN, HIGH);
	delay(100);
	digitalWrite(SERVO_B_PIN, LOW);


	//checkCommand();

	// DEBUG Control servo A
	/*const uint16_t servoMicros = 1100 + ((millis() / 5) % 800);
	digitalWrite(SERVO_A_PIN, HIGH);
	delayMicroseconds(servoMicros);
	digitalWrite(SERVO_A_PIN, LOW);
	delayMicroseconds(20000 - servoMicros);*/
}
