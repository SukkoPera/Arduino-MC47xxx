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
 * This examples clears the Status Register
 */

#include <MC47xxx.h>

MC47xxx ram;

void setup () {
	Serial.begin (9600);

	if (!ram.begin ()) {
		Serial.println ("Cannot initialize RAM");
		halt ();
	}

	byte status;
	if (ram.getStatus (status)) {
		Serial.print ("Initial Status = 0b");
		Serial.println (status, BIN);
	} else {
		Serial.println ("Cannot get status");
	}

	if (!ram.writeRegister (MC47xxx::REG_STATUS, 0x00)) {
		Serial.println ("Cannot write status register");
	}

	if (ram.getStatus (status)) {
		Serial.print ("New Status = 0b");
		Serial.println (status, BIN);
	} else {
		Serial.println ("Cannot get new status");
	}
}

void loop () {
}
