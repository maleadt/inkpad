/*
 * generic.h
 * Inkpad - generic functions.
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
#ifndef __GENERIC
#define __GENERIC


// System headers
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <string>
#include <iostream>


//////////////
// ROUTINES //
//////////////

// Swap two items through references
template <class X> void
swap(X &a, X &b)
{
	X temp = a;
	a = b;
	b = temp;
}

// Generate a number from lower up to (and with) upper
int random_range(int lowest_number, int highest_number);

// Convert several types to a string
template <typename X>
std::string stringify(X input)
{
	std::string output;
	std::stringstream convert;
	convert << input;
	output = convert.str();
	return output;
}

// Include guard
#endif
