/*
 * exception.h
 * Inkpad exception datastructure.
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
#ifndef __EXCEPTION
#define __EXCEPTION


// System headers
#include <exception>
#include <string>

//////////////////////
// CLASS DEFINITION //
//////////////////////

class Exception : public std::exception
{
	public:
		// Construction and destruction
		Exception(std::string inputError) : dataLibrary("unknown"), dataLocation("unknown"), dataError(inputError)
			{ }
		Exception(std::string inputLibrary, std::string inputLocation, std::string inputError) : dataLibrary(inputLibrary), dataLocation(inputLocation), dataError(inputError)
			{ }
        ~Exception() throw()
            { }

		// Exception handling
		const char* who() const throw()     // The library causing an exception
			{ return dataLibrary.c_str(); }
		const char* where() const throw()   // The function in the library
			{ return dataLocation.c_str(); }
		const char* what() const throw()    // The actual error
			{ return dataError.c_str(); }

	private:
        std::string dataLibrary;
		std::string dataLocation;
		std::string dataError;
};

// Include guard
#endif
