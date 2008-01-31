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
# Based upon top2svg v0.2, copyright (c) 2007 AngieSoft
# All rights reserved.
#
#  Redistribution and use in source and binary forms, with or
#  without modification, are permitted provided that the following
#  conditions are met:
#
#  1. Redistributions of source code must retain the above
#     copyright notice, this list of conditions and the following
#     disclaimer.
#
#  2. Redistributions in binary form must reproduce the above
#     copyright notice, this list of conditions and the following
#     disclaimer in the documentation and/or other materials
#     provided with the distribution.
#
#  3. Neither the name of the author nor the names of its
#     contributors may be used to endorse or promote products
#     derived from this software without specific prior written
#     permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
# CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
# EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.



##########
##CONFIG##
##########

#
# Initialisation
#

# Needed modules
use strict;
use warnings;
use Getopt::Long;
use GD;
use GD::Polyline;

# Load engines
require 'engines/input.pm';
require 'engines/output.pm';
require 'engines/process.pm';
require 'engines/find.pm';


#
# Shared variables
#

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
	'Delete'		=>	0,
	'Verbosity'		=>	0,
);

# Binaries
our $bin_compress = "gzip";











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
my $Mode_Results = GetOptions(
	"cli"		=>	\$Mode_Cli,
	"gui"		=>	\$Mode_Gui,
	"help"		=>	\$Mode_Help,
	
	"verbosity=i"	=>	\$Opt_Generic_Verbose,
	"quiet"		=>	\$Opt_Generic_Quiet,
	"really-quiet"	=>	\$Opt_Generic_ReallyQuiet,
);

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

if (! $Mode_Gui)
{
	#
	# Command-line parameters
	#
	
	# Welcome message
	&log(0, "Initializing user interface");







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
	my $Opt_Result = GetOptions(
		"source=s"	=>	\$Opt_Input_Folder,
		"delete"	=>	\$Opt_Input_Delete,
	
		"rotate=i"	=>	\$Opt_Process_Rotate,
		"scale=i"	=>	\$Opt_Process_Scale,
		"border=i"	=>	\$Opt_Process_Border,
		"no-border"	=>	\$Opt_Process_NoBorder,
	
		"target=s"	=>	\$Opt_Output_Folder,
		"out-format=s"	=>	\$Opt_Output_Format,
	);

	# Output handling
	$Config{'Verbosity'} = $Opt_Generic_Verbose if ($Opt_Generic_Verbose);
	$Config{'Verbosity'} = -1 if ($Opt_Generic_Quiet);
	$Config{'Verbosity'} = -2 if ($Opt_Generic_ReallyQuiet);
	
	# Directory handling
	my $direcory_input = $Opt_Input_Folder;
	my $direcory_output = $Opt_Output_Folder;

	# Other values
	$Layout{'Border'} = $Opt_Process_Border if ($Opt_Process_Border);
	$Layout{'Border'} = 0 if ($Opt_Process_NoBorder); 
	$Layout{'Scale'} = $Opt_Process_Scale if ($Opt_Process_Scale);
	$Layout{'Rotate'} = $Opt_Process_Rotate if ($Opt_Process_Rotate);
	$Layout{'Output_format'} = $Opt_Output_Format if ($Opt_Output_Format);
	$Config{'Delete'} = $Opt_Input_Delete;
	
	
	
	#
	# Validation
	#

	# Other debug statements
	&log(1, "Will delete source files") if ($Config{'Delete'});

	# Check required arguments
	&log(-2, "I need at least a source and target directory (try --help for more information).") unless (($direcory_input)&&($direcory_output));

	# Validate target directory
	$direcory_output =~ s/\/$//;	# Remove ending "/"
	if (!-d $direcory_output)
	{
		&log(-2, "Target directory \"$direcory_output\" is unexistant or not accisable.");
		exit;
	}

	# Validate source directory
	$direcory_input =~ s/\/$//;	# Remove ending "/"
	if (-d $direcory_input)
	{
		&log(0, "Scanning and converting all files in \"$direcory_input\" recursively");
	} else {
		&log(-2, "Source directory \"$direcory_input\" is unexistant or not accisable.");
		exit;
	}
	
	
	
	#
	# Main
	#
	
	process($direcory_input,$direcory_output);

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
			unlink "$directory_input/$file" if ($Config{'Delete'});
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
	my $log_msg = shift;
	
	# Prefix table
	my @prefix = ("!", "!", "!", "*" , "\t-", "\t\t-", "\t\t~");
	
	# Print the message; if we want to see it
	if ($log_level <= $Config{'Verbosity'})		# The maximum level we want to see
	{
		print $prefix[$log_level + 3], " ", $log_msg, "\n";
	}
	
	# Exit on critical error
	exit if ($log_level <= -2);
	
	return;
}

__END__

