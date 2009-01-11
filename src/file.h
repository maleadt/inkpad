/*
 * file.h
 * Inkpad file handling.
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
#ifndef __IFILE
#define __IFILE


// Headers
#include <iostream>
#include <fstream>

//
// Constants
//


/////////////////
// DEFINITIONS //
/////////////////

// Open a file
void file_open(std::ifstream& inputStream, const std::string& inputFile);
void file_open(std::ofstream& inputStream, const std::string& inputFile);

// Close a file
void file_close(std::ifstream& inputStream);
void file_close(std::ofstream& inputStream);

// Identify a file
bool file_identify(const std::string& inputFile, std::string& outputType);
bool file_identify(std::ifstream& inputStream, std::string& outputType);


// Include guard
#endif
