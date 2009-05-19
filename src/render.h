/*
 * render.h
 * Inkpad render engines.
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
#ifndef __RENDER
#define __RENDER

// System headers
#include <iostream>
#include <string>
#include "data.h"
#include <cmath>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

// Application headers
#include "exception.h"
#include "generic.h"

// Containers
#include <vector>
using std::vector;


//
// Render engines
//

// Cairo
#ifdef RENDER_CAIRO
#include <cairo/cairo.h>
#endif

// wxWidgets
#ifdef RENDER_WXWIDGETS
#endif


//////////////////////
// CLASS DEFINITION //
//////////////////////

/*
TODO
Which renders are available are now pulled out through render_Available.
This should also be done for output files, through output_file_available or so.
Therefore, file output and screen output should be split somehow!
*/

class Render
{
	public:
		// Construction and destruction
		Render();

		// Class member routines
		void setData(Data*);
		void write(wxDC&, const std::string) const;

		// Informational routines
		void render_available(vector<std::string>&) const;

	private:
		// Data processing
        #ifdef RENDER_CAIRO
		void render_output_cairo(cairo_t*, float scale) const;
		#endif
		#ifdef RENDER_WXWIDGETS
		void render_output_dc(wxMemoryDC&) const;
		#endif

		// Data
		const Data* data;
};


// Include guard
#endif
