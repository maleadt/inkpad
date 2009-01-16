/*
 * input.cpp
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

#define bitwise_tow(h,l) ((((unsigned char)h)<<8)|((unsigned char)l))


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
	else if (type == "dhw")
	{
		std::ifstream stream;
		file_open(stream, inputFile);
		data_input_dhw(stream);
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
		stream.read(buffer, 6); //TODO fails at end of file, might need to read first bits to indicate if EOF? Check file!
		buffer[6] = 0;	// Fix nullpointer at end of string
		int x2 = bitwise_tow(buffer[4], buffer[3]);
		int y2 = 12000 - bitwise_tow(buffer[2], buffer[1]);

		// Create a new line (if we haven't started a new stroke
		if (!end_of_stroke)
		{
			vector<double> points;
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

// ACECAD DigiMemo file format (.dhw)
void Input::data_input_dhw(std::ifstream& stream)
{
	// General buffer variable
	char* buffer;

	// Read fileheader
	buffer = new char [32];
	stream.read(buffer, 32);
	if (strncmp(buffer, "ACECAD_DIGIMEMO_HANDWRITING_____", 32) != 0)
	{
		throw std::string("header of dhw file seems damaged");
		return;
	}
	delete[] buffer;

	// Read version
	char version = 0x01;
	buffer = new char [1];
	stream.read(buffer, 1);
	if (strncmp(buffer, &version, 1) != 0)
	{
		throw std::string("unsupported dhw version");
		return;
	}
	delete[] buffer;

	// Read the image size
	buffer = new char [1];
	stream.read(buffer, 1);
	std::cout << "Read value: " << buffer << std::endl;
	/*
	std::cout << "Int value: " << atoi(buffer) << std::endl;
	printchar(buffer);
	data->imgSizeX = xstrtoi(buffer);
	buffer[1] = 0;
	stream.read(buffer, 1);
	printchar(buffer);
	data->imgSizeX += 16*16*xstrtoi(buffer);
	buffer[1] = 0;
	stream.read(buffer, 1);
	printchar(buffer);
	data->imgSizeY = xstrtoi(buffer);
	buffer[1] = 0;
	stream.read(buffer, 1);
	printchar(buffer);
	data->imgSizeY += 16*16*xstrtoi(buffer);
	delete[] buffer;
	std::cout << "Got size: " << data->imgSizeX << " x " << data->imgSizeY << std::endl;
	*/

	// Clear the buffer
	//delete[] buffer;
}
