/*
 * output.h
 * Inkpad output handling.
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
#ifndef __OUTPUT
#define __OUTPUT


// Headers
#include <iostream>
#include <string>
#include <fstream>
#include "data.h"
#include "file.h"
#include <cmath>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif
#include <cairo/cairo.h>

// Containers
#ifdef WITH_USTD
#include "ustd.h"
using ustd::vector;
#else
#include <vector>
using std::vector;
#endif


//
// Constants
//

// Render
const std::string RENDER = "cairo";


//////////////////////
// CLASS DEFINITION //
//////////////////////

class Output
{
	public:
		// Construction and destruction
		Output();

		// Class member routines
		void setData(Data*);
		void write(const std::string& inputFile, const std::string& inputType) const;
		void write(const std::string& inputFile) const;
		void write(wxDC&) const;

	private:
		// Data processing
		void data_output_svg(std::ofstream&) const;
		void data_output_dc(wxMemoryDC&) const;
		void data_output_cairo(cairo_t*, float scale) const;

		// Data
		const Data* data;
};


// Include guard
#endif
