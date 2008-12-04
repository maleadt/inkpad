/*
 * main.cpp
 * Inkpad main executable.
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

// Application headers
#include "input.h"
#include "output.h"
#include "data.h"

// Default headers
#include <iostream>
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

#include <wx/sizer.h>


//////////////////////
// CLASS DEFINITION //
//////////////////////


//
// Unique identifiers
//

enum
{
	MENU_Open = 1,
	MENU_Save,
	MENU_SaveAs,
	MENU_Quit,

	MENU_Undo,
	MENU_Redo,
	MENU_Rotate,

	MENU_ZoomIn,
	MENU_ZoomOut,
	MENU_Fullscreen,

	MENU_Settings,

	MENU_About
};


//
// Forward declarations
//

// Classes
class Inkpad;
class FrameMain;
class DrawPane;

//
// The application itself
//

// Derive a new class
class Inkpad: public wxApp
{
	public:
		// Elements
		FrameMain *frame;
		DrawPane * drawPane;

		// Data
		bool hasData;
		Input* engineInput;
		Output* engineOutput;
		Data* engineData;
		std::string filename;

	private:
		virtual bool OnInit();
};

// Implement it
IMPLEMENT_APP(Inkpad)


//
// The main window
//

// Derive a new frame: the main window
class FrameMain: public wxFrame
{
	public:
		FrameMain(const wxString& title, const wxPoint& pos, const wxSize& size);
		Inkpad* parent;

		// File menu
		void OnMenuOpen(wxCommandEvent& event);
		void OnMenuSave(wxCommandEvent& event);
		void OnMenuSaveAs(wxCommandEvent& event);
		void OnMenuQuit(wxCommandEvent& event);

		// Edit menu
		void OnMenuUndo(wxCommandEvent& event);
		void OnMenuRedo(wxCommandEvent& event);
		void OnMenuRotate(wxCommandEvent& event);

		// View menu
		void OnMenuZoomIn(wxCommandEvent& event);
		void OnMenuZoomOut(wxCommandEvent& event);
		void OnMenuFullscreen(wxCommandEvent& event);

		// Tools menu
		void OnMenuSettings(wxCommandEvent& event);

		// Help menu
		void OnMenuAbout(wxCommandEvent& event);

		DECLARE_EVENT_TABLE()
};

// Menu event table
BEGIN_EVENT_TABLE(FrameMain, wxFrame)
	EVT_MENU(MENU_Open, FrameMain::OnMenuOpen)
	EVT_MENU(MENU_Save, FrameMain::OnMenuSave)
	EVT_MENU(MENU_SaveAs, FrameMain::OnMenuSaveAs)
	EVT_MENU(MENU_Quit, FrameMain::OnMenuQuit)

	EVT_MENU(MENU_Undo, FrameMain::OnMenuUndo)
	EVT_MENU(MENU_Redo, FrameMain::OnMenuRedo)
	EVT_MENU(MENU_Rotate, FrameMain::OnMenuRotate)

	EVT_MENU(MENU_ZoomIn, FrameMain::OnMenuZoomIn)
	EVT_MENU(MENU_ZoomOut, FrameMain::OnMenuZoomOut)
	EVT_MENU(MENU_Fullscreen, FrameMain::OnMenuFullscreen)

	EVT_MENU(MENU_Settings, FrameMain::OnMenuSettings)

	EVT_MENU(MENU_About, FrameMain::OnMenuAbout)
END_EVENT_TABLE()



//
// The draw pane
//


// Derive a new panel
class DrawPane : public wxPanel
{
	public:
		DrawPane(wxFrame* _parent);
		Inkpad* parent;

		void paintEvent(wxPaintEvent& evt);
		void paintNow();

		void render(wxDC& dc);

		DECLARE_EVENT_TABLE()

};

// Link events
BEGIN_EVENT_TABLE(DrawPane, wxPanel)
	// catch paint events
	EVT_PAINT(DrawPane::paintEvent)
END_EVENT_TABLE()




/////////////////
// APPLICATION //
/////////////////


//
// General
//

bool Inkpad::OnInit()
{
	// Set title and size
	frame = new FrameMain( _T("Inkpad"), wxPoint(50,50), wxSize(440,600));
	frame->parent = this;

	// Add a new drawpane
	drawPane = new DrawPane( (wxFrame*) frame );
	drawPane->parent = this;
	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(drawPane, 1, wxEXPAND);

	frame->SetSizer(sizer);
	frame->SetAutoLayout(true);

	// Spawn all engines
	engineInput = new Input;
	engineOutput = new Output;
	engineData = new Data;
	hasData = false;

	// Link input and output engines to data engine
	engineInput->setData(engineData);
	engineOutput->setData(engineData);

	// Show the frame
	frame->Show(TRUE);
	SetTopWindow(frame);
	return TRUE;
}


///////////
// Frame //
///////////


//
// Construction
//

FrameMain::FrameMain(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame((wxFrame *)NULL, -1, title, pos, size, wxDEFAULT_FRAME_STYLE | wxFULL_REPAINT_ON_RESIZE)
{
	// File menu
	wxMenu *menuFile = new wxMenu;
	menuFile->Append( MENU_Open, _T("&Open") );
	menuFile->Append( MENU_Save, _T("&Save") );
	menuFile->Append( MENU_SaveAs, _T("Save &as") );
	menuFile->AppendSeparator();
	menuFile->Append( MENU_Quit, _T("&Quit") );

	// Edit menu
	wxMenu *menuEdit = new wxMenu;
	menuEdit->Append( MENU_Undo, _T("&Undo") );
	menuEdit->Append( MENU_Redo, _T("&Redo") );
	menuEdit->AppendSeparator();
	menuEdit->Append( MENU_Rotate, _T("&Rotate") );

	// View menu
	wxMenu *menuView = new wxMenu;
	menuView->Append( MENU_ZoomIn, _T("Zoom &in") );
	menuView->Append( MENU_ZoomOut, _T("Zoom &out") );
	menuView->AppendSeparator();
	menuView->Append( MENU_Fullscreen, _T("&Fullscreen") );

	// Tools menu
	wxMenu *menuTools = new wxMenu;
	menuTools->Append( MENU_Settings, _T("&Settings") );

	// Help menu
	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append( MENU_About, _T("&About...") );

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append( menuFile, _T("&File") );
	menuBar->Append( menuEdit, _T("&Edit") );
	menuBar->Append( menuView, _T("&View") );
	menuBar->Append( menuTools, _T("&Tools") );
	menuBar->Append( menuHelp, _T("&Help") );

	SetMenuBar( menuBar );

	CreateStatusBar();
	SetStatusText( _T("Inkpad initialised") );
}

//
// File-menu
//

// Open a file
void FrameMain::OnMenuOpen(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog *OpenDialog = new wxFileDialog(
		this, _("Open file"), wxEmptyString, wxEmptyString,
		wxT("TOP image files (*.top)|*.top|"),
		wxFD_OPEN, wxDefaultPosition);

	// Creates a "open file" dialog
	if (OpenDialog->ShowModal() == wxID_OK)
	{
		try
		{
			// Give the input engine the file we selected
			parent->engineInput->read(std::string(OpenDialog->GetPath().mb_str()));
			parent->hasData = true;

			// Change the window's title
			SetTitle(_T("Inkpad - ") + OpenDialog->GetFilename());

			// Force a redraw
			wxClientDC dc(parent->drawPane);
			parent->drawPane->render(dc);
		}

		catch (std::string error)
		{
			wxString WXerror(error.c_str(), wxConvUTF8);
			wxMessageBox(_T("Error while reading: ") + WXerror + _T("."),
				_T("Error"), wxOK | wxICON_ERROR, this);
		}
	}
}


// Save a file
void FrameMain::OnMenuSave(wxCommandEvent& WXUNUSED(event))
{
	// Have we saved before?
	if (parent->filename.length() > 0)
	{
		parent->engineOutput->write(parent->filename);
	}

	// If not, present the "save as" dialog
	else
	{
		//OnMenuSaveAs(event);
	}
}

// Save-as a file
void FrameMain::OnMenuSaveAs(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog *SaveDialog = new wxFileDialog(
		this, _("Save file"), wxEmptyString, wxEmptyString,
		wxT("SVG vector image (*.svg)|*.svg|"),
		wxFD_SAVE|wxOVERWRITE_PROMPT, wxDefaultPosition);

	// Creates a "open file" dialog
	if (SaveDialog->ShowModal() == wxID_OK)
	{
		try
		{
			// Give the input engine the file we selected
			parent->engineOutput->write(std::string(SaveDialog->GetPath().mb_str()));
			parent->filename = SaveDialog->GetPath().mb_str();
		}

		catch (std::string error)
		{
			wxString WXerror(error.c_str(), wxConvUTF8);
			wxMessageBox(_T("Error while saving: ") + WXerror + _T("."),
				_T("Error"), wxOK | wxICON_ERROR, this);
		}
	}
}

// Quit
void FrameMain::OnMenuQuit(wxCommandEvent& WXUNUSED(event))
{
	Close(TRUE);
}


//
// Edit-menu
//

// Undo last action
void FrameMain::OnMenuUndo(wxCommandEvent& WXUNUSED(event))
{
}

// Redo last undo
void FrameMain::OnMenuRedo(wxCommandEvent& WXUNUSED(event))
{
}

// Rotate the image
void FrameMain::OnMenuRotate(wxCommandEvent& WXUNUSED(event))
{
}



//
// View-menu
//

// Zoom in
void FrameMain::OnMenuZoomIn(wxCommandEvent& WXUNUSED(event))
{
}

// Zoom out
void FrameMain::OnMenuZoomOut(wxCommandEvent& WXUNUSED(event))
{
}

// View the image full-screen
void FrameMain::OnMenuFullscreen(wxCommandEvent& WXUNUSED(event))
{
}


//
// Tools-menu
//

// Configure the application
void FrameMain::OnMenuSettings(wxCommandEvent& WXUNUSED(event))
{
}


//
// Help-menu
//

// Display an about box
void FrameMain::OnMenuAbout(wxCommandEvent& WXUNUSED(event))
{
	wxMessageBox(_T("Inkpad is a piece of software capable of converting proprietary TOP files from the MEDION MD85276 Digital Notepad to some other formats.\n\nCurrent code by Tim Besard (tim.besard@gmail.com)."),
		_T("About inkpad"), wxOK | wxICON_INFORMATION, this);
}



//////////////
// DRAWPANE //
//////////////


//
// Construction and destruction
//

// Constructor
DrawPane::DrawPane(wxFrame* _parent) : wxPanel(_parent)
{
}


//
// Event handling
//

// Panel needs to be redrawn
void DrawPane::paintEvent(wxPaintEvent& evt)
{
	// Force a redraw
	wxPaintDC dc(this);
	render(dc);
}


//
// Rendering functions
//

// The actual rendering
void DrawPane::render(wxDC& dc)
{
	// Only draw if we have data
	if (parent->hasData)
	{
		// Get the current image's size
		std::cout << "Redrawing" << std::endl;
		int maxXi, maxYi;
		parent->engineData->getSize(maxXi, maxYi);
		float maxX = (float)maxXi;
		float maxY = (float)maxYi;

		// Get the size of the DC in pixels
		int w, h;
		dc.GetSize(&w, &h);

		// Calculate a suitable scaling factor
		float scaleX=(float)(w/maxX);
		float scaleY=(float)(h/maxY);

		// Use x or y scaling factor, whichever fits on the DC
		float actualScale = wxMin(scaleX,scaleY);

		// Calculate the position on the DC for centring the graphic
		float posX = (float)((w - (maxX*actualScale))/2.0);
		float posY = (float)((h - (maxY*actualScale))/2.0);

		// Set the scale and origin
		dc.SetUserScale(actualScale, actualScale);
		dc.SetDeviceOrigin( (long)posX, (long)posY );

		// Output elements
		parent->engineOutput->write(dc);
	}
}



/*
int main()
{
	std::cout << "* Application initializing" << std::endl;

	// Scan given folder for specific folder structure
	try
	{
		// Initialise objects
		std::cout << "* Initialising" << std::endl;
		Input engineInput;
		Output engineOutput;

		// Read the file
		std::cout << "* Reading data" << std::endl;
		engineInput.read("testfile.top");
		Data* engineData = engineInput.getdata();

		// Output the file
		std::cout << "* Writing data" << std::endl;
		engineOutput.write(engineData, "testfile.svg");
	}
	catch (std::string error)
	{
		std::cout << "Caught error: " << error << std::endl;
	}

	return 0;
}
*/
