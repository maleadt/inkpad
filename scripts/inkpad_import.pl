#!/usr/bin/perl

# Title
# ~~~~~
#  Medion Digital Ink Pad Convertion and Synchronisation
#
# Description
# ~~~~~~~~~~~
#  This script will fetch propreitary .TOP files from the
#    Medion MD 85276 Digital Ink Pad, copy them to a user-defined
#    local source, and convert them to SVG/XML GZIP compressed format.
#
# Automatisation
# ~~~~~~~~~~~~~~
#  Create a udev rule (/etc/udev/rules.d/95-inkpad.rule); containing:
#    ATTRS{model}=="Pen Pad storage ", NAME="PenPadStorage", RUN+="/home/tim/Scripts/Notepad - Import.pl"
#
#  This will start the script at insertion, and will allow the script to detect the mountpoint dynamically.
#
# Version History
# ~~~~~~~~~~~~~~~
#  v0.1
#    * Initial CLI released, based upon top2svg
#
# Todo
# ~~~~
#  * Enhance error-reporting
#  * Use Dialogs on errors
#  * Find out what the sixth bit in the .TOP file means, it isn't the stroke ID (as not using it results in less path breakages)
#
# Log priority levels
# ~~~~~~~~~~~~~~~~~~~
#  -3 = Fatal error (script cannot be launched)
#  -2 = Critical error (script has crashed)
#  -1 = Warning (possible failure)
#   0 = Neutral message
#   1 = Verbose message (main thread internals)
#   2 = Very verbose message (process launching)
#   3 = Extremely verbose message (process internals)
#
# Developer comments
# ~~~~~~~~~~~~~~~~~~
#  * Data structure of vector "@data_points"
#    Array of arrays, in which each array contains the following fields
#       - Type of stroke (_Line)
#       - Starting coördinate X
#       - Starting coördinate Y
#     [ - Point coördinate X ]
#     [ - Point coördinate Y ] x n
#  * Engine behaviour
#    Input
#      - Reference to @data_points
#      - Addition parameters
#    Output
#      - Reference to processed @data_points
#
# Thanks to
# ~~~~~~~~~
#  * Uwe Henschel, with his top2svg script, on which I inspired
#      my readTop algorithm
#

# Copyright (c) 2008 Tim Besard <tim.besard@gmail.com>
# All rights reserved.
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.



##########
##CONFIG##
##########

#
# Initialisation
#

# Needed modules
use strict;
use warnings;
use GD;
use GD::Polyline;
use Gtk2;
use Gtk2::SimpleMenu;

# Load engines
require 'engines/input.pm';
require 'engines/output.pm';
require 'engines/process.pm';
require 'engines/find.pm';

# Load helper scripts
require 'Gtk2_routines.pm';

# Constants
use constant TRUE  => 1;
use constant FALSE => 0;


#
# Shared variables
#

my $rcsid = '$Id:$';

# Layout properties
our %Layout = (
	'Colour_foreground'	=>	'black',
	'Colour_background'	=>	'white',
	
	'Thickness'		=>	10,
	'Border'		=>	5,
	
	# Default viewbox
	'X_min'			=>	0,
	'Y_min'			=>	0,
	'X_max'			=>	8500,
	'Y_max'			=>	12000,
	
	'Scale'			=>	10,
	'Output_format'		=>	"svg",
);

# Generic configuration propreties
our %Config = (
	'DeleteFiles'		=>	0,
	'DeleteFolders'		=>	0,
	'Verbosity'		=>	0,
);

# GUI
my $window;

# Binaries
our $bin_compress = "gzip";

# Redirect some handlers
$SIG{__WARN__} = sub { $_[0] =~ m/^(.+) at /; &log(-2, $1) };











#########################
#                       #
#                       #
#     Program modes     #
#                       #
#                       #
#########################


# Program modes
my (	$Mode_Cli,
	$Mode_Help,
	$Mode_Gui,
);

# Generic parameters
my (	$Opt_Generic_Verbose,
	$Opt_Generic_Quiet,
	$Opt_Generic_ReallyQuiet,
);

