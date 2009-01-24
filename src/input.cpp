/*
 * input.cpp
 * Inkpad input handling.
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
 *  - TOP specification reversly engineered, with help of the existing codebase from
 *    toptools
 *
 */

///////////////////
// CONFIGURATION //
///////////////////

//
// Essential stuff
//

// Headers
#include "input.h"

#define dbytes_to_value(h,l) ((((unsigned char)h)<<8)|((unsigned char)l))
#define byte_to_value(h) (((unsigned char)h)<<0)


////////////////////
// CLASS ROUTINES //
////////////////////

//
// Construction and destruction
//

Input::Input()
{
}


//
// Class member routines
//

// Set the data pointer
void Input::setData(Data* inputDataPointer)
{
	data = inputDataPointer;
}

// Read from the file
void Input::read(const std::string &inputFile)
{
	// Guess the data type from the extension
	std::string type;
	if (!file_identify(inputFile, type))
	{
		throw std::string("cannot extract input file type");
		return;
	}

	// Decapitalize given type
	for (unsigned int i = 0; i < type.size(); i++)
		type[i] = tolower(type[i]);

	// Process all cases
	if (type == "top")
	{
		std::ifstream stream;
		file_open(stream, inputFile);
		data_input_top(stream);
		file_close(stream);
	}
	else if (type == "dhw")
	{
		std::ifstream stream;
		file_open(stream, inputFile);
		data_input_dhw(stream);
		file_close(stream);
	}
	else
	{
		throw std::string("unsupported input file type");
		return;
	}
}


//
// Data generation routines
//

// Generate a set of static data
void Input::generate_static(int amount)
{
	// Configure the pen
	data->penWidth = 10;
	data->penForeground = BLACK;
	data->penBackground = WHITE;

	// Configure image defaults
	data->imgSizeX = GENERATE_WIDTH;
	data->imgSizeY = GENERATE_HEIGHT;
	data->imgBackground = WHITE;

    // Polylines
    double dx = GENERATE_WIDTH*4/amount;
    double dy = GENERATE_HEIGHT*4/amount;
    for (int i = 1; i < amount/4; i+=1)
    {
        vector<double> set(10);

        set[0] = i*dx;
        set[1] = i*dy;

        set[2] = GENERATE_WIDTH-i*dx;
        set[3] = i*dy;

        set[4] = GENERATE_WIDTH-i*dx;
        set[5] = GENERATE_HEIGHT-i*dy;

        set[6] = i*dx;
        set[7] = GENERATE_HEIGHT-i*dy;

        set[8] = i*dx;
        set[9] = (i+1)*dy;

        data->addPolyline(set);
    }
}



//
// Data processing
//

// TODO: implement more formats:
// MyScript Notes file format (.notes)
// Logitech PEN file format (.pen)
// Anoto Pen Generated Coordinates file format (.pgc)
// Nokia Pen Generated Document file format (.pgd)
// IBM Crosspad file format (.ps)
// Anoto file format (.svg)
// iRex iLiad file format (.irx)
// Pegasus NoteTaker file format (.pnt)

// Waltop file format (.top)
void Input::data_input_top(std::ifstream& stream)
{
	// General buffer variable
	char* buffer;

	// Read fileheader
	buffer = new char [6];
	stream.read(buffer, 6);
	if (strncmp(buffer, "WALTOP", 6) != 0)
	{
		throw std::string("header of top file seems damaged");
		return;
	}
	delete[] buffer;

	// Read first 26 bytes (unknown content)
	buffer = new char [26];
	stream.read(buffer, 26);
	delete[] buffer;

	// Check if file isn't empty
	if (stream.eof())
		return;

	// Configure the pen
	data->penWidth = 10;
	data->penForeground = BLACK;
	data->penBackground = WHITE;

	// Configure image defaults
	data->imgSizeX = 8800;
	data->imgSizeY = 12000;
	data->imgBackground = WHITE;

	// Initialise and read start coördinates
	buffer = new char [6];
	stream.read(buffer, 6);
	int x1 = dbytes_to_value(buffer[4], buffer[3]);
	int y1 = 12000 - dbytes_to_value(buffer[2], buffer[1]);

	// Read untill at end of file
	bool end_of_stroke = false;
	stream.read(buffer, 6);
	while (!stream.eof())
	{
		// Initialise and read end coördinates
		int x2 = dbytes_to_value(buffer[4], buffer[3]);
		int y2 = 12000 - dbytes_to_value(buffer[2], buffer[1]);

		// Create a new line (if we haven't started a new stroke
		if (!end_of_stroke)
		{
			vector<double> points;
			points.resize(4);
			points[0] = x1;
			points[1] = y1;
			points[2] = x2;
			points[3] = y2;
			data->addPolyline(points);
		}
		else
		{
			end_of_stroke = false;
		}

		// Switch the coördinates
		x1 = x2;
		y1 = y2;

		// Check if we should later on draw a line from these endpoints
		if (buffer[0] == 0)
		{
			end_of_stroke = true;
		}

		// Read new bytes
		stream.read(buffer, 6);
	}

	// Clear the buffer
	delete[] buffer;
}

