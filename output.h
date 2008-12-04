/*
 * output.h
 * Inkpad output handling.
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
#ifndef __OUTPUT
#define __OUTPUT


// Headers
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "data.h"
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

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
		void write(const std::string& inputFile, const std::string& inputType);
		void write(const std::string& inputFile);
		void write(wxDC& dc);

	private:
		// File handling
		void file_open(std::ofstream&, const std::string& inputFile);
		void file_close(std::ofstream&);

		// Data processing
		bool data_type(const std::string &, std::string&);
		void data_output_svg(std::ofstream&);
		void data_output_dc(wxDC& dc);


		// Data
		const Data* data;
};


// Include guard
#endif
