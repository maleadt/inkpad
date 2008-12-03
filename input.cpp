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
	// Reset everything
	clear();
}


//
// Class member routines
//

// Read from the file
void Input::read(const std::string &inputFile)
{
	// Save the requested filename
	file = inputFile;

	// Open the file
	file_open();

	// Read the file
	data_input();

	// Close the file
	file_close();
}

// Reset the object's data
void Input::clear()
{
	type.clear();
	file.clear();
	data.clear();
}

// Export all read data
Data* Input::getdata()
{
	// Return a pointer to the processed data
	return &data;
}


//
// File handling
//

// Open a file
void Input::file_open()
{
	// Check if stream hasn't been used before
	if (stream.is_open())
	{
		throw std::string("cannot re-use input stream");
	}

	// Open the stream
	stream.open(file.c_str());

	// Check stream validity
	if (!stream.is_open())
	{
		throw std::string("error while opening input stream");
	}
}

// Close a file
void Input::file_close()
{
	stream.close();
}


//
// Data processing
//

// Input data (wrapper around the different types we support)
void Input::data_input()
{
	// Guess the data type from the extension
	if (!data_type())
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
		data_input_top();
	}
	else
	{
		throw std::string("unsupported input file type");
		return;
	}
}

// Input data in TOP format
void Input::data_input_top()
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
			data.addLine(x1, y1, x2, y2);
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

// Guess the data type
bool Input::data_type()
{
	// Extension check
	unsigned int position = file.find_last_of(".");
	if (position < file.length())
	{
		std::string extension = file.substr(position+1);
		type = extension;
		return true;
	}

	// Data check
	// TODO: Read some bytes to detect data type
	return false;
}
