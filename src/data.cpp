/*
 * data.cpp
 * Inkpad data handling.
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

	// Cache reset
	cacheBoundsDirty = true;

	// Delete elements
	elements.clear();
}


//
// Element input
//

// Add a single point
void Data::addPoint(int x1, int y1)
{
	addPoint(x1, y1, elements.end());
}
void Data::addPoint(int x1, int y1, list<Element>::iterator it)
{
	// New element
	Element tempElement;
	tempElement.identifier = 1;

	// Save parameters
	tempElement.parameters.resize(2);
	tempElement.parameters[0] = x1;
	tempElement.parameters[1] = y1;

	// Save the element
	addElement(tempElement, elements.end());
}

// Add a new polyline
void Data::addPolyline(const vector<double>& points)
{
	addPolyline(points, elements.end());
}
void Data::addPolyline(const vector<double>& points, list<Element>::iterator it)
{
	// New element
	Element tempElement;
	tempElement.identifier = 2;

	// Save parameters
	tempElement.parameters = points;

	// Save the element
	addElement(tempElement, it);
}

// Add a new polybezier
void Data::addPolybezier(const vector<double>& points)
{
	addPolybezier(points, elements.end());
}
void Data::addPolybezier(const vector<double>& points, list<Element>::iterator it)
{
	// New element
	Element tempElement;
	tempElement.identifier = 3;

	// Save parameters
	tempElement.parameters = points;

	// Save the element
	addElement(tempElement, it);
}

// Add a new element (private, applies current settings)
void Data::addElement(Element& inputElement, list<Element>::iterator it)
{
	// Save pen condition
	inputElement.width = penWidth;
	inputElement.foreground = penForeground;
	inputElement.background = penBackground;

	// Save the element
	elements.insert(it, inputElement);

	// Invalidate caches
	cacheBoundsDirty = true;
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
	double angle_rad = angle / 180 * M_PI;

	// Move the image to it's center
	translate(-(imgSizeX/2), -(imgSizeY/2));

    // Process all items in a parallelised manner
    #pragma omp parallel
    {
        // Calculate a range
        #ifdef WITH_OPENMP
        boost::iterator_range<list<Element>::iterator> range = split_range_openmp(boost::make_iterator_range(elements.begin(), elements.end()), elements.size());
        #else
        boost::iterator_range<list<Element>::iterator> range = boost::make_iterator_range(elements.begin(), elements.end());
        #endif

        // Process the range
        list<Element>::iterator it;
        for (it = boost::begin(range); it != boost::end(range); ++it)
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

                // Polybezier
                case 3:
                    for (unsigned int i = 0; i < it->parameters.size(); i+=2)
                        help_rotate(it->parameters[i], it->parameters[i+1], angle_rad);
                    break;

                default:
                    throw Exception("data", "rotate", "unsupported element with ID " + stringify(it->identifier));
            }
        }
    }

	// Invalidate caches
	cacheBoundsDirty = true;

	// Move the image back to it's original location
	autocrop();
}

// Relocate the canvas
void Data::translate(int dx, int dy)
{
    // Process all items in a parallelised manner
    #pragma omp parallel
    {
        // Calculate a range
        #ifdef WITH_OPENMP
        boost::iterator_range<list<Element>::iterator> range = split_range_openmp(boost::make_iterator_range(elements.begin(), elements.end()), elements.size());
        #else
        boost::iterator_range<list<Element>::iterator> range = boost::make_iterator_range(elements.begin(), elements.end());
        #endif

        // Process the range
        list<Element>::iterator it;
        for (it = boost::begin(range); it != boost::end(range); ++it)
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

                    // Polybezier
                case 3:
                    for (unsigned int i = 0; i < it->parameters.size(); i+=2)
                    {
                        it->parameters[i] += dx;
                        it->parameters[i+1] += dy;
                    }
                    break;

                default:
                    throw Exception("data", "translate", "unsupported element with ID " + stringify(it->identifier));
            }
        }
    }

	// Invalidate caches
	cacheBoundsDirty = true;
}

// Crop the image automatically
void Data::autocrop()
{
	// Look up the size of our image
	int x0, y0, x1, y1;
	size(x0, y0, x1, y1);

	// Relocate the canvas
	translate(-x0, -y0);

	// Change the image's size
	imgSizeX = x1 - x0;
	imgSizeY = y1 - y0;

	// Invalidate caches
	cacheBoundsDirty = true;
}


//
// Data optimalisation
//

// Look for polylines
void Data::search_polyline()
{
	// Loop elements
	list<Element>::iterator it = elements.begin();
	while (it != elements.end())
	{
		// Initialize a polyline vector
		vector<double> polyline;

		// Add start point(s)
		switch (it->identifier)
		{
				// Point
			case 1:
				polyline.reserve(2);
				polyline.push_back(it->parameters[0]);
				polyline.push_back(it->parameters[1]);
				break;

				// Polyline
			case 2:
				polyline = it->parameters;
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
		list<Element>::iterator it_a = it;
		list<Element>::iterator it2 = ++it_a;
		while (it2 != elements.end())
		{
			// Compare ending point
			switch (it2->identifier)
			{
					// Point
				case 1:
					if (x == it2->parameters[0] && y == it2->parameters[1])
						found = true;
					break;

					// Polyline
				case 2:
					if (x == it2->parameters[0] && y == it2->parameters[1])
					{
						for (unsigned int i = 2; i < it2->parameters.size(); i++)
							polyline.push_back(it2->parameters[i]);
						found = true;
					}
					break;

					// Not supported form
				default:
					continue;
			}

			// If the line matched, remove it and push it up the temporary polyline
			if (found)
			{
				// Delete the old element
				it2 = elements.erase(it2);

				// Alter the new comparison points
				x = polyline[ polyline.size() - 2 ];
				y = polyline[ polyline.size() - 1 ];
				found = false;
			}
			else
			{
				++it2;
			}
		}

		// If the size differs, we have removed some lines, so save the resulting polyline
		if (oldsize != polyline.size())
		{
			it = elements.erase(it);
			addPolyline(polyline, it);
		}
		else
		{
			++it;
		}
	}
}

// Simplify polylines
// http://www.kevlindev.com/tutorials/geometry/simplify_polyline/index.htm
void Data::simplify_polyline(double radius)
{
	// Loop elements
	list<Element>::iterator it = elements.begin();
	while (it != elements.end())
	{
		switch (it->identifier)
		{
			case 2:
			{
				vector<double> result;

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

					// Calculate primary vector coefficients
					double lineX = curX - lastX;
					double lineY = curY - lastY;

					// Loop all points in between
					bool falls_in_between = true;
					for (unsigned int j = lastI+2; j < i-2 && falls_in_between; j+=2)
					{
						// Calculate distance from point to line through secondary vector coefficients (dot product)
						double pointX = it->parameters[j] - lastX;
						double pointY = it->parameters[j+1] - lastY;
						double dist = abs(pointX * lineY - lineX * pointY) / sqrt(lineX * lineX + lineY * lineY);

						// Check distance
						if (dist > radius)
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

				// And add the final point
				result.push_back(it->parameters[ it->parameters.size()-2 ]);
				result.push_back(it->parameters[ it->parameters.size()-1 ]);

				it->parameters = result;
				break;
			}

			default:
				break;
		}
		++it;
	}

	// Invalidate caches
	cacheBoundsDirty = true;
}

// Smoothn polylines
// http://www.sitepen.com/blog/2007/07/16/softening-polylines-with-dojox-graphics/
void Data::smoothn_polyline(double tension)
{
	// Loop elements
	list<Element>::iterator it = elements.begin();
	while (it != elements.end())
	{
		switch (it->identifier)
		{
			case 2:
			{
				// Resulting vector
				vector<double> result;
				result.push_back(it->parameters[0]);
				result.push_back(it->parameters[1]);

				// Loop polyline
				for (unsigned int i = 2; i < it->parameters.size(); i+=2)
				{
					result.reserve(result.size() + 6);

					// Calculate data
					double dx = it->parameters[i] - it->parameters[i-2];
					double add = dx / tension;

					// First control point
					result.push_back(it->parameters[i-2] + add);
					result.push_back(it->parameters[i-1]);

					// Second control point
					result.push_back(it->parameters[i] - add);
					result.push_back(it->parameters[i+1]);

					// End point
					result.push_back(it->parameters[i]);
					result.push_back(it->parameters[i+1]);
				}


				// Replace polyline with polybezier
				it = elements.erase(it);
				addPolybezier(result, it);
				break;
			}

			default:
				break;
		}
	}

	// Invalidate caches
	cacheBoundsDirty = true;
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
void Data::size(int& x0, int& y0, int &x1, int& y1)
{
    // Have we got data?
    if (elements.empty())
    {
        x0 = 0;
        y0 = 0;
        x1 = 0;
        y1 = 0;
    }

    // Do we really need a refresh?
    else if (!cacheBoundsDirty)
    {
        x0 = cacheBoundsLowerX;
        y0 = cacheBoundsLowerY;
        x1 = cacheBoundsUpperX;
        y1 = cacheBoundsUpperY;
    }

    // Sadly, we do really need to calculate the size
    else
    {
        // Starting value
        x0 = -1;
        y0 = -1;
        x1 = 0;
        y1 = 0;

        // Loop elements
        list<Element>::const_iterator it = elements.begin();
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

                // Polybezier
                case 3:
                    for (unsigned int i = 0; i < it->parameters.size(); i+=2)
                    {
                        help_range(x0, x1, it->parameters[i]);
                        help_range(y0, y1, it->parameters[i+1]);
                    }
                    break;

                default:
                    throw Exception("data", "size", "unsupported element with ID " + stringify(it->identifier));
            }
            ++it;
        }
    }

    // Save state to cache
    if (cacheBoundsDirty)
    {
        cacheBoundsDirty = false;
        cacheBoundsLowerX = x0;
        cacheBoundsLowerY = y0;
        cacheBoundsUpperX = x1;
        cacheBoundsUpperY = y1;
    }
}


//
// Statistics
//

// The amount of elements
int Data::statElements() const
{
	return elements.size();
}

// The amount of parameters
int Data::statParameters() const
{
	// Loop elements
	int count = 0;
	list<Element>::const_iterator it = elements.begin();
	while (it != elements.end())
	{
		switch (it->identifier)
		{
			case 1:
				count += 2;
				break;
			case 2:
				count += 2*it->parameters.size();
			case 3:
				count += 2*it->parameters.size();
			default:
				break;
		}
		++it;
	}
	return count;
}
