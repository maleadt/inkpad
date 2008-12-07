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

// Add a new polyline
void Data::addPolyline(const std::vector<double>& points)
{
	// New element
	Element tempElement;
	tempElement.identifier = 2;

	// Save parameters
	tempElement.parameters = points;

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

	// Move the image to it's center
	translate(-(imgSizeX/2), -(imgSizeY/2));

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

			// Polyline
			case 2:
				for (unsigned int i = 0; i < it->parameters.size(); i+=2)
					help_rotate(it->parameters[i], it->parameters[i+1], angle_rad);
				break;

			default:
				throw std::string("unsupported element during canvas rotation");
		}
		++it;
	}

	// Move the image back to it's original location
	translate((imgSizeX/2), (imgSizeY/2));

	// TODO: change the image's size correct
	// Temporary solution (still buggy)
	if (angle == 90 || angle == -90)
	{
		int temp = imgSizeX;
		imgSizeX = imgSizeY;
		imgSizeY = temp;
	}
	else if (angle == 180 || angle == -180 || ((int)angle%360) == 0)
	{
		// Do nothing
	}
	else
	{
		autocrop();
	}
	/*
	// Now move it so all is positive (alghorithm by Gert-Jan Stockman)
	int dx = (int)(cos(angle_rad) * (imgSizeX/2) - sin(angle_rad) * (imgSizeY/2) - (imgSizeX/2));
	int dy = (int)(sin(angle_rad) * (-imgSizeX/2) + cos(angle_rad) * (imgSizeY/2) - (imgSizeY/2));
	translate(dx, dy);
	*/
}

// Relocate the canvas
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

			// Polyline
			case 2:
				for (unsigned int i = 0; i < it->parameters.size(); i+=2)
				{
					it->parameters[i] += dx;
					it->parameters[i+1] += dy;
				}
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


//
// Data optimalisation
//

// Look for polylines
void Data::search_polyline()
{
	// Loop elements
	for (unsigned int i = 0; i < elements.size(); i++)
	{
		// Initialize a polyline vector
		std::vector<double> polyline;

		// Add start point(s)
		switch (elements[i].identifier)
		{
			// Point
			case 1:
				polyline.reserve(2);
				polyline.push_back(elements[i].parameters[0]);
				polyline.push_back(elements[i].parameters[1]);
				break;

			// Polyline
			case 2:
				polyline = elements[i].parameters;
				break;

			// Not supported form
			default:
				continue;
		}

		// Save the end points
		double x = polyline[ polyline.size() - 2 ];
		double y = polyline[ polyline.size() - 1 ];
		unsigned int oldsize = polyline.size();

		// Scan other elements to look for a match with those end points
		bool found = false;
		for (unsigned int j = i+1; j < elements.size(); j++)
		{
			// Compare ending point
			switch (elements[j].identifier)
			{
				// Point
				case 1:
					if (x == elements[j].parameters[0] && y == elements[j].parameters[1])
						found = true;
					break;

				// Polyline
				case 2:
					if (x == elements[j].parameters[0] && y == elements[j].parameters[1])
					{
						for (unsigned int i = 2; i < elements[j].parameters.size(); i++)
							polyline.push_back(elements[j].parameters[i]);
						found = true;
					}
					break;

				// Not supported form
				default:
					continue;
			}

			// If found
			if (found)
			{
				// Delete the old element
				elements.erase( elements.begin() + (j--) );

				// Alter the new comparison points
				x = polyline[ polyline.size() - 2 ];
				y = polyline[ polyline.size() - 1 ];
				found = false;
			}
		}

		// Save the polyline if we found something
		if (oldsize != polyline.size())
		{
			elements.erase( elements.begin() + (i--) );
			addPolyline(polyline);
		}
	}
}

// Simplify polylines
// http://www.kevlindev.com/tutorials/geometry/simplify_polyline/index.htm
void Data::simplify_polyline(double accuracy)
{
	// Loop elements
	std::vector<Element>::iterator it = elements.begin();
	while (it != elements.end())
	{
		switch (it->identifier)
		{
			case 2:
			{
				std::vector<double> result;

				// Define last point
				double lastX = it->parameters[0];
				double lastY = it->parameters[1];
				double lastI = 0;

				// Starting point should always go on the result
				result.push_back(lastX);
				result.push_back(lastY);

				// Loop other points
				for (unsigned int i = 4; i < it->parameters.size(); i+=2)
				{
					// Define current point
					double curX = it->parameters[i];
					double curY = it->parameters[i+1];

					// Derive (dx/dy)
					double d = (curX - lastX) / (curY - lastY);

					// Loop all points in between
					bool falls_in_between = true;
					for (unsigned int j = lastI; j < i; j+=2)
					{
						// Calculate borders in Y direction
						double y = curY + d*(it->parameters[j] - curX);
						double y_min = accuracy * y;
						double y_max = (2-accuracy) * y;

						// Check
						if (it->parameters[j+1] < y_min || it->parameters[j+1] > y_max)
							falls_in_between = false;
					}

					if (!falls_in_between)
					{
						result.push_back(curX);
						result.push_back(curY);

						lastX = curX;
						lastY = curY;
						lastI = i;
					}
				}

				it->parameters = result;
				break;
			}

			default:
				break;
		}
		++it;
	}
}


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

			// Polyline
			case 2:
				for (unsigned int i = 0; i < it->parameters.size(); i+=2)
				{
					help_range(x0, x1, it->parameters[i]);
					help_range(y0, y1, it->parameters[i+1]);
				}
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

// The amount of parameters
int Data::statParameters()
{
	// Loop elements
	int count = 0;
	std::vector<Element>::iterator it = elements.begin();
	while (it != elements.end())
	{
		switch (it->identifier)
		{
			case 1:
				count += 2;
				break;
			case 2:
				count += 2*it->parameters.size();
			default:
				break;
		}
		++it;
	}
	return count;
}