# Read the Command Input
my %Opt_Generic = (
	"cli"		=>	\$Mode_Cli,
	"gui"		=>	\$Mode_Gui,
	"help"		=>	\$Mode_Help,
	
	"verbosity"	=>	\$Opt_Generic_Verbose,
	"quiet"		=>	\$Opt_Generic_Quiet,
	"really-quiet"	=>	\$Opt_Generic_ReallyQuiet
);
parseParameters(\%Opt_Generic, 1);

# Detect program mode
$Mode_Gui = 1 if ((! $Mode_Help)&&(! $Mode_Cli));	# Prefer GUI if nothing specified
if ($Mode_Help) { $Mode_Gui = 0; $Mode_Cli = 0; }	# Help mode overrides everything
error(-2, "Cannot launch GUI as well as CLI.") if (($Mode_Gui)&&($Mode_Cli));

##########
###HELP###
##########

if ($Mode_Help)
{
	print <<END
Usage: inkpad_import.pl [OPTIONS]
Import proprietary .TOP files from a Medion MD 85276 Digital Ink Pad,
convert them to an Gzip compressed SVG/XML format, and save them
on a local source.

Examples:
    ./inkpad_import.pl
    
Program modes:
    --gui             Launch a graphical user interface
    --cli             Use a command line interface
    --help            Display this help

Graphical User Interface parameters:


Command Line Interface parameters:
 * Input parameters:
    --source=PATH     Source directory for the .TOP files.
                        Most likely this will the mount point
                        of your MD 85276.
    --delete          Delete source files.
   
 * Output parameters:
    --target=PATH     Target directory for the .SVG(Z) files.
                        Subdirectories will be created based on
                        the current date and the subfolders relative
                        to the source directory.
    --out-format=EXT  Format of output file [SVG-SVGZ-PNG-JP(E)G-GIF]
 
 * Processing parameters:
    --rotate=ANGLE    Rotate the image over a given angle.
    --scale=PERCENT   Scale the image by a given percent (default 10)
    --border=PERCENT  Whitespace border when cropping
    --no-border       Don't apply any border when cropping


Generic parameters:
    --verbosity=LVL   Level of verbose output [1, 2, 3]
    --quiet           Be quiet (only display errors and warnings)
    --really-quiet    Be really quiet (only display errors)


Copyright 2008, by Tim Besard (tim.besard\@gmail.com)
END
;
	exit;
}










#########
###GUI###
#########

if ($Mode_Gui)
{
	#
	# Command-line parameters
	#
	
	# Welcome message
	&log(0, "Initializing graphical user interface");
	
	
	#
	# Gtk2
	#
	
	# Create main window
	my $window = Gtk2_New_Window("Medion Digital Ink Pad Convertion and Synchronisation");
	
	# Main widgets
	my %toolbar_map = (
		"0 gtk-go-back - Back"		=>	\&GUI_Back,
		"1 gtk-go-forward - Forward"	=>	\&GUI_Forward,
		"2 gtk-go-up - Up"		=>	\&GUI_Up,
		"3 space"			=>	"",
		"4 gtk-convert - Convert"	=>	\&GUI_Convert,
		"5 gtk-clear - Clear"		=>	\&GUI_Clear,
		"6 space"			=>	"",
		"7 gtk-properties - Properties"	=>	\&GUI_Properties,
		"8 space"			=>	"",
		"9 gtk-quit - Quit"		=>	\&GUI_Quit,
		);
	my $toolbar = Gtk2_New_Toolbar(\%toolbar_map);
	my $layout =  &GUI_Layout;
	my $statusbar = Gtk2::Statusbar->new ();
	
	# Apply the interface
	my $vbox = Gtk2_New_VBox($window);
	$vbox->pack_start ($toolbar, 0, 0, 0);
	$vbox->add ($layout);
	$vbox->pack_start ($statusbar, 0, 0, 0);

	# Launch the window
	Gtk2_Launch($window);
	
	
	
	
	exit;
}











