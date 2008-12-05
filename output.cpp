/*
 * output.cpp
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
 * Comments:
 *  - SVG implemented by http://www.w3.org/TR/SVG/
 *
 */

///////////////////
// CONFIGURATION //
///////////////////

//
// Essential stuff
//

// Headers
#include "output.h"


////////////////////
// CLASS ROUTINES //
////////////////////

//
// Construction and destruction
//

Output::Output()
{
}


//
// Class member routines
//

// Set the data-container pointer
void Output::setData(Data* inputDataPointer)
{
	data = inputDataPointer;
}

// Write the data to a given file (in a given format)
void Output::write(const std::string& inputFile, const std::string& inputType)
{
	// Decapitalize given type
	std::string typeLC;
	typeLC.resize(inputType.length());
	for (unsigned int i = 0; i < inputType.size(); i++)
		typeLC[i] = tolower(inputType[i]);

	// Process all cases
	if (typeLC == "svg")
	{
		std::ofstream stream;
		file_open(stream, inputFile);
		data_output_svg(stream);
		file_close(stream);
	}

	// We got an undetected case
	else
	{
		throw std::string("unknown output file type");
		return;
	}
}

// Write the data to a given file (but detect the format)
void Output::write(const std::string& inputFile)
{
	// Detect the type
	std::string type;
	if (!data_type(inputFile, type))
	{
		throw std::string("unsupported output file type");
		return;
	}

	// Write the vile
	write(inputFile, type);
}

// Write the data to a given wxWidgets draw container
void Output::write(wxDC& dc)
{
	data_output_dc(dc);
}


//
// File handling
//

// Open a file
void Output::file_open(std::ofstream& stream, const std::string& inputFile)
{
	// Open the stream
	stream.open(inputFile.c_str());

	// Check stream validity
	if (!stream.is_open())
	{
		throw std::string("error while opening output stream");
	}
}

// Close a file
void Output::file_close(std::ofstream& inputStream)
{
	inputStream.close();
}


//
// Data processing
//

// Guess the data type
bool Output::data_type(const std::string& inputFile, std::string& type)
{
	// Extension check
	unsigned int position = inputFile.find_last_of(".");
	if (position < inputFile.length())
	{
		std::string extension = inputFile.substr(position+1);
		type = extension;
		return true;
	}
	return false;
}

// Output data in SVG format
void Output::data_output_svg(std::ofstream& stream)
{
	// Print the SVG header
	stream	<< "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
			<< "<svg xmlns=\"http://www.w3.org/2000/svg\"\n"
			<< "	xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n"
			<< "	xmlns:ev=\"http://www.w3.org/2001/xml-events\"\n"
			<< "	version=\"1.1\" baseProfile=\"full\"\n"
			<< "	width=\"" << data->imgSizeX << "\" height=\"" << data->imgSizeY << "\" viewBox=\"0 0 " << data->imgSizeX << " " << data->imgSizeY << "\">\n";

	// Add a background rectangle
	stream << "<rect x=\"0\" y=\"0\" width=\"" << data->imgSizeX << "\" height=\"" << data->imgSizeY << "\" fill=\"" << data->imgBackground.rgb_hex() << "\" stroke=\"" << data->imgBackground.rgb_hex() << "\" stroke-width=\"1px\" />\n";

	// Process all elements
	std::vector<Element>::const_iterator tempIterator = data->begin();
	while (tempIterator != data->end())
	{
		switch (tempIterator->identifier)
		{
			// A point
			case 1:
				break;

			// A line
			case 2:
				stream << "<line x1=\"" << tempIterator->parameters[0] << "\" y1=\"" << tempIterator->parameters[1] << "\" x2=\""
					 << tempIterator->parameters[2] << "\" y2=\"" << tempIterator->parameters[3] << "\" fill=\"none\" stroke=\"" << tempIterator->foreground .rgb_hex()<< "\" stroke-width=\"" << tempIterator->width << "px\"/>\n";
				break;

			// Unsupported type
			default:
				throw std::string("unsupported element during svg output");
		}
		++tempIterator;
	}

	// Print the SVG footer
	stream << "</svg>\n";
}

// Output data to wxWidgets draw container
void Output::data_output_dc(wxDC& dc)
{
	// Clear the dc
	wxBrush brush;
	brush.SetColour( data->imgBackground.rgb_wxColor() );
	dc.SetBackground(brush);
	dc.Clear();

	// Draw a border
	//dc.SetPen( wxPen( BLACK, 1 ) );
	dc.DrawRectangle(0, 0, data->imgSizeX, data->imgSizeY);

	// Process all elements
	std::vector<Element>::const_iterator tempIterator = data->begin();
	while (tempIterator != data->end())
	{
		switch (tempIterator->identifier)
		{
			// A point
			case 1:
				dc.SetPen( wxPen( tempIterator->foreground.rgb_wxColor(), tempIterator->width ) );
				dc.DrawPoint( tempIterator->parameters[0], tempIterator->parameters[1] );
				break;

			// A line
			case 2:
				dc.SetPen( wxPen( tempIterator->foreground.rgb_wxColor(), tempIterator->width ) );
				dc.DrawLine( tempIterator->parameters[0], tempIterator->parameters[1], tempIterator->parameters[2], tempIterator->parameters[3] );
				break;

			// Unsupported type
			default:
				throw std::string("unsupported element during dc output");
		}
		++tempIterator;
	}
}
