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
#include <wx/wx.h>
#include <wx/sizer.h>


//////////////////////
// CLASS DEFINITION //
//////////////////////


//
// Unique identifiers
//

enum
{
	ID_Open = 1,
	ID_Save,
	ID_SaveAs,
	ID_Quit,

	ID_Undo,
	ID_Redo,
	ID_Rotate,

	ID_ZoomIn,
	ID_ZoomOut,
	ID_Fullscreen,

	ID_Settings,

	ID_About
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
	virtual bool OnInit();

	// Frame
	FrameMain *frame;

	// Draw pane
	DrawPane * drawPane;
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

// Frame events
FrameMain::FrameMain(const wxString& title, const wxPoint& pos, const wxSize& size)
: wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
	// File menu
	wxMenu *menuFile = new wxMenu;
	menuFile->Append( ID_Open, _T("&Open") );
	menuFile->Append( ID_Save, _T("&Save") );
	menuFile->Append( ID_SaveAs, _T("Save &as") );
	menuFile->AppendSeparator();
	menuFile->Append( ID_Quit, _T("&Quit") );

	// Edit menu
	wxMenu *menuEdit = new wxMenu;
	menuEdit->Append( ID_Undo, _T("&Undo") );
	menuEdit->Append( ID_Redo, _T("&Redo") );
	menuEdit->AppendSeparator();
	menuEdit->Append( ID_Rotate, _T("&Rotate") );

	// View menu
	wxMenu *menuView = new wxMenu;
	menuView->Append( ID_ZoomIn, _T("Zoom &in") );
	menuView->Append( ID_ZoomOut, _T("Zoom &out") );
	menuView->AppendSeparator();
	menuView->Append( ID_Fullscreen, _T("&Fullscreen") );

	// Tools menu
	wxMenu *menuTools = new wxMenu;
	menuTools->Append( ID_Settings, _T("&Settings") );

	// Help menu
	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append( ID_About, _T("&About...") );

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

// Menu event table
BEGIN_EVENT_TABLE(FrameMain, wxFrame)
	EVT_MENU(ID_Open, FrameMain::OnMenuOpen)
	EVT_MENU(ID_Save, FrameMain::OnMenuSave)
	EVT_MENU(ID_SaveAs, FrameMain::OnMenuSaveAs)
	EVT_MENU(ID_Quit, FrameMain::OnMenuQuit)

	EVT_MENU(ID_Undo, FrameMain::OnMenuUndo)
	EVT_MENU(ID_Redo, FrameMain::OnMenuRedo)
	EVT_MENU(ID_Rotate, FrameMain::OnMenuRotate)

	EVT_MENU(ID_ZoomIn, FrameMain::OnMenuZoomIn)
	EVT_MENU(ID_ZoomOut, FrameMain::OnMenuZoomOut)
	EVT_MENU(ID_Fullscreen, FrameMain::OnMenuFullscreen)

	EVT_MENU(ID_Settings, FrameMain::OnMenuSettings)

	EVT_MENU(ID_About, FrameMain::OnMenuAbout)
END_EVENT_TABLE()



//
// The draw pane
//


// Derive a new panel
class DrawPane : public wxPanel
{
	public:
		DrawPane(wxFrame* parent);

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
	frame = new FrameMain( _T("Hello World"), wxPoint(50,50), wxSize(440,600) );

	drawPane = new DrawPane( (wxFrame*) frame );
	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(drawPane, 1, wxEXPAND);

	frame->SetSizer(sizer);
	frame->SetAutoLayout(true);

	frame->Show(TRUE);
	SetTopWindow(frame);
	return TRUE;
}


///////////
// Frame //
///////////

//
// File-menu
//

// Open a file
void FrameMain::OnMenuOpen(wxCommandEvent& WXUNUSED(event))
{
}

// Save a file
void FrameMain::OnMenuSave(wxCommandEvent& WXUNUSED(event))
{
}

// Save-as a file
void FrameMain::OnMenuSaveAs(wxCommandEvent& WXUNUSED(event))
{
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
DrawPane::DrawPane(wxFrame* parent) :
wxPanel(parent)
{
}


//
// Event handling
//

// Panel needs to be redrawn
void DrawPane::paintEvent(wxPaintEvent& evt)
{
	wxPaintDC dc(this);
	render(dc);
}


//
// Rendering functions
//

// The actual rendering
void DrawPane::render(wxDC& dc)
{
	// draw some text
	dc.DrawText(wxT("Testing"), 40, 60);

	// draw a circle
	dc.SetBrush(*wxGREEN_BRUSH); // green filling
	dc.SetPen( wxPen( wxColor(255,0,0), 5 ) ); // 5-pixels-thick red outline
	dc.DrawCircle( wxPoint(200,100), 25 /* radius */ );

	// draw a rectangle
	dc.SetBrush(*wxBLUE_BRUSH); // blue filling
	dc.SetPen( wxPen( wxColor(255,175,175), 10 ) ); // 10-pixels-thick pink outline
	dc.DrawRectangle( 300, 100, 400, 200 );

	// draw a line
	dc.SetPen( wxPen( wxColor(0,0,0), 3 ) ); // black line, 3 pixels thick
	dc.DrawLine( 300, 100, 700, 300 ); // draw line across the rectangle

	// Look at the wxDC docs to learn how to draw other stuff
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
