/*******************************************************************************
 * This file is part of Arduino-MC47xxx                                        *
 *                                                                             *
 * Copyright (C) 2017 by SukkoPera                                             *
 *                                                                             *
 * Arduino-MC47xxx is free software: you can redistribute it and/or modify     *
 * it under the terms of the GNU General Public License as published by        *
 * the Free Software Foundation, either version 3 of the License, or           *
 * (at your option) any later version.                                         *
 *                                                                             *
 * Arduino-MC47xxx is distributed in the hope that it will be useful,          *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 * GNU General Public License for more details.                                *
 *                                                                             *
 * You should have received a copy of the GNU General Public License           *
 * along with Arduino-MC47xxx. If not, see <http://www.gnu.org/licenses/>.     *
 *******************************************************************************
 *
 * This example tests that the Auto-Store functionality is working properly.
 *
 * Note that for this to work properly, you must place a 10 uF capacitor on the
 * Vcap pin!
 *
 * WARNING: THIS TEST WILL OVERWRITE THE WHOLE CONTENTS OF YOUR EERAM!
 */

#include <MC47xxx.h>

MC47xxx ram;

// 2048 for 47x16, 512 for 47x04
const int RAM_SIZE = 2048;

// How many bytes to show per line
const byte ROW_SIZE = 16;

void halt () {
	pinMode (LED_BUILTIN, OUTPUT);
	for (;;) {
		digitalWrite (LED_BUILTIN, HIGH);
		delay (300);
		digitalWrite (LED_BUILTIN, LOW);
		delay (300);
	}
}

void setup () {
	Serial.begin (9600);

	if (!ram.begin ()) {
		Serial.println ("Cannot initialize RAM");
		halt ();
	}

	// 1. Make sure autostore is enabled
	if (!ram.isAutoStoreEnabled ()) {
		Serial.print ("Enabling autostore... ");
		if (!ram.setAutoStore (true)) {
			Serial.println ("Failed");
			halt ();
		} else {
			Serial.println ("Done");
		}
	} else {
		Serial.println ("Autostore is already enabled");
	}

	// Choose a random seed for the random number generator ;)
	randomSeed (analogRead (A3));
	byte seed = random (1023);

	// 2. Write random contents
	Serial.println ("--- WRITING ---");
	randomSeed (seed);			// Use random seed
	for (word i = 0; i < RAM_SIZE; i++) {
		byte v = random (256);
		if (!ram.write (i, v)) {
			Serial.print ("\nWriting to location ");
			Serial.print (i);
			Serial.println (" failed");
			halt ();
		} else {
			Serial.print (v);
			Serial.print ("\t");
		}

		if ((i + 1) % ROW_SIZE == 0)
			Serial.println ();
	}

	// 3. Power cycle (At least tell user to do so ;)
	unsigned long startWait = 0;
	do {
		if (startWait == 0 || millis () - startWait > 1000) {
			Serial.println ("\n--- PLEASE POWER-CYCLE YOUR EERAM AND PRESS ENTER ---");
			startWait = millis ();
		}
	} while (Serial.available () == 0);
	while (Serial.available ())
		Serial.read ();

	// 4. Make sure RAM reports as non-modified
	if (ram.isModified ()) {
		Serial.println ("Uhm... you didn't power-cycle your EERAM, did you? Please reset and try again :)");
		halt ();
	}

	// 5. Verify contents
	Serial.println ("\n--- READING BACK ---");
	randomSeed (seed);			// Restore seed
	for (word i = 0; i < RAM_SIZE; i++) {
		byte b = random (256);			// Well, not so random ;)
		byte v;
		if (!ram.read (i, v)) {
			Serial.print ("\nReading from location ");
			Serial.print (i);
			Serial.println (" failed");
			halt ();
		} else {
			Serial.print (v);
			Serial.print ("\t");

			if (v != b) {
				Serial.println ("ERROR");
				halt ();
			}
		}

		if ((i + 1) % ROW_SIZE == 0)
			Serial.println ();
	}

	Serial.println ("\nALL GOOD!");
}

void loop () {
}
