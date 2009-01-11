/*
 * main.cpp
 * Inkpad main executable.
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

// TODO: throw custom exception, include from general.h
// http://stackoverflow.com/questions/134569/c-exception-throwing-stdstring


//
// Essential stuff
//

// Application headers
#include "input.h"
#include "output.h"
#include "data.h"

// Default headers
#include <iostream>
#include <ctime>
#include <wx/filename.h>
#include <wx/cmdline.h>
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
	MENU_Autocrop,

	MENU_ZoomIn,
	MENU_ZoomOut,
	MENU_Fullscreen,

	MENU_Settings,
	MENU_SearchPolylines,
	MENU_SimplifyPolylines,

	MENU_About,

	TOOL_Open,
	TOOL_Save,
	TOOL_Left,
	TOOL_Right,
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
		FrameMain* frame;
		DrawPane* drawPane;

		// Data
		Input* engineInput;
		Output* engineOutput;
		Data* engineData;

		// File handline
		void setfile_save(const wxFileName&);
		void setfile_load(const wxFileName&);
		wxFileName getfile_save();
		wxFileName getfile_load();
		void clearfile_save();
		void clearfile_load();

	private:
		// Initialisation
		virtual bool OnInit();
		bool InitBatch();
		bool InitGui();

		// Command-line parser
		virtual void OnInitCmdLine(wxCmdLineParser& parser);
		virtual bool OnCmdLineParsed(wxCmdLineParser& parser);

		// Active files
		wxFileName file_save;
		wxFileName file_load;


		// Application flags
		bool batch;
};

// Configure the command-line parameters
static const wxCmdLineEntryDesc g_cmdLineDesc [] =
{
	// Switches
	 { wxCMD_LINE_SWITCH, wxT("h"), wxT("help"), wxT("displays help on the command line parameters"),
		  wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
	 { wxCMD_LINE_SWITCH, wxT("b"), wxT("batch"), wxT("work in batch modus (no gui)"),
		  wxCMD_LINE_VAL_NONE},

	// Options
	 { wxCMD_LINE_OPTION, wxT("bi"), wxT("batch-input"), wxT("read from specific file"),
		  wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_NEEDS_SEPARATOR },
	 { wxCMD_LINE_OPTION, wxT("bo"), wxT("batch-output"), wxT("write to specific file"),
		  wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_NEEDS_SEPARATOR },

	// Standard unnamed parameter
	 { wxCMD_LINE_PARAM, 0, 0, wxT("FILE"),
		  wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },

	 { wxCMD_LINE_NONE }
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
		void OnMenuAutocrop(wxCommandEvent& event);

		// View menu
		void OnMenuZoomIn(wxCommandEvent& event);
		void OnMenuZoomOut(wxCommandEvent& event);
		void OnMenuFullscreen(wxCommandEvent& event);

		// Tools menu
		void OnMenuSettings(wxCommandEvent& event);
		void OnMenuSearchPolylines(wxCommandEvent& event);
		void OnMenuSimplifyPolylines(wxCommandEvent& event);

		// Help menu
		void OnMenuAbout(wxCommandEvent& event);

		// Toolbar
		void OnToolOpen(wxCommandEvent& event);
		void OnToolSave(wxCommandEvent& event);
		void OnToolLeft(wxCommandEvent& event);
		void OnToolRight(wxCommandEvent& event);

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
	EVT_MENU(MENU_Autocrop, FrameMain::OnMenuAutocrop)

	EVT_MENU(MENU_ZoomIn, FrameMain::OnMenuZoomIn)
	EVT_MENU(MENU_ZoomOut, FrameMain::OnMenuZoomOut)
	EVT_MENU(MENU_Fullscreen, FrameMain::OnMenuFullscreen)

	EVT_MENU(MENU_Settings, FrameMain::OnMenuSettings)
	EVT_MENU(MENU_SearchPolylines, FrameMain::OnMenuSearchPolylines)
	EVT_MENU(MENU_SimplifyPolylines, FrameMain::OnMenuSimplifyPolylines)

	EVT_MENU(MENU_About, FrameMain::OnMenuAbout)

	EVT_TOOL(TOOL_Open, FrameMain::OnToolOpen)
	EVT_TOOL(TOOL_Save, FrameMain::OnToolSave)
	EVT_TOOL(TOOL_Left, FrameMain::OnToolLeft)
	EVT_TOOL(TOOL_Right, FrameMain::OnToolRight)

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
	EVT_PAINT(DrawPane::paintEvent)
END_EVENT_TABLE()







/////////////////
// APPLICATION //
/////////////////


//
// General
//

// General initialisation
bool Inkpad::OnInit()
{
    // Call default behaviour (mandatory, it calls the command-line parser)
    if (!wxApp::OnInit())
        return false;

	// Spawn all engines
	engineInput = new Input;
	engineOutput = new Output;
	engineData = new Data;

	// Link input and output engines to data engine
	engineInput->setData(engineData);
	engineOutput->setData(engineData);

	// Call specific initialiser
	if (batch)
	{
		return InitBatch();
	}
	else
	{
		return InitGui();
	}
}

// Specific initialisation: batch mode
bool Inkpad::InitBatch()
{
	try
	{
		// Read file
		engineInput->read(std::string(getfile_load().GetFullPath().mb_str()));

		// Write file
		engineOutput->write(std::string(getfile_save().GetFullPath().mb_str()));
	}
	catch (std::string error)
	{
		std::cout << "Error: " << error << std::endl;
	}

	return false;
}

// Specific initialisation: GUI mode
bool Inkpad::InitGui()
{
	// Set title and size
	frame = new FrameMain( _T("Inkpad"), wxPoint(50,50), wxSize(440,600));
	frame->parent = this;

	// Should we load a file?
	if (getfile_load().IsOk())
	{
		// Give the input engine the file we selected
		try
		{
			engineInput->read(std::string(getfile_load().GetFullPath().fn_str()));
		}
		catch (std::string error)
		{
			wxString WXerror(error.c_str(), wxConvUTF8);
			wxLogError(_T("Error while reading: ") + WXerror + _T("."));
		}
	}

	// Add a new drawpane
	drawPane = new DrawPane( (wxFrame*) frame );
	drawPane->parent = this;
	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(drawPane, 1, wxEXPAND);
	frame->SetSizer(sizer);
	frame->SetAutoLayout(true);

	// Show the frame
	frame->Show(TRUE);
	SetTopWindow(frame);

	return true;
}

//
// File handling
//

void Inkpad::setfile_save(const wxFileName& inputFile)
{
	file_save = inputFile;
	file_save.Normalize( wxPATH_NORM_LONG | wxPATH_NORM_DOTS | wxPATH_NORM_TILDE | wxPATH_NORM_ABSOLUTE );
}
void Inkpad::setfile_load(const wxFileName& inputFile)
{
	file_load = inputFile;
	file_load.Normalize( wxPATH_NORM_LONG | wxPATH_NORM_DOTS | wxPATH_NORM_TILDE | wxPATH_NORM_ABSOLUTE );
}
wxFileName Inkpad::getfile_save()
{
	return file_save;
}
wxFileName Inkpad::getfile_load()
{
	return file_load;
}
void Inkpad::clearfile_save()
{
	file_save.Clear();
}
void Inkpad::clearfile_load()
{
	file_load.Clear();
}

//
// Command-line parser
//

void Inkpad::OnInitCmdLine(wxCmdLineParser& parser)
{
	parser.SetDesc (g_cmdLineDesc);
	// must refuse '/' as parameter starter or cannot use "/path" style paths
	parser.SetSwitchChars (wxT("-"));
}

bool Inkpad::OnCmdLineParsed(wxCmdLineParser& parser)
{
	// Get unnamed parameter (only one accepted)
	if (parser.GetParamCount() > 0)
	{
		setfile_load( wxFileName( parser.GetParam(0) ) );
	}

	// Check if we are in batch mode
	if (parser.Found( wxT("b")))
	{
		// Get input and output parameters
		wxString paramInput, paramOutput;
		if (parser.Found( wxT("bi"), &paramInput))
		{
			setfile_load( wxFileName(paramInput));
		}
		if (parser.Found( wxT("bo"), &paramOutput))
		{
			setfile_save( wxFileName(paramOutput));
		}

		// Batch mode requirments
		if (getfile_load().IsOk() && getfile_save().IsOk())
		{
			batch = true;
		} else {
			std::cout << "Batch mode requires given input and output parameters" << std::endl;
			parser.Usage();
			return false;
		}
	} else {
		batch = false;
	}

	return true;
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
	menuEdit->Append( MENU_Autocrop, _T("&Autocrop") );

	// View menu
	wxMenu *menuView = new wxMenu;
	menuView->Append( MENU_ZoomIn, _T("Zoom &in") );
	menuView->Append( MENU_ZoomOut, _T("Zoom &out") );
	menuView->AppendSeparator();
	menuView->Append( MENU_Fullscreen, _T("&Fullscreen") );

	// Tools menu
	wxMenu *menuTools = new wxMenu;
	menuTools->Append( MENU_Settings, _T("&Settings") );
	menuTools->AppendSeparator();
	menuTools->Append( MENU_SearchPolylines, _T("Search for &polylines") );
	menuTools->Append( MENU_SimplifyPolylines, _T("Simplify &polylines") );

	// Help menu
	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append( MENU_About, _T("&About...") );

	// Construct menu
	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append( menuFile, _T("&File") );
	menuBar->Append( menuEdit, _T("&Edit") );
	menuBar->Append( menuView, _T("&View") );
	menuBar->Append( menuTools, _T("&Tools") );
	menuBar->Append( menuHelp, _T("&Help") );
	SetMenuBar( menuBar );



	// Load images
	wxToolBar *toolbar = this->CreateToolBar();
	wxImage::AddHandler( new wxPNGHandler );
	wxBitmap open(wxT("/usr/share/icons/gnome/24x24/actions/gtk-open.png") );
	wxBitmap save(wxT("/usr/share/icons/gnome/24x24/actions/gtk-save.png") );
	wxBitmap left(wxT("/usr/share/icons/gnome/24x24/actions/object-rotate-left.png") );
	wxBitmap right(wxT("/usr/share/icons/gnome/24x24/actions/object-rotate-right.png") );

	// Construct the toolbar
	toolbar->AddTool(TOOL_Open, wxT("Open"), open, wxT("Open a new image"), wxITEM_NORMAL);
	toolbar->AddTool(TOOL_Save, wxT("Save"), save, wxT("Save the current image"), wxITEM_NORMAL);
	toolbar->AddSeparator();
	toolbar->AddTool(TOOL_Left, wxT("Left"), left, wxT("Rotate to the left"), wxITEM_NORMAL);
	toolbar->AddTool(TOOL_Right, wxT("Right"), right, wxT("Rotate to the right"), wxITEM_NORMAL);
	toolbar->Realize();




	// Statusbar
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
		wxT("TOP image files (*.top)|*.[tT][oO][pP]|DHW image files (*.top)|*.[dD][hH][wW]|"),
		wxFD_OPEN, wxDefaultPosition);

	// Creates a "open file" dialog
	if (OpenDialog->ShowModal() == wxID_OK)
	{
		try
		{
			// Give the input engine the file we selected
			parent->engineData->clear();
			parent->engineInput->read(std::string(OpenDialog->GetPath().mb_str()));

			// Save the loaded file
			parent->setfile_load(OpenDialog->GetPath());

			// Clear the "save" filename
			parent->clearfile_save();

			// Force a redraw
			wxClientDC dc(parent->drawPane);
			parent->drawPane->render(dc);
		}

		catch (std::string error)
		{
			wxString WXerror(error.c_str(), wxConvUTF8);
			wxLogError(_T("Error while reading: ") + WXerror + _T("."));
		}
	}
}


// Save a file
void FrameMain::OnMenuSave(wxCommandEvent& WXUNUSED(event))
{
	// Have we saved before?
	if (parent->getfile_save().IsOk())
	{
		try
		{
			parent->engineOutput->write(std::string(parent->getfile_save().GetFullPath().fn_str()));
		}

		catch (std::string error)
		{
			wxString WXerror(error.c_str(), wxConvUTF8);
			wxLogError(_T("Error while saving: ") + WXerror + _T("."));
		}
	}

	// If not, present the "save as" dialog
	else
	{
		wxFileDialog *SaveDialog = new wxFileDialog(
			this, _("Save file"), wxEmptyString, wxEmptyString,
			wxT("SVG vector image (*.svg)|*.[sS][vV][gG]|"),
			wxFD_SAVE|wxOVERWRITE_PROMPT, wxDefaultPosition);

		// Creates a "open file" dialog
		if (SaveDialog->ShowModal() == wxID_OK)
		{
			try
			{
				// Give the input engine the file we selected
				parent->engineOutput->write(std::string(SaveDialog->GetPath().mb_str()));
				parent->setfile_save(SaveDialog->GetPath());
			}

			catch (std::string error)
			{
				wxString WXerror(error.c_str(), wxConvUTF8);
				wxLogError(_T("Error while saving: ") + WXerror + _T("."));
			}
		}
	}
}

// Save-as a file
void FrameMain::OnMenuSaveAs(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog *SaveDialog = new wxFileDialog(
		this, _("Save file"), wxEmptyString, wxEmptyString,
		wxT("SVG vector image (*.svg)|*.[sS][vV][gG]|"),
		wxFD_SAVE|wxOVERWRITE_PROMPT, wxDefaultPosition);

	// Creates a "open file" dialog
	if (SaveDialog->ShowModal() == wxID_OK)
	{
		try
		{
			// Give the input engine the file we selected
			parent->engineOutput->write(std::string(SaveDialog->GetPath().mb_str()));
			parent->setfile_save(SaveDialog->GetPath());
		}

		catch (std::string error)
		{
			wxString WXerror(error.c_str(), wxConvUTF8);
			wxLogError(_T("Error while reading: ") + WXerror + _T("."));
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
	// Aks the user an amount
	wxString angle_string = wxGetTextFromUser(_T("Angle to rotate (in degrees):"), _T("Rotate"), _T("90"));
	double angle;
	angle_string.ToDouble(&angle);

	// Rotate
	parent->engineData->rotate(angle);

	// Redraw
	wxClientDC dc(parent->drawPane);
	parent->drawPane->render(dc);
}

// Automatically crop the image
void FrameMain::OnMenuAutocrop(wxCommandEvent& WXUNUSED(event))
{
	// Autocrop
	parent->engineData->autocrop();

	// Redraw
	wxClientDC dc(parent->drawPane);
	parent->drawPane->render(dc);
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

// Search for polylines
void FrameMain::OnMenuSearchPolylines(wxCommandEvent& WXUNUSED(event))
{
	// Redraw
	try
	{
		parent->engineData->search_polyline();
		wxClientDC dc(parent->drawPane);
		parent->drawPane->render(dc);
	}
	catch (std::string error)
	{
		wxString WXerror(error.c_str(), wxConvUTF8);
		wxLogError(_T("Error while drawing: ") + WXerror + _T("."));
	}
}

// Simplify polylines
void FrameMain::OnMenuSimplifyPolylines(wxCommandEvent& WXUNUSED(event))
{
	// Redraw
	try
	{
		parent->engineData->simplify_polyline(1.5);
		wxClientDC dc(parent->drawPane);
		parent->drawPane->render(dc);
	}
	catch (std::string error)
	{
		wxString WXerror(error.c_str(), wxConvUTF8);
		wxLogError(_T("Error while drawing: ") + WXerror + _T("."));
	}
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
// Toolbar
//

// Open a new file
void FrameMain::OnToolOpen(wxCommandEvent& event)
{
	OnMenuOpen(event);
}

// Save the file
void FrameMain::OnToolSave(wxCommandEvent& event)
{
	OnMenuSave(event);
}

// Rotate left

void FrameMain::OnToolLeft(wxCommandEvent& WXUNUSED(event))
{
		// Rotate
	parent->engineData->rotate(-90);

	// Redraw
	wxClientDC dc(parent->drawPane);
	parent->drawPane->render(dc);
}

// Rotate right

void FrameMain::OnToolRight(wxCommandEvent& WXUNUSED(event))
{
		// Rotate
	parent->engineData->rotate(90);

	// Redraw
	wxClientDC dc(parent->drawPane);
	parent->drawPane->render(dc);
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
void DrawPane::paintEvent(wxPaintEvent& WXUNUSED(event))
{
	// Force a redraw
	wxPaintDC dc(this);
	this->render(dc);
}


//
// Rendering functions
//

// The actual rendering
void DrawPane::render(wxDC& dc)
{
	// Only draw if we have data
	if (parent->engineData->statElements() > 0)
	{
		// Set the DC's title
		parent->frame->SetTitle(_T("Inkpad - ") + parent->getfile_load().GetName());

		// Get the current image's size
		float maxX = (float)parent->engineData->imgSizeX;
		float maxY = (float)parent->engineData->imgSizeY;

		// Get the size of the DC in pixels
		int w, h;
		dc.GetSize(&w, &h);

		// Calculate a suitable scaling factor
		float scaleX=(float)(w/maxX);
		float scaleY=(float)(h/maxY);

		// Use x or y scaling factor, whichever fits on the DC (but beware of 10% margin)
		float actualScale = wxMin(scaleX,scaleY)*0.95;

		// Calculate the position on the DC for centring the graphic
		float posX = (float)((w - (maxX*actualScale))/2.0);
		float posY = (float)((h - (maxY*actualScale))/2.0);

		// Set the scale and origin
		dc.SetUserScale(actualScale, actualScale);
		dc.SetDeviceOrigin( (long)posX, (long)posY );

		// Output elements
		parent->engineOutput->write(dc);

		// Adjust status bar
		wxString statusbar;
		statusbar << parent->engineData->imgSizeX << _T(" x ") << parent->engineData->imgSizeY << _T(" pixels")
				  << _T(" (") << parent->engineData->statElements() << _T(" elements with ") << parent->engineData->statParameters() << _T(" parameters)");
		parent->frame->SetStatusText(  statusbar );
	}
}





