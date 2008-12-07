/*
 * input.cpp
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
 * Comments:
 *  - TOP specification reversly engineered, with help of the existing codebase from
 *    toptools
 *
 */

///////////////////
// CONFIGURATION //
///////////////////

//
// Essential stuff
//

// Headers
#include "input.h"


////////////////////
// CLASS ROUTINES //
////////////////////

//
// Construction and destruction
//

Input::Input()
{
}


//
// Class member routines
//

// Set the data pointer
void Input::setData(Data* inputDataPointer)
{
	data = inputDataPointer;
}

// Read from the file
void Input::read(const std::string &inputFile)
{
	// Guess the data type from the extension
	std::string type;
	if (!file_identify(inputFile, type))
	{
		throw std::string("cannot extract input file type");
		return;
	}

	// Decapitalize given type
	for (unsigned int i = 0; i < type.size(); i++)
		type[i] = tolower(type[i]);

	// Process all cases
	if (type == "top")
	{
		std::ifstream stream;
		file_open(stream, inputFile);
		data_input_top(stream);
		file_close(stream);
	}
	else
	{
		throw std::string("unsupported input file type");
		return;
	}
}



//
// Data processing
//

// TODO: implement more formats:
// MyScript Notes file format (.notes)
// Logitech PEN file format (.pen)
// Anoto Pen Generated Coordinates file format (.pgc)
// Nokia Pen Generated Document file format (.pgd)
// IBM Crosspad file format (.ps)
// Anoto file format (.svg)
// iRex iLiad file format (.irx)
// ACECAD DigiMemo file format (.dhw) - http://www.kevlindev.com/blog/?p=58
// Pegasus NoteTaker file format (.pnt)

// Waltop file format (.top)
void Input::data_input_top(std::ifstream& stream)
{
	// General buffer variable
	char* buffer;

	// Read fileheader
	buffer = new char [6];
	stream.read(buffer, 6);
	if (strncmp(buffer, "WALTOP", 6) != 0)
	{
		throw std::string("header of top file seems damaged");
		return;
	}
	delete[] buffer;

	// Read first 26 bytes (unknown content)
	buffer = new char [26];
	stream.read(buffer, 26);
	delete[] buffer;

	// Check if file isn't empty
	if (stream.eof())
		return;

	// Configure the pen
	data->penWidth = 10;
	data->penForeground = BLACK;
	data->penBackground = WHITE;

	// Configure image defaults
	data->imgSizeX = 8800;
	data->imgSizeY = 12000;
	data->imgBackground = WHITE;

	// Initialise and read start coördinates
	buffer = new char [6];
	stream.read(buffer, 6);
	int x1 = bitwise_tow(buffer[4], buffer[3]);
	int y1 = 12000 - bitwise_tow(buffer[2], buffer[1]);

	// Read untill at end of file
	bool end_of_stroke = false;
	while (!stream.eof())
	{
		// Initialise and read end coördinates
		stream.read(buffer, 6);
		int x2 = bitwise_tow(buffer[4], buffer[3]);
		int y2 = 12000 - bitwise_tow(buffer[2], buffer[1]);

		// Create a new line (if we haven't started a new stroke
		if (!end_of_stroke)
		{
			std::vector<double> points;
			points.resize(4);
			points[0] = x1;
			points[1] = y1;
			points[2] = x2;
			points[3] = y2;
			data->addPolyline(points);
		} else {
			end_of_stroke = false;
		}

		// Switch the coördinates
		x1 = x2;
		y1 = y2;

		// Check if we should later on draw a line from these endpoints
		if (buffer[0] == 0)
		{
			end_of_stroke = true;
		}
	}

	// Clear the buffer
	delete[] buffer;
}
