/*
 * input.h
 * Inkpad input handling.
 *
 * Copyright (c) 2008 Tim Besard <tim.besard@gmail.com>
 * All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

///////////////////
// CONFIGURATION //
///////////////////

//
// Essential stuff
//

// Include guard
#ifndef __INPUT
#define __INPUT


// Headers
#include <iostream>
#include <fstream>
#include <string.h>
#include "data.h"


//
// Inline functions
//

#define bitwise_tow(h,l) ((((unsigned char)h)<<8)|((unsigned char)l))

//////////////////////
// CLASS DEFINITION //
//////////////////////

class Input
{
	public:
		// Construction and destruction
		Input(const std::string &inputFile);

		// Class member routines
		void read();
		Data* getdata();

	private:
		// File handling
		void file_open();
		void file_close();

		// Data processing
		void data_input();
		void data_input_top();
		bool data_type();

		// Data
		std::string type;
		std::string file;
		std::ifstream stream;
		Data data;
};




// Include guard
#endif