#########
###CLI###
#########

if ($Mode_Cli)
{
	#
	# Command-line parameters
	#
	
	# Welcome message
	&log(0, "Initializing command line interface");

	# Input parameters
	my (	$Opt_Input_Folder,
		$Opt_Input_Alternative,
		$Opt_Input_Delete,
	);

	# Processing parameters
	my (	$Opt_Process_Rotate,
		$Opt_Process_Scale,
		$Opt_Process_Border,
		$Opt_Process_NoBorder,
	);

	# Output parameters
	my (	$Opt_Output_Folder,
		$Opt_Output_Format,
	);

	# Read the Command Input
	my %Opt_Specific = (
		%Opt_Generic,					# Avoid complaints for program mode parameters
		"source"	=>	\$Opt_Input_Folder,
		"delete"	=>	\$Opt_Input_Delete,
	
		"rotate"	=>	\$Opt_Process_Rotate,
		"scale"		=>	\$Opt_Process_Scale,
		"border"	=>	\$Opt_Process_Border,
		"no-border"	=>	\$Opt_Process_NoBorder,
	
		"target"	=>	\$Opt_Output_Folder,
		"out-format"	=>	\$Opt_Output_Format,
	);
	parseParameters(\%Opt_Specific);

	# Output handling
	$Config{'Verbosity'} = $Opt_Generic_Verbose if ($Opt_Generic_Verbose);
	$Config{'Verbosity'} = -1 if ($Opt_Generic_Quiet);
	$Config{'Verbosity'} = -2 if ($Opt_Generic_ReallyQuiet);
	
	# Directory handling
	my $directory_input = $Opt_Input_Folder;
	my $directory_output = $Opt_Output_Folder;

	# Other values
	$Layout{'Border'} = $Opt_Process_Border if ($Opt_Process_Border);
	$Layout{'Border'} = 0 if ($Opt_Process_NoBorder); 
	$Layout{'Scale'} = $Opt_Process_Scale if ($Opt_Process_Scale);
	$Layout{'Rotate'} = $Opt_Process_Rotate if ($Opt_Process_Rotate);
	$Layout{'Output_format'} = $Opt_Output_Format if ($Opt_Output_Format);
	$Config{'DeleteFiles'} = $Opt_Input_Delete;
	
	
	
	#
	# Validation
	#

	# Other debug statements
	&log(1, "Will delete source files") if ($Config{'DeleteFiles'});

	# Check required arguments
	&log(-2, "I need at least a source and target directory (try --help for more information).") unless (($directory_input)&&($directory_output));

	# Validate target directory
	$directory_output =~ s/\/$//;	# Remove ending "/"
	if (!-d $directory_output)
	{
		&log(-2, "Target directory \"$directory_output\" is unexistant or not accisable.");
		exit;
	}

	# Validate source directory
	$directory_input =~ s/\/$//;	# Remove ending "/"
	if (-d $directory_input)
	{
		&log(0, "Scanning and converting all files in \"$directory_input\" recursively");
	} else {
		&log(-2, "Source directory \"$directory_input\" is unexistant or not accisable.");
		exit;
	}
	
	
	
	#
	# Main
	#
	
	process($directory_input, $directory_output);

	# Bye-bye
	&log(0, "Exiting");
	exit;
}















##########################
#                        #
#                        #
#        Routines        #
#                        #
#                        #
##########################






#########
###GUI###
#########

