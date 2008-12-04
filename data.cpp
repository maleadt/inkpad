/*
 * data.cpp
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

// Headers
#include "data.h"


////////////////////
// CLASS ROUTINES //
////////////////////

//
// Construction and destruction
//

Data::Data()
{
	clear();
}

void Data::clear()
{
	// Default values
	setColourBg(WHITE);
	setColourFg(BLACK);
	setWidth(10);

	// Delete elements
	elements.clear();
}


//
// Element appearance
//

// Set the line width
void Data::setWidth(int inputWidth)
{
	width = inputWidth;
}

// Set the background colour
void Data::setColourBg(const Colour& inputColour)
{
	colour_bg = inputColour;
}

// Set the foreground colour
void Data::setColourFg(const Colour& inputColour)
{
	colour_fg = inputColour;
}


//
// Element input
//

// Add a single point
void Data::addPoint(int x1, int y1)
{
	Element tempElement;
	tempElement.identifier = 1;

	tempElement.parameters.resize(2);
	tempElement.parameters[0] = x1;
	tempElement.parameters[1] = y1;

	addElement(tempElement);
}

// Add a new line
void Data::addLine(int x1, int y1, int x2, int y2)
{
	Element tempElement;
	tempElement.identifier = 2;

	tempElement.parameters.resize(4);
	tempElement.parameters[0] = x1;
	tempElement.parameters[1] = y1;
	tempElement.parameters[2] = x2;
	tempElement.parameters[3] = y2;

	addElement(tempElement);
}

// Add a new element (private, applies current settings)
void Data::addElement(Element& inputElement)
{
	// Apply current settings
	inputElement.foreground = colour_fg;
	inputElement.background = colour_bg;
	inputElement.width = width;

	// Save the element
	elements.push_back(inputElement);
}


//
// Element output
//

// Give the background colour
Colour Data::getColourBg() const
{
	return colour_bg;
}

void check_range(int& low, int& high, const int& value)
{
	if (value < low || low == -1)
	{
		low = value;
	}
	else if (value > high)	// WARNING: this could trigger a corned case where values only go down
	{
		high = value;
	}
}

// Get the maximum size
void Data::getSize(int& x0, int& y0, int &x1, int& y1) const
{
	// Starting value
	x0 = -1;
	y0 = -1;
	x1 = 0;
	y1 = 0;

	// Loop elements
	std::vector<Element>::const_iterator it = elements.begin();
	int count = 0;
	while (it != elements.end())
	{
		count++;
		switch (it->identifier)
		{
			// Point
			case 1:
				check_range(x0, x1, it->parameters[0]);
				check_range(y0, y1, it->parameters[1]);
				break;

			// Line
			case 2:
				check_range(x0, x1, it->parameters[0]);
				check_range(y0, y1, it->parameters[1]);
				check_range(x0, x1, it->parameters[2]);
				check_range(y0, y1, it->parameters[3]);
				break;
			default:
				throw std::string("unsupported element during size check");
		}
		++it;
	}
}
