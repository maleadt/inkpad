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


// System headers
#include <iostream>
#include <fstream>
#include <string.h>

// Application headers
#include "exception.h"
#include "generic.h"
#include "data.h"
#include "file.h"

// Containers
#include <vector>
using std::vector;


//
// Constants
//

// Data generation
const int GENERATE_HEIGHT = 10000;
const int GENERATE_WIDTH = 10000;

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

		// Data generation routines
		void generate_static(int);

	private:
		// Data processing
		void data_input_top(std::ifstream&);
		void data_input_dhw(std::ifstream&);

		// Data
		Data* data;
};




// Include guard
#endif
