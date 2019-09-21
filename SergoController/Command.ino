
// Master communication fields
volatile bool commandReady = false;

volatile uint32_t lastRxMicros = 0;
volatile uint32_t message;

volatile uint8_t blipsReceived = 0;

void initializeCommand() {
	// Set up master communication
	pinMode(IR_RX_PIN, INPUT_PULLUP);

	// Set up pin change interrupts on RX
	// See https://thewanderingengineer.com/2014/08/11/pin-change-interrupts-on-attiny85/
	GIMSK = bit(PCIE0);	// turns on pin change interrupts
	PCMSK0 = bit(IR_RX_PIN);	// turn on interrupts on RX pin

	sei();  // enable interrupts
}

void checkCommand() {
	// Return if there is no command ready
	if (!commandReady) {
		return;
	}

	noInterrupts();

	// TODO execute command here
	for (uint8_t i = 0; i < blipsReceived; i++) {
		digitalWrite(SERVO_B_PIN, HIGH);
		delay(250);
		digitalWrite(SERVO_B_PIN, LOW);
		delay(250);
	}
	delay(1000);

	/*digitalWrite(SERVO_B_PIN, HIGH);
	delay(100);
	digitalWrite(SERVO_B_PIN, LOW);
	delay(100);
	for (uint8_t i = 0; i < 16; i++) {
		digitalWrite(SERVO_B_PIN, (message >> i) & 1);
		delay(333);
	}
	digitalWrite(SERVO_B_PIN, LOW);
	delay(100);
	digitalWrite(SERVO_B_PIN, HIGH);
	delay(100);
	digitalWrite(SERVO_B_PIN, LOW);*/

	clearCommand();

	interrupts();
}

// Disable interrupts before calling this function.
void clearCommand() {
	commandReady = false;

	blipsReceived = 0;
	message = 0;
}

// RX pin change interrupt - receive commands from remote control
ISR(PCINT0_vect) {
	// Ignore rising edges
	if (digitalRead(IR_RX_PIN)) {
		return;
	}

	blipsReceived++;

	uint32_t microsNow = micros();
	uint32_t delayMicros = microsNow - lastRxMicros;

	// Low bit range		316 - 526 us
	// High bit range		526 –  947 us
	// Start/stop bit range	947 – 1579 us
	if (delayMicros < 316) {
		return;
	} else if (delayMicros < 526) {
		// Append low bit
		message <<= 1;
	} else if (delayMicros < 947) {
		// Append high bit
		message = (message << 1) | 1;
	} else if (delayMicros < 1579 && message) {
		//commandReady = true;

		// DEBUG
		commandReady = (message >> 15) & 1;

	}

	lastRxMicros = microsNow;
}

