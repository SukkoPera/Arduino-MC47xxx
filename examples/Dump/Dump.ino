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
 * This example tests dumps the whole contents of your EERAM.
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

	Serial.println ("--- RAM CONTENTS ---");
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

	Serial.println ("\nALL GOOD!");
}

void loop () {
}
