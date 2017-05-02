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
 * Arduino library that allows interfacing with Microchip 47xxx serial EERAMs
 */

#include <Wire.h>

class MC47xxx {
private:
	static const byte DEFAULT_I2C_MEMADDR = 0b01010000;		// 80 dec
	static const byte DEFAULT_I2C_REGADDR = 0b00011000;		// 24 dec

	static const byte CMD_STORE = 0b00110011;
	static const byte CMD_RECALL = 0b11011101;

	static const int T_WC = 1;			// ms
	static const int T_STORE = 25;		// ms (can be reduced to 8 for 47x04)
	static const int T_RECALL = 5;		// ms (can be reduced to 2 for 47x04)

	static const int TIMEOUT = 3000;	// ms

	TwoWire* i2cBus = NULL;

	byte busAddr = DEFAULT_I2C_MEMADDR;
	byte regAddr = DEFAULT_I2C_REGADDR;

	enum {
		B_EVENT = 0,
		B_ASE = 1,
		B_BP0 = 2,
		B_BP1 = 3,
		B_BP2 = 4,
		// Unused
		// Unused
		B_AM = 7
	};

public:
	static const byte REG_STATUS = 0x00;
	static const byte REG_COMMAND = 0x55;

	boolean begin (TwoWire& _i2cBus, const byte a1 = 0, const byte a2 = 0) {
		i2cBus = &_i2cBus;
		busAddr = DEFAULT_I2C_MEMADDR | ((!!a1) << 1);
		regAddr = DEFAULT_I2C_REGADDR | ((!!a2) << 2);

		byte dummy;
		return getStatus (dummy);
	}

	boolean begin () {
		Wire.begin ();
		i2cBus = &Wire;

		byte dummy;
		return getStatus (dummy);
	}

	// SRAM RANDOM READ
	boolean read (const word addr, byte& val) {
		byte ret = false;

		// Send address
		i2cBus -> beginTransmission (busAddr);
		i2cBus -> write (highByte (addr));
		i2cBus -> write (lowByte (addr));
		byte res = Wire.endTransmission ();

		if (res == 0) {
			// Read byte
			i2cBus -> requestFrom (busAddr, (byte) 1);

			unsigned long start = millis ();
			while (i2cBus -> available () < 1 && millis () - start < TIMEOUT)
				;
			if (i2cBus -> available () > 0) {
				val = i2cBus -> read ();
				ret = true;
			}
		} else {
			// Couldn't send address
		}

		return ret;
	}

	boolean write (const word addr, const byte val) {
		byte ret = false;

		i2cBus -> beginTransmission (busAddr);
		i2cBus -> write (highByte (addr));
		i2cBus -> write (lowByte (addr));
		i2cBus -> write (val);
		byte res = i2cBus -> endTransmission ();
		//~ Serial.println (res);

		switch (res) {
			case 0:
				// Success!
				ret = true;
				break;
			case 1:
				// Data too long to fit in transmit buffer
				break;
			case 2:
				// Received NACK on transmit of address
				break;
			case 3:
				// Received NACK on transmit of data
				break;
			case 4:
			default:
				// Other error
				break;
		}

		return ret;
	}

	boolean getStatus (byte& val) {
		byte ret = false;

		i2cBus -> requestFrom (regAddr, (byte) 1);
		unsigned long start = millis ();
		while (i2cBus -> available () < 1 && millis () - start < TIMEOUT)
			;
		if (i2cBus -> available () > 0) {
			val = i2cBus -> read ();
			ret = true;
		}

		return ret;
	}

	boolean writeRegister (byte reg, byte val) {
		byte ret = false;

		i2cBus -> beginTransmission (regAddr);
		i2cBus -> write (reg);
		i2cBus -> write (val);
		if (i2cBus -> endTransmission () == 0)
			ret = true;

		// Wait for Twc ms
		delay (1);

		return ret;
	}

	boolean store () {
		boolean ret = writeRegister (REG_COMMAND, CMD_STORE);
		if (ret)
			delay (T_STORE);
		return ret;
	}

	boolean recall () {
		boolean ret = writeRegister (REG_COMMAND, CMD_RECALL);
		if (ret)
			delay (T_RECALL);
		return ret;
	}

	boolean isEventDetected () {
		byte status;
		return getStatus (status) && (status & (1 << B_EVENT));
	}

	boolean isAutoStoreEnabled () {
		byte status;
		return getStatus (status) && (status & (1 << B_ASE));
	}

	boolean isWriteProtected () {
		byte status;
		return getStatus (status) && (status & ((1 << B_BP0) | (1 << B_BP1) | (1 << B_BP2)));
	}

	boolean isModified () {
		byte status;
		return getStatus (status) && (status & (1 << B_AM));
	}

	boolean setAutoStore (boolean enabled) {
		byte ret = false;

		byte status;
		if (getStatus (status)) {
			if (enabled)
				status |= 1 << B_ASE;
			else
				status &= ~(1 << B_ASE);
			ret = writeRegister (REG_STATUS, status);
		}

		return ret;
	}
};
