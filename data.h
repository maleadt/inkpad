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
#include <string.h>
#include <cmath>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif


//
// Constants
//

// Mathematical
#define PI 3.14159265


////////////////
// DATA TYPES //
////////////////

// A colour
struct Colour
{
	Colour()
	{
	}
	Colour(int _r, int _g, int _b) : r(_r), g(_g), b(_b)
	{
	}

	std::string rgb_hex() const
	{
		std::string hex = "#";
		char buffer[3];
		sprintf(buffer, "%.2X", r);
		hex += buffer;
		sprintf(buffer, "%.2X", g);
		hex += buffer;
		sprintf(buffer, "%.2X", b);
		hex += buffer;
		return hex;
	}

	wxColor rgb_wxColor() const
	{
		wxColor wx(r, g, b);
		return wx;
	}

	int r;
	int g;
	int b;
};

// The structure
struct Element
{
	// Data
	int identifier;
	std::vector<double> parameters;
	Colour foreground;
	Colour background;
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

// TODO: move hasdata from main to here
// TODO: cache control in getSize function

class Data
{
	public:
		// Construction and destruction
		Data();
		void clear();

		// Pen configuration
		int penWidth;
		Colour penBackground;
		Colour penForeground;

		// Image configuration
		int imgSizeX, imgSizeY;
		Colour imgBackground;

		// Element input
		void addPoint(int, int);
		void addLine(int, int, int, int);

		// Element conversion
		void rotate(double angle);
		void translate(int dx, int dy);
		void autocrop();

		// Element output
		void getSize(int&, int&, int&, int&) const;

		// Statistics
		int statElements();

		// Iterators
		typedef std::vector<Element>::const_iterator const_iterator;
		const_iterator begin() const { return elements.begin(); }
		const_iterator end() const { return elements.end(); }

	private:
		// Elements
		void addElement(Element&);
		std::vector<Element> elements;
};


////////////////////
// STATIC OBJECTS //
////////////////////

//
// Colours
//

static Colour BLACK(0, 0, 0);
static Colour WHITE(255, 255, 255);



// Include guard
#endif
