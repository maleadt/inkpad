/*
 * output.cpp
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
void Output::write(const std::string& inputFile, const std::string& inputType) const
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
void Output::write(const std::string& inputFile) const
{
	// Detect the type
	std::string type;
	if (!file_identify(inputFile, type))
	{
		throw std::string("unsupported output file type");
		return;
	}

	// Write the vile
	write(inputFile, type);
}

// Write the data to a given wxWidgets draw container
void Output::write(wxMemoryDC& dc) const
{
	data_output_dc(dc);
}


//
// Data processing
//

// Output data in SVG format
void Output::data_output_svg(std::ofstream& stream) const
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
	list<Element>::const_iterator tempIterator = data->begin();
	while (tempIterator != data->end())
	{
		switch (tempIterator->identifier)
		{
			// Point
			case 1:
				break;

			// Polyline
			case 2:
				stream << "<polyline points=\"";
				for (unsigned int i = 0; i < tempIterator->parameters.size(); i += 2)
					stream << tempIterator->parameters[i] << "," << tempIterator->parameters[i+1] << " ";
				stream << "\" fill=\"none\" stroke=\"" << tempIterator->foreground .rgb_hex()<< "\" stroke-width=\"" << tempIterator->width << "px\"/>\n";
				break;

			// Polybezier
			case 3:
				stream << "<path d=\"";
				stream << "M" << tempIterator->parameters[0] << "," << tempIterator->parameters[1];
				for (unsigned int i = 2; i < tempIterator->parameters.size(); i += 6)
				{
					stream << " C" << tempIterator->parameters[i] << "," << tempIterator->parameters[i+1];
					stream << " " << tempIterator->parameters[i+2] << "," << tempIterator->parameters[i+3];
					stream << " " << tempIterator->parameters[i+4] << "," << tempIterator->parameters[i+5];
				}
				stream << "\" fill=\"none\" stroke=\"" << tempIterator->foreground .rgb_hex()<< "\" stroke-width=\"" << tempIterator->width << "px\"/>\n";
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
void Output::data_output_dc(wxMemoryDC& dc) const
{
    // Clear the DC
    dc.Clear();

	// Draw a background with border
	wxBrush brush;
	brush.SetColour( data->imgBackground.rgb_wxColor() );
	dc.SetBrush(brush);
	dc.DrawRectangle(0, 0, data->imgSizeX, data->imgSizeY);

	// Process all elements
	list<Element>::const_iterator tempIterator = data->begin();
	while (tempIterator != data->end())
	{
		switch (tempIterator->identifier)
		{
			// Point
			case 1:
				dc.SetPen( wxPen( tempIterator->foreground.rgb_wxColor(), tempIterator->width ) );
				dc.DrawPoint( tempIterator->parameters[0], tempIterator->parameters[1] );
				break;

			// Polyline
			case 2:
				dc.SetPen( wxPen( tempIterator->foreground.rgb_wxColor(), tempIterator->width ) );
				for (unsigned int i = 2; i < tempIterator->parameters.size(); i+=2)
					dc.DrawLine( tempIterator->parameters[i-2], tempIterator->parameters[i-1], tempIterator->parameters[i], tempIterator->parameters[i+1] );
				break;

			// Polybezier
			case 3:
			{
				dc.SetPen( wxPen( tempIterator->foreground.rgb_wxColor(), tempIterator->width ) );
				wxPoint points[tempIterator->parameters.size() / 2];
				int count = 0;
				for (unsigned int i = 0; i < tempIterator->parameters.size(); i+=2)
				{
					points[count].x = tempIterator->parameters[i];
					points[count].y= tempIterator->parameters[i+1];
					count++;
				}
				dc.DrawSpline(tempIterator->parameters.size()/2, points);
				break;
			}

			// Unsupported type
			default:
				throw std::string("unsupported element during dc output");
		}
		++tempIterator;
	}
}
