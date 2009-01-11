/*
 * file.cpp
 * Inkpad file handling.
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
#include "file.h"


//////////////
// ROUTINES //
//////////////

//
// File handling
//

// Open a file
void file_open(std::ifstream& inputStream, const std::string& inputFile)
{
	// Open the stream
	inputStream.open(inputFile.c_str());

	// Check stream validity
	if (!inputStream.is_open())
	{
		throw std::string("error while opening input stream");
	}
}
void file_open(std::ofstream& inputStream, const std::string& inputFile)
{
	// Open the stream
	inputStream.open(inputFile.c_str());

	// Check stream validity
	if (!inputStream.is_open())
	{
		throw std::string("error while opening output stream");
	}
}

// Close a file
void file_close(std::ifstream& inputStream)
{
	inputStream.close();
}
void file_close(std::ofstream& inputStream)
{
	inputStream.close();
}

// Identify a file
bool file_identify(const std::string& inputFile, std::string& type)
{
	// Extension check
	unsigned int position = inputFile.find_last_of(".");
	if (position < inputFile.length())
	{
		std::string extension = inputFile.substr(position+1);
		type = extension;
		return true;
	}

	return false;
}
bool file_identify(std::ifstream& inputStream, std::string& type)
{
	return false;
}
