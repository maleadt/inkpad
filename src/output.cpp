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

// Write the data to a file (but detect the format)
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

// Write the data to a wxWidgets draw container
void Output::write(wxDC& dc) const
{
    // Get the current image's size
	float maxX = (float)data->imgSizeX;
	float maxY = (float)data->imgSizeY;

	// Get the size of the DC in pixels
	int w, h;
	dc.GetSize(&w, &h);

	// Calculate a suitable scaling factor
	float scaleX=(float)(w/maxX);
	float scaleY=(float)(h/maxY);

	// Use x or y scaling factor, whichever fits on the DC (but beware of 10% margin)
	float actualScale = wxMin(scaleX,scaleY)*0.95;

	// Calculate the new dimensins
	int width = maxX * actualScale + 0.5;
	int height = maxY * actualScale + 0.5;

	// Center the image
	float posX = (float)((w - (maxX*actualScale))/2.0);
	float posY = (float)((h - (maxY*actualScale))/2.0);
	dc.SetDeviceOrigin((long)posX, (long)posY);

	// Render using Cairo
	if (RENDER == "cairo")
	{
		// Surface format
		cairo_format_t format = CAIRO_FORMAT_RGB24;

		// Create the buffers
		unsigned char *dataCairo = new unsigned char[width*height*4];
		unsigned char *dataWx = new unsigned char[width*height*3];

		// Create a surface
		cairo_surface_t* surface;
		surface = cairo_image_surface_create_for_data(dataCairo, format, width, height, width*4);

		// Create cairo object
		cairo_t* cr;
		cr = cairo_create(surface);

		// Draw
		data_output_cairo(cr);

		// Convert from Cairo RGB24 format to wxImage BGR format.
		for (int y=0; y<height; y++)
		{
			for (int x=0; x<width; x++)
			{
				dataWx[x*3+y*width*3] = dataCairo[x*4+2+y*width*4];
				dataWx[x*3+1+y*width*3] = dataCairo[x*4+1+y*width*4];
				dataWx[x*3+2+y*width*3] = dataCairo[x*4+y*width*4];
			}
		}

		// Blit final image to the screen.
		wxBitmap m_bitmap(wxImage(width, height, dataWx, true));
		dc.DrawBitmap(m_bitmap, 0, 0, true);

		// Cleanup
		delete dataWx, dataCairo;
		cairo_destroy(cr);
		cairo_surface_destroy(surface);
	}

	// Render using wxWidgets
	else if (RENDER == "wxwidgets")
	{
		// Create a temporary DC to draw on
		wxMemoryDC dc_mem;

		// Set the scale and origin
		dc_mem.SetUserScale(actualScale, actualScale);

		// Attach a bitmap to that DC
		wxBitmap dc_bitmap(maxX*actualScale, maxY*actualScale);
		dc_mem.SelectObject(dc_bitmap);

		// Draw
		data_output_dc(dc_mem);

		// Copy the temporary DC's content to the actual DC
		dc.Blit(wxPoint(0, 0), wxSize(maxX, maxY), &dc_mem, wxPoint(0, 0), wxCOPY);

		// Destruct the memory DC
		dc_mem.SelectObject(wxNullBitmap);
	}

	// Unknown render
	else
	{
	    throw std::string("unknown render");
	}
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

// Output data to Cairo surface
void Output::data_output_cairo(cairo_t* cr) const
{

}

// Output data to wxWidgets draw container
void Output::data_output_dc(wxMemoryDC& dc) const
{
	// Clear the DC
	dc.Clear();

	// Draw the background
	wxBrush brush;
	brush.SetColour(data->imgBackground.rgb_wxColor());
	dc.SetBrush(brush);
	dc.DrawRectangle(0, 0, data->imgSizeX-1, data->imgSizeY-1);

	// Process all elements
	list<Element>::const_iterator tempIterator = data->begin();
	while (tempIterator != data->end())
	{
		switch (tempIterator->identifier)
		{
				// Point
			case 1:
				dc.SetPen(wxPen(tempIterator->foreground.rgb_wxColor(), tempIterator->width));
				dc.DrawPoint(tempIterator->parameters[0], tempIterator->parameters[1]);
				break;

				// Polyline
			case 2:
				dc.SetPen(wxPen(tempIterator->foreground.rgb_wxColor(), tempIterator->width));
				for (unsigned int i = 2; i < tempIterator->parameters.size(); i+=2)
					dc.DrawLine(tempIterator->parameters[i-2], tempIterator->parameters[i-1], tempIterator->parameters[i], tempIterator->parameters[i+1]);
				break;

				// Polybezier
			case 3:
				{
					dc.SetPen(wxPen(tempIterator->foreground.rgb_wxColor(), tempIterator->width));
					wxPoint* points = new wxPoint[tempIterator->parameters.size() / 2];
					int count = 0;
					for (unsigned int i = 0; i < tempIterator->parameters.size(); i+=2)
					{
						points[count].x = tempIterator->parameters[i];
						points[count].y = tempIterator->parameters[i+1];
						count++;
					}
					dc.DrawSpline(tempIterator->parameters.size()/2, points);
					delete[] points;
					break;
				}

				// Unsupported type
			default:
				throw std::string("unsupported element during dc output");
		}
		++tempIterator;
	}
}
