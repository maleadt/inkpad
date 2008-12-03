/*
 * output.cpp
 * Inkpad output handling.
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
#include "output.h"


////////////////////
// CLASS ROUTINES //
////////////////////

//
// Construction and destruction
//

Output::Output()
{
}


//
// Class member routines
//

// Write the data to a given file (in a given format)
void Output::write(const Data* inputDataPointer, const std::string& inputFile, const std::string& inputType)
{
	// Save the given data
	data = inputDataPointer;
	file = inputFile;
	type = inputType;

	// Check and open the file
	file_open();

	// Convert data to given type
	data_output();

	// Close the file
	file_close();
}

// Write the data to a given file (but detect the format)
void Output::write(const Data* inputDataPointer, const std::string& inputFile)
{
	// Save the filename
	file = inputFile;

	// Detect the type
	if (!data_type())
	{
		throw std::string("unsupported output file type");
		return;
	}

	// Write the vile
	write(inputDataPointer, inputFile, type);
}


//
// File handling
//

// Open a file
void Output::file_open()
{
	// Check if stream hasn't been used before
	if (stream.is_open())
	{
		throw std::string("cannot re-use output stream");
	}

	// Open the stream
	stream.open(file.c_str());

	// Check stream validity
	if (!stream.is_open())
	{
		throw std::string("error while opening output stream");
	}
}

// Close a file
void Output::file_close()
{
	stream.close();
}


//
// Data processing
//

// Guess the data type
bool Output::data_type()
{
	// Extension check
	unsigned int position = file.find_last_of(".");
	if (position < file.length())
	{
		std::string extension = file.substr(position+1);
		type = extension;
		return true;
	}
	return false;
}

// Output data (wrapper around different data types)
void Output::data_output()
{
	// Decapitalize given type
	for (unsigned int i = 0; i < type.size(); i++)
		type[i] = tolower(type[i]);

	// Process all cases
	if (type == "svg")
	{
		data_output_svg();
	}

	// We got an undetected case
	else
	{
		throw std::string("unknown output file type");
		return;
	}
}

// Output data in SVG format
void Output::data_output_svg()
{
	// Get the maximal size
	int x_max = 8800;
	int y_max = 12000;

	// Print the SVG header
	stream	<< "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
			<< "<svg xmlns=\"http://www.w3.org/2000/svg\"\n"
			<< "	xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n"
			<< "	xmlns:ev=\"http://www.w3.org/2001/xml-events\"\n"
			<< "	version=\"1.1\" baseProfile=\"full\"\n"
			<< "	width=\"" << x_max << "\" height=\"" << y_max << "\" viewBox=\"0 0 " << x_max << " " << y_max << "\">\n";

	// Add a background rectangle
	stream << "<rect x=\"0\" y=\"0\" width=\"" << x_max << "\" height=\"" << y_max << "\" fill=\"" << data->getColourBg() << "\" stroke=\"" << data->getColourBg() << "\" stroke-width=\"1px\" />\n";

	// Process all elements
	std::vector<Element>::const_iterator tempIterator = data->begin();
	while (tempIterator != data->end())
	{
		switch (tempIterator->identifier)
		{
			// A point
			case 1:
				break;

			// A line
			case 2:
				stream << "<line x1=\"" << tempIterator->parameters[0] << "\" y1=\"" << tempIterator->parameters[1] << "\" x2=\""
					 << tempIterator->parameters[2] << "\" y2=\"" << tempIterator->parameters[3] << "\" fill=\"none\" stroke=\"" << tempIterator->foreground << "\" stroke-width=\"" << tempIterator->width << "px\"/>\n";
				break;

			// Unsupported type
			default:
				throw std::string("unsupported element during svg output");
		}
		++tempIterator;
	}

	// Print the SVG footer
	stream << "</svg>\n";
}
