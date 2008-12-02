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

Input::Input(const std::string& inputFile)
{
	file = inputFile;
}


//
// Class member routines
//

// Read from the file
void Input::read()
{
	// Open the file
	file_open();

	// Read the file
	data_input();

	// Close the file
	file_close();
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
		throw std::string("unknown input file type");
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
	while (!stream.eof())
	{
		// Initialise and read end coördinates
		stream.read(buffer, 6);
		int x2 = bitwise_tow(buffer[4], buffer[3]);
		int y2 = 12000 - bitwise_tow(buffer[2], buffer[1]);


		if (buffer[0] == 0)
		{
			// End of a stroke, no need to save it
			// TODO: can we get a point here? (twice a "end of stroke")?
			//       if so, detect it!
		}
		else
		{
			// Create a new line
			Line tempLine;
			tempLine.start_x = x1;
			tempLine.start_y = y1;
			tempLine.end_x = x2;
			tempLine.end_y = x2;

			// Save the line
			data.addLine(tempLine);
		}


		// Switch the coördinates
		x1 = x2;
		y1 = y2;
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