sub GUI_Toolbar
{
	my $toolbar = Gtk2::Toolbar->new ();

	$toolbar->insert_stock ("gtk-go-back",
							'Back',
							'tt private text',
							\&GUI_Dummy,
							'user_data',
							0);

	$toolbar->insert_stock ("gtk-go-forward",
							'Forward',
							'tt private text',
							\&GUI_Dummy,
							'user_data',
							1);

	$toolbar->insert_stock ("gtk-go-up",
							'Up',
							'tt private text',
							\&GUI_Dummy,
							'user_data',
							2);

	$toolbar->insert_space (3);
	
	$toolbar->insert_stock ("gtk-convert",
							'Convert',
							'tt private text',
							\&GUI_Dummy,
							'user_data',
							4);
							
	$toolbar->insert_stock ("gtk-clear",
							'Clear',
							'tt private text',
							\&GUI_Dummy,
							'user_data',
							5);
							
	$toolbar->insert_space (6);

	$toolbar->insert_stock ("gtk-properties",
							'Properties',
							'tt private text',
							\&GUI_Properties,
							'user_data',
							7);
							
	$toolbar->insert_space (8);
	
	$toolbar->insert_stock ("gtk-quit",
							'Quit',
							'tt private text',
							\&GUI_Toolbar_Quit,
							'user_data',
							9);

	return $toolbar;
}

sub GUI_Toolbar_Quit
{
	Gtk2->main_quit;
	exit;
}

sub GUI_Dummy
{
	@_ = shift;
	foreach my $i (@_)
	{
		print "Input: $i\n";
	}
}

sub GUI_Properties
{
	# Main dialog
	my $dialog = Gtk2::Dialog->new ('Properties', $window,
					'destroy-with-parent',
					'gtk-close' => 'close',
					);
	my $dialog_vbox = $dialog->vbox;
					
	# Dialog handlers
	$dialog->signal_connect (response => \&GUI_Properties_Button );
	$dialog->signal_connect(delete_event => \&GUI_Properties_Destroy);
	
	# Page 1 - Input modifiers
	my $page1 = Gtk2::VBox->new;
		my $page1_frame1 = Gtk2::Frame->new("Deletion option");
		$page1_frame1->set_border_width(3);
		$page1->add($page1_frame1);
			my $page1_frame1_vbox = Gtk2::VBox->new(FALSE, 6);
			$page1_frame1->add($page1_frame1_vbox);
				my $checkbox_deleteTOP = Gtk2::CheckButton->new ("_Delete converted files");
				$page1_frame1_vbox->add($checkbox_deleteTOP);
				$checkbox_deleteTOP->signal_connect (clicked => sub {
					if ($checkbox_deleteTOP->get_active)
					{
						$Config{'DeleteFiles'} = 1;
					} else {
						$Config{'DeleteFiles'} = 0;
					} });
				my $checkbox_deleteFolder = Gtk2::CheckButton->new ("_Delete empty folders");
				$page1_frame1_vbox->add($checkbox_deleteFolder);
				$checkbox_deleteFolder->signal_connect (clicked => sub {
					if ($checkbox_deleteFolder->get_active)
					{
						$Config{'DeleteFolders'} = 1;
					} else {
						$Config{'DeleteFolders'} = 0;
					} });
	$dialog_vbox->add($page1);
	
	# Show dialog
	$dialog->show_all();
}

sub GUI_Properties_Button
{
	my ($dialog, $response_id) = @_;
	
	if ($response_id eq "close")
	{
		&GUI_Properties_Destroy;
	}
}

sub GUI_Properties_Destroy
{
	my $dialog = shift;
	$dialog->destroy();
}

sub GUI_Layout()
{
	my $hpane = Gtk2::HPaned->new ();
	
	#$hpane->add1 (&create_mount_pane ());
	$hpane->add2 (Gtk2::Viewport->new ());

	return $hpane;
}








###########
###LOGIC###
###########


#
# General routines
#

# Scan directory recursively
##TODO: depend launched read and write routines on found file (hash: extention -> conversion routine)
sub process
{
	# Input values
	my $directory_input = shift;
	my $directory_output = shift;
	&log(2, "Scanning directory \"$directory_input\"");

	# Open the directory
	local *DIR;
	opendir(DIR, $directory_input) or die "cannot open directory ($!)";
	
	# List all files in the directory
	while (defined(my $file = readdir(DIR)))
	{
		# We got a directory
		if (-d "$directory_input/$file")
		{
			&log(3, "Got directory $file");
			
			# Skip unwanted directories (. and ..)
			next if ($file =~ m/^\.{1,2}$/);
			
			# Recursive call
			process("$directory_input/$file");
		}
		
		# We got a file
		elsif (-e "$directory_input/$file")
		{
			&log(3, "Got file $file");
			
			# We need a .top file!
			next unless ($file =~ m/^(.+)\.top$/i);
			
			# Start the conversion to the requested file format
			convert(	"$directory_input/$file",
					"TOP",

					"$directory_output",
					$Layout{'Output_format'}
				);
			
			# Delete the original file
			unlink "$directory_input/$file" if ($Config{'DeleteFiles'});
		}
	}
	closedir(DIR);
	
	&log(3, "Finished processing $directory_input");
}

