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

Output::Output(const Data& inputData, const std::string& inputFile, const std::string& inputType)
{
	// Don't copy the object, it's the user's responsibility the pointer will still be alive later on
	data = &inputData;

	// Copying two string won't incude heavy memory usage
	file = inputFile;
	type = inputType;
}


//
// Class member routines
//

// Write the data to a given file (in a given format)
void Output::write()
{
	// Check and open the file
	file_open();

	// Convert data to given type
	data_output();

	// Close the file
	file_close();
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
}
