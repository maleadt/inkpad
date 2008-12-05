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
	// Default pen values
	penBackground = WHITE;
	penForeground = BLACK;
	penWidth = 10;

	// Default image values
	imgBackground = WHITE;

	// Delete elements
	elements.clear();
}


//
// Element input
//

// Add a single point
void Data::addPoint(int x1, int y1)
{
	// New element
	Element tempElement;
	tempElement.identifier = 1;

	// Save parameters
	tempElement.parameters.resize(2);
	tempElement.parameters[0] = x1;
	tempElement.parameters[1] = y1;

	// Save the element
	addElement(tempElement);
}

// Add a new line
void Data::addLine(int x1, int y1, int x2, int y2)
{
	// New element
	Element tempElement;
	tempElement.identifier = 2;

	// Save parameters
	tempElement.parameters.resize(4);
	tempElement.parameters[0] = x1;
	tempElement.parameters[1] = y1;
	tempElement.parameters[2] = x2;
	tempElement.parameters[3] = y2;

	// Save the element
	addElement(tempElement);
}

// Add a new element (private, applies current settings)
void Data::addElement(Element& inputElement)
{
	// Save pen condition
	inputElement.width = penWidth;
	inputElement.foreground = penForeground;
	inputElement.background = penBackground;

	// Save the element
	elements.push_back(inputElement);
}


//
// Element conversion
//

// Rotate the image
inline void help_rotate(double& x, double& y, double angle_rad)
{
	double xc = x;
	x = x * cos(angle_rad) - y * sin(angle_rad);
	y = xc * sin(angle_rad) + y* cos(angle_rad);
}
void Data::rotate(double angle)
{
	// Convert the given angle
	double angle_rad = angle / 180 * PI;

	// Rotate all elements
	std::vector<Element>::iterator it = elements.begin();
	while (it != elements.end())
	{
		switch (it->identifier)
		{
			// Point
			case 1:
				help_rotate(it->parameters[0], it->parameters[1], angle_rad);
				break;

			// Line
			case 2:
				help_rotate(it->parameters[0], it->parameters[1], angle_rad);
				help_rotate(it->parameters[2], it->parameters[3], angle_rad);
				break;
			default:
				throw std::string("unsupported element during canvas rotation");
		}
		++it;
	}

	// Make most of the image visible again
	int x0, y0, x1, y1;
	getSize(x0, y0, x1, y1);
	x0 = (x0 < 0 ? x0 : 0);
	y0 = (y0 < 0 ? y0 : 0);
	translate(-x0, -y0);
}

// Relocate the canvas
// TODO: calculate new image size
void Data::translate(int dx, int dy)
{
	// Loop elements
	std::vector<Element>::iterator it = elements.begin();
	while (it != elements.end())
	{
		switch (it->identifier)
		{
			// Point
			case 1:
				it->parameters[0] += dx;
				it->parameters[1] += dy;
				break;

			// Line
			case 2:
				it->parameters[0] += dx;
				it->parameters[1] += dy;
				it->parameters[2] += dx;
				it->parameters[3] += dy;
				break;
			default:
				throw std::string("unsupported element during canvas relocation");
		}
		++it;
	}
}

// Crop the image automatically
void Data::autocrop()
{
	// Look up the size of our image
	int x0, y0, x1, y1;
	getSize(x0, y0, x1, y1);

	// Relocate the canvas
	translate(-x0, -y0);

	// Change the image's size
	imgSizeX = x1 - x0;
	imgSizeY = y1 - y0;
}


// http://www.kevlindev.com/tutorials/geometry/simplify_polyline/index.htm



//
// Element output
//

inline void help_range(int& low, int& high, const int& value)
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
	while (it != elements.end())
	{
		switch (it->identifier)
		{
			// Point
			case 1:
				help_range(x0, x1, it->parameters[0]);
				help_range(y0, y1, it->parameters[1]);
				break;

			// Line
			case 2:
				help_range(x0, x1, it->parameters[0]);
				help_range(y0, y1, it->parameters[1]);
				help_range(x0, x1, it->parameters[2]);
				help_range(y0, y1, it->parameters[3]);
				break;
			default:
				throw std::string("unsupported element during size check");
		}
		++it;
	}
}


//
// Statistics
//

// The amount of elements
int Data::statElements()
{
	return elements.size();
}