# Compress given file
sub compress
{	
	# Input values
	my $file_uncompressed = shift;
	my $file_compressed = shift;
	
	&log(2, "Compressing \"$file_uncompressed\" to \"$file_compressed\"");
	
	# Compress file
	&log(3, "Compressing \"$file_uncompressed\"");
	system($bin_compress, "-f9", $file_uncompressed);
	
	# Rename file
	&log(3, "Renaming \"$file_uncompressed\" to \"$file_compressed\"");
	rename("$file_uncompressed.gz", $file_compressed);
	
	return 1;
}


#
# Conversion wrappers
#

##TODO: link possible preprocessing to variables (autocrop, rotate, translate, smoothn)
# Convert given TOP file to another format
sub convert
{
	#
	# Initialize
	#
	
	# Input values
	my $fileIn = shift;
	my $typeIn = shift;
	
	# Output values
	my $fileOut = shift;
	my $typeOut = shift;
	
	# Input
	my $data_points_ref;
	if ($typeIn =~ m/top/i) { $data_points_ref = readTop($fileIn) }
	else { die("Unsupported input file format: \"$typeIn\"") }
	
	# Pre-processing
	$data_points_ref = processMultipath($data_points_ref);
	$data_points_ref = processScale($data_points_ref, $Layout{'Scale'});
	#$data_points_ref = processRotate($data_points_ref, -90);
	$data_points_ref = processRelocate($data_points_ref);
	
	# Output
	if ($typeOut =~ m/^svg$/i) { writeSvg($data_points_ref, $fileOut) }
	elsif ($typeOut =~ m/^svgz$/i) { writeSvgz($data_points_ref, $fileOut) }
	elsif ($typeOut =~ m/^(png|jpg|jpeg|gif)$/i) {writeGD($data_points_ref, $typeOut, $fileOut) }
	else { die("Unsupported output file format: \"$typeOut\"") }
	
	return 1;
}


#
# System routines
#

# Log routine, enhanced print routine with priority options
sub log
{
	# Input values
	my $log_level = shift;
	my $log_msg = shift||return;	# Empty log message
	chomp $log_msg;
	
	# Prefix table
	my @prefix = ("!", "!", "!", "*" , "\t-", "\t\t-", "\t\t~");
	
	# Print the message; if we want to see it
	if ($log_level <= $Config{'Verbosity'})		# The maximum level we want to see
	{
		print STDOUT $prefix[$log_level + 3], " ", $log_msg, "\n" if ($log_level >= 0);
		print STDERR $prefix[$log_level + 3], " ", $log_msg, "\n" if ($log_level < 0);
	}
	
	# Exit on critical error
	exit if ($log_level <= -2);
	
	return;
}

# Command-line parameters parsing routine
sub parseParameters
{
	# Input values
	my $Map_ref = shift;
	my %Map = %$Map_ref;
	my $noWarn = shift;
	
	# Loop values
	for my $i (0 ... $#ARGV)
	{
		if ($ARGV[$i] =~ m/^--([^=]+)(=(.+)$|$)/)
		{
			my $Parameter = $1;
			my $Option = $3||1;
			
			my $Key_ref = $Map{$Parameter};
			
			if ($Key_ref)
			{
				${ $Key_ref } = $Option;
			} else {
				&log(-2, "Unknown command-line parameter: $Parameter.") unless ($noWarn);		
			}
		}	
	}
}

__END__

