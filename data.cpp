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
}


//
// Input routines
//

// Add a single point
void Data::addPoint(Point &inputPoint)
{
	points.push_back(inputPoint);
}

// Add a new line
void Data::addLine(Line &inputLine)
{
	lines.push_back(inputLine);
}

