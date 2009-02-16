/*
 * render.cpp
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

// Headers
#include "render.h"


////////////////////
// CLASS ROUTINES //
////////////////////

//
// Construction and destruction
//

Render::Render()
{
}


//
// Class member routines
//

// Set the data-container pointer
void Render::setData(Data* inputDataPointer)
{
	data = inputDataPointer;
}

// Write the data to a wxWidgets draw container
void Render::write(wxDC& dc, const std::string render) const
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

	// Bogus if
	if (false)
	{
	}

	// Render using Cairo
	#ifdef RENDER_CAIRO
	else if (render == "cairo")
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
		render_output_cairo(cr, actualScale);

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
		delete[] dataWx, dataCairo;
		cairo_destroy(cr);
		cairo_surface_destroy(surface);
	}
	#endif

	// Render using wxWidgets
	#ifdef RENDER_WXWIDGETS
	else if (render == "wxwidgets")
	{
		// Create a temporary DC to draw on
		wxMemoryDC dc_mem;

		// Set the scale and origin
		dc_mem.SetUserScale(actualScale, actualScale);

		// Attach a bitmap to that DC
		wxBitmap dc_bitmap(maxX*actualScale, maxY*actualScale);
		dc_mem.SelectObject(dc_bitmap);

		// Draw
		render_output_dc(dc_mem);

		// Copy the temporary DC's content to the actual DC
		dc.Blit(wxPoint(0, 0), wxSize(maxX, maxY), &dc_mem, wxPoint(0, 0), wxCOPY);

		// Destruct the memory DC
		dc_mem.SelectObject(wxNullBitmap);
	}
	#endif

	// Unknown render
	// TODO: this throw is not catched
	else
	{
	    throw Exception("render", "write", "invalid render specified");
	}
}


//
// Informational routines
//

// List the available renders
void Render::render_available(vector<std::string>& data) const
{
    // Cairo render
    #ifdef RENDER_CAIRO
    data.push_back("cairo");
    #endif

    // wxWidgets render
    #ifdef RENDER_WXWIDGETS
    data.push_back("wxwidgets");
    #endif
}


//
// Data processing
//

// Output data to Cairo surface
#ifdef RENDER_CAIRO
void Render::render_output_cairo(cairo_t* cr, float scale) const
{
	// Clear the surface

	// Draw the background
	cairo_set_source_rgb(cr, BLACK.r, BLACK.g, BLACK.b);
	cairo_set_line_width(cr, 1);
	cairo_rectangle(cr, 1, 1, scale*data->imgSizeX-2, scale*data->imgSizeY-2);
	cairo_set_source_rgb(cr, data->imgBackground.r, data->imgBackground.b, data->imgBackground.g);
	cairo_fill(cr);

	// Process all elements
	list<Element>::const_iterator tempIterator = data->begin();
	while (tempIterator != data->end())
	{
		switch (tempIterator->identifier)
		{
				// Point
			case 1:
				cairo_set_source_rgb(cr, tempIterator->foreground.r, tempIterator->foreground.g, tempIterator->foreground.b);
				cairo_arc(cr, scale*tempIterator->parameters[0], scale*tempIterator->parameters[1], scale*1, 0, 2*M_PI);
				cairo_fill(cr);
				break;

				// Polyline
			case 2:
				cairo_set_source_rgb(cr, tempIterator->foreground.r, tempIterator->foreground.g, tempIterator->foreground.b);
				cairo_set_line_width(cr, scale*tempIterator->width);
				cairo_move_to(cr, scale*tempIterator->parameters[0], scale*tempIterator->parameters[1]);
				for (unsigned int i = 2; i < tempIterator->parameters.size(); i+=2)
					cairo_line_to(cr, scale*tempIterator->parameters[i], scale*tempIterator->parameters[i+1]);
				cairo_stroke(cr);
				break;

				// Unsupported type
			default:
                throw Exception("render", "render_output_cairo", "unsupported element with ID " + stringify(tempIterator->identifier));
		}
		++tempIterator;
	}
}
#endif

// Output data to wxWidgets draw container
#ifdef RENDER_WXWIDGETS
void Render::render_output_dc(wxMemoryDC& dc) const
{
	// Clear the DC
	dc.Clear();

	// Draw the background
	wxBrush brush;
	brush.SetColour(data->imgBackground.rgb_wxColor());
	dc.SetBrush(brush);
	dc.SetPen(wxPen(BLACK.rgb_wxColor(), 1));
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
                throw Exception("render", "render_output_wxwidgets", "unsupported element with ID " + stringify(tempIterator->identifier));
		}
		++tempIterator;
	}
}
#endif
