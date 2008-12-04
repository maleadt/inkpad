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


//////////////////////
// CLASS DEFINITION //
//////////////////////

//
// The application itself
//

// Derive a new class
class Inkpad: public wxApp
{
	virtual bool OnInit();
};


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


//
// Menu identifiers
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



////////////////////
// CLASS ROUTINES //
////////////////////


//
// Application functions
//

bool Inkpad::OnInit()
{
	FrameMain *frame = new FrameMain( _T("Hello World"), wxPoint(50,50), wxSize(440,600) );
	frame->Show(TRUE);
	SetTopWindow(frame);
	return TRUE;
}


//
// Main window functions
//

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



//////////
// MENU //
//////////

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



//
// Main application
//

// Start the application
IMPLEMENT_APP(Inkpad)



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
