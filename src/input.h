/*
 * input.h
 * Inkpad input handling.
 *
 * Copyright (c) 2009 Tim Besard <tim.besard@gmail.com>
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
#include "file.h"

// Containers
#ifdef USE_USTD
 #include "ustd.h"
 using ustd::vector;
#else
 #include <vector>
 using std::vector;
#endif

//////////////////////
// CLASS DEFINITION //
//////////////////////

class Input
{
	public:
		// Construction and destruction
		Input();

		// Class member routines
		void read(const std::string &inputFile);
		void setData(Data*);

	private:
		// Data processing
		void data_input_top(std::ifstream&);
		void data_input_dhw(std::ifstream&);

		// Data
		Data* data;
};




// Include guard
#endif
