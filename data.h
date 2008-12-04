/*
 * data.h
 * Inkpad data handling.
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

// Include guard
#ifndef __DATA
#define __DATA


// Headers
#include <iostream>
#include <vector>


////////////////
// DATA TYPES //
////////////////

// The structure
struct Element
{
	// Data
	int identifier;
	std::vector<double> parameters;
	std::string foreground;
	std::string background;
	int width;
};

/*
 * Possible elements
 *
 * - ID 1: a point
 *   params: x value, y value
 *
 * - ID 2: a line
 *   params: x start value, y start value, x end value, y end value
 *
 */


//////////////////////
// CLASS DEFINITION //
//////////////////////

class Data
{
	public:
		// Construction and destruction
		Data();
		void clear();

		// Element configuration
		void setWidth(int);
		void setColourBg(const std::string&);
		void setColourFg(const std::string&);

		// Element input
		void addPoint(int, int);
		void addLine(int, int, int, int);

		// Element output
		std::string getColourBg() const;
		void getSize(int &w, int &h) const;

		// Iterators
		typedef std::vector<Element>::const_iterator const_iterator;
		const_iterator begin() const { return elements.begin(); }
		const_iterator end() const { return elements.end(); }

	private:
		// Element configuration data
		std::string colour_bg;	// TODO: distinguish image background from element background, the same atm
		std::string colour_fg;
		int width;

		// Element containers
		void addElement(Element&);
		std::vector<Element> elements;
};



// Include guard
#endif
