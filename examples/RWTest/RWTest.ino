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
 * This example does a full Read-Write test of your EERAM.
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

	// Choose a random seed for the random number generator ;)
	randomSeed (analogRead (A3));
	byte seed = random (1023);

	Serial.println ("--- INITIAL RAM CONTENTS ---");
	for (word i = 0; i < RAM_SIZE; i++) {
		byte v;

		if (!ram.read (i, v)) {
			Serial.print ("\nReading from location ");
			Serial.print (i);
			Serial.println (" failed");
			halt ();
		} else {
			Serial.print (v);
			Serial.print ("\t");
		}

		// New line every ROW_SIZE columns
		if ((i + 1) % ROW_SIZE == 0)
			Serial.println ();
	}

	Serial.println ("\n--- WRITING ---");
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