// ACECAD DigiMemo file format (.dhw)
void Input::data_input_dhw(std::ifstream& stream)
{
	// General buffer variable
	char* buffer;

	// Fileheader
	buffer = new char [32];
	stream.read(buffer, 32);
	if (strncmp(buffer, "ACECAD_DIGIMEMO_HANDWRITING_____", 32) != 0)
	{
		throw std::string("header of dhw file seems damaged");
		return;
	}
	delete[] buffer;

	// Version
	char version = 1;
	buffer = new char [1];
	stream.read(buffer, 1);
	if (strncmp(buffer, &version, 1) != 0)
	{
		throw std::string("unsupported dhw version");
		return;
	}
	delete[] buffer;

	// Image size
	buffer = new char [4];
	stream.read(buffer, 4);
	data->imgSizeX = dbytes_to_value(buffer[1], buffer[0]);
	data->imgSizeY = dbytes_to_value(buffer[3], buffer[2]);
	delete[] buffer;

	// Background
	data->imgBackground = WHITE;

	// Page type
	// TODO: preserve field in data structure
	buffer = new char [1];
	stream.read(buffer, 1);
	std::string type;
	switch (byte_to_value(buffer[0]))
	{
		case 0:
			type = "A5";
			break;
		case 1:
			type = "A4";
			break;
		case 2:
			type = "A3";
			break;
		case 3:
			type = "A2";
			break;
		case 4:
			type = "A1";
			break;
		case 5:
			type = "A0";
			break;
		case 6:
			type = "B7";
			break;
		case 7:
			type = "B6";
			break;
		case 8:
			type = "B5";
			break;
		case 9:
			type = "B4";
			break;
		default:
			std::cout << "WARNING: DHW file page type is unknown (" << byte_to_value(buffer[0]) << ")" << std::endl;
			type = "unknown";
			break;
	}
	delete[] buffer;

	// Padding bytes
	buffer = new char[2];
	stream.read(buffer, 2);
	if (dbytes_to_value(buffer[1], buffer[0]) != 0)
	{
		throw std::string("dhw padding bytes invalid");
		return;
	}
	delete[] buffer;

	// Process the file
	vector<double> points;
	buffer = new char[1];
	stream.read(buffer, 1);
	while (!stream.eof())
	{
	    // Process the byte
	    int tag = byte_to_value(buffer[0]);

	    // Layer
	    if (tag == 0x90)
	    {
	        stream.read(buffer, 1);
	        int data = byte_to_value(buffer[0]);
	        //std::cout << "Layer number: " << data << std::endl;
	    }

	    // Timestamp
	    else if (tag == 0x88)
	    {
	        stream.read(buffer, 1);
	        int data = byte_to_value(buffer[0]);
	        //std::cout << "Timestamp: " << data << std::endl;
	    }

	    // Pen state
	    else if (tag >= 128 && tag <= 135)    // Pattern 10000XXX
	    {
	        // Pen up or down?
	        if (tag%2 == 0)
	        {
	            // Pen up
	            // Cannot save right now, still 1 point to follow
	        } else {
	            // Pen down, save previous points
	            if (!points.empty())
	            {
                    data->addPolyline(points);
                    points.clear();
	            }
	        }

	        // Extract colour
	        int colour = (tag >> 1)-64;
	        switch (colour)
	        {
	            case 0:
                    data->penForeground = BLACK;
                    break;
                case 1:
                    data->penForeground = RED;
                    break;
                case 2:
                    data->penForeground = BLUE;
                    break;
                case 3:
                    data->penForeground = GREEN;
                    break;
                default:
                    throw std::string("input-dhw: unknown pen colour");
                    break;
	        }
	    }

	    // Point
	    if (tag < 128)  // Pattern 0XXXXXXX
	    {
            // Read raw coördinates
	        int x1 = byte_to_value(buffer[0]);
	        stream.read(buffer, 1);
	        int x2 = byte_to_value(buffer[0]);
	        stream.read(buffer, 1);
	        int y1 = byte_to_value(buffer[0]);
	        stream.read(buffer, 1);
	        int y2 = byte_to_value(buffer[0]);

	        // Shift to actual coördinates
	        double x = x1 | x2<<7;
	        double y = data->imgSizeY - (y1 | y2<<7);

	        // Push them up the temporary queue
	        points.push_back(x);
	        points.push_back(y);
	    }

	    // Read a new byte
	    stream.read(buffer, 1);
	}

	// Push last series of points
	data->addPolyline(points);

	// Remove buffer
	delete[] buffer;
}
