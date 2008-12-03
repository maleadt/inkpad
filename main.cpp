/*
 * main.cpp
 * Inkpad main executable.
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

//
// Configuration
//

// Application headers
#include "input.h"
#include "output.h"
#include "data.h"

// Default headers
#include <iostream>



//
// Main application
//

int main()
{
	std::cout << "* Application initializing" << std::endl;

	// Scan given folder for specific folder structure
	try
	{
		// Initialise objects
		std::cout << "* Initialising" << std::endl;
		Input engineInput;
		Output engineOutput;

		// Read the file
		std::cout << "* Reading data" << std::endl;
		engineInput.read("testfile.top");
		Data* engineData = engineInput.getdata();

		// Output the file
		std::cout << "* Writing data" << std::endl;
		engineOutput.write(engineData, "testfile.svg", "svg");
	}
	catch (std::string error)
	{
		std::cout << "Caught error: " << error << std::endl;
	}

	// For all files:
		// Input all data
		// Process all data
		// Output all data

	return 0;
}
