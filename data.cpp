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
	setColourBg("white");
	setColourFg("black");
	setWidth(10);

	// Delete elements
	elements.clear();
}


//
// Element configuration
//

// Set the line width
void Data::setWidth(int inputWidth)
{
	width = inputWidth;
}

// Set the background colour
void Data::setColourBg(const std::string& inputColour)
{
	colour_bg = inputColour;
}

// Set the foreground colour
void Data::setColourFg(const std::string& inputColour)
{
	colour_fg = inputColour;
}


//
// Elemens input
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
std::string Data::getColourBg() const
{
	return colour_bg;
}

// Get the maximum size
void Data::getSize(int &w, int &h)
{
	w = 8800;
	h = 12000;
}
