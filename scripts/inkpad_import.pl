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
# Data format descriptions
# ~~~~~~~~~~~~~~~~~~~~~~~~
#  @data_points
#    Array of arrays, in which each array contains the following fields
#     - Type of stroke (_Line)
#     - Starting coördinate X
#     - Starting coördinate Y
#   [ - Point coördinate X ]
#   [ - Point coördinate Y ] x n
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

# Counter variables
my $directory_subfolderCount = 0;

# LINKERHOEK = (0, 1000)
# RECHTERHOEK = (8500, 12000)
# Layout properties
my %Layout = (
	'Colour_foreground'	=>	'black',
	'Colour_background'	=>	'white',
	'Thickness'		=>	10,		# Should be larger then 3px
	'Width'			=>	8500,
	'Height'		=>	12000,		# Width and height are real dimensions...
	'OffsetX'		=>	0,		# ...they get corrected throughout the script by the offset values
	'OffsetY'		=>	1000,
	'Scale'			=>	0.1,
	'Output_format'		=>	"svg",
);

# Calculate frame size
$Layout{'HeightView'} = ( $Layout{'Height'} - $Layout{'OffsetY'} ) / 10;
$Layout{'WidthView'} = ( $Layout{'Width'} - $Layout{'OffsetX'} ) / 10;

# Binaries
my $bin_compress = "gzip";


#
# Command-line parameters
#

# Read parameters
my ($Opt_Source, $Opt_Target, $Opt_Landscape, $Opt_Scale, $Opt_formatOut, $Opt_NoDelete, $Opt_Verbose, $Opt_Quiet, $Opt_ReallyQuiet, $Opt_Help);
my $Opt_Result = GetOptions(
	"source=s"	=>	\$Opt_Source,
	"target=s"	=>	\$Opt_Target,
	"landscape"	=>	\$Opt_Landscape,
	"scale=i"	=>	\$Opt_Scale,
	"out-format=s"	=>	\$Opt_formatOut,
	"no-delete"	=>	\$Opt_NoDelete,
	"verbosity=i"	=>	\$Opt_Verbose,
	"quiet"		=>	\$Opt_Quiet,
	"really-quiet"	=>	\$Opt_ReallyQuiet,
	"help"		=>	\$Opt_Help,
);

# Output handling
my $output_level = 0;
$output_level = $Opt_Verbose if ($Opt_Verbose);
$output_level = -1 if ($Opt_Quiet);
$output_level = -2 if ($Opt_ReallyQuiet);

# Welcome message
&log(0, "Initializing");

# Display help, and exit
if ($Opt_Help)
{
	print <<END
Usage: inkpad_import.pl [OPTIONS]
Import proprietary .TOP files from a Medion MD 85276 Digital Ink Pad,
convert them to an Gzip compressed SVG/XML format, and save them
on a local source

Additional parameters:
  --source=PATH     Source directory for the .TOP files.
                      Most likely this will the mount point
                      of your MD 85276.
                    If not specified, the script will try to
                      detect the mount point (will only work
                      if the UDEV rule has been activated),
                      or default to "/media/disk".
  --landscape       Generate SVG in landscape format
  --scale=PERCENT   Scale the image by a given percent (default 10)
  --target=PATH     Target directory for the .SVG(Z) files.
                      Subdirectories will be created based on
                      the current date and the subfolders relative
                      to the source directory.
  --out-format=EXT  Format of output file [SVG-SVGZ-PNG-JPG-GIF]
  --no-delete       Original files and folders will not be deleted
  --verbosity=LVL   Level of verbose output [1, 2, 3]
  --quiet           Be quiet (only display errors and warnings)
  --really-quiet    Be really quiet (only display errors)
  --help            Display this help

Copyright 2008, by Tim Besard (tim.besard\@gmail.com)
END
;
	exit;
}

# Source directory handling
my $directory_source = '/media/disk';	# Default value
if ($Opt_Source)
{
	# We have an override value
	$directory_source = $Opt_Source;
	&log(1, "Source directory has been overrided to \"$directory_source\"");
}
else
{
	# We do not have an override value, check for UDEV rule
	open(MOUNTS, "/bin/mount |");
	while (<MOUNTS>)
	{
		next unless (m/PenPadStorage on ([^ ]+)/);
		$directory_source = $1;
		&log(1, "Source directory has been altered by UDEV rule to \"$directory_source\"");
	}
	close MOUNTS;
}
&log(1, "Using source directory: \"$directory_source\"");

# Target directory handling
my $directory_target = '/home/tim/Afbeeldingen/Tekeningen';
if ($Opt_Target)
{
	# We have an override value
	$directory_target = $Opt_Target;
	&log(1, "Target directory has been overrided to \"$directory_target\"");
}
&log(1, "Using target directory: \"$directory_target\"");

# Other values
$Layout{'Scale'} = ($Opt_Scale / 100) if ($Opt_Scale);
$Layout{'Output_format'} = $Opt_formatOut if ($Opt_formatOut);

# Other debug statements
&log(1, "Will not delete source files") if ($Opt_NoDelete);


##########
###MAIN###
##########

# Generate a subfolder tag
my @months = qw (januari februari maart april mei juni juli augustus september oktober november december);
my @timeData = localtime(time);
my $date_year = $timeData[5] + 1900;
my $date_day = $timeData[3];
my $date_month = $months[$timeData[4]];
my $directory_subfolder = "$date_day $date_month $date_year";

# Validate target directory
$directory_target =~ s/\/$//;	# Remove ending "/"
if (!-d $directory_target)
{
	&log(-2, "Target directory \"$directory_target\" is unexistant or not accisable");
	exit;
}

# Validate source directory
$directory_source =~ s/\/$//;	# Remove ending "/"
if (-d $directory_source)
{
	&log(0, "Scanning and converting all files in \"$directory_source\" recursively");
	process($directory_source);
} else {
	&log(-2, "Source directory \"$directory_source\" is unexistant or not accisable");
	exit;
}

# Bye-bye
&log(0, "Exiting");
exit;



############
##ROUTINES##
############


#
# General routines
#

# Scan directory recursively
##TODO: depend launched read and write routines on found file (hash: extention -> conversion routine)
sub process
{
	# Input values
	my $directory = shift;
	&log(2, "Scanning directory \"$directory\"");
	
	# Directory handling, did we already create a new folder?
	my $directory_newfolder = 0;
	my $directory_nonTop = 0;

	# Open the directory
	local *DIR;
	opendir(DIR, $directory) or die "cannot open directory ($!)";
	
	# List all files in the directory
	while (defined(my $file = readdir(DIR)))
	{
		# We got a directory
		if (-d "$directory/$file")
		{
			&log(3, "Got directory $file");
			
			# Skip unwanted directories (. and ..)
			next if ($file =~ m/^\.{1,2}$/);
			
			# Recursive call
			process("$directory/$file");
			
			# Our recursive process did not delete the folder, which means
			#  that there is a non .top file in there, don't delete this folder!
			$directory_nonTop++ if (-d "$directory/$file");
		}
		
		# We got a file
		elsif (-e "$directory/$file")
		{
			&log(3, "Got file $file");
			
			# We need a .top file!
			$directory_nonTop++ unless ($file =~ m/^(.+)\.top$/i);	# We got a non .top file, don't delete this folder!
			next unless ($file =~ m/^(.+)\.top$/i);
			
			# Directory handling, first time we got a .top file, let's make a new subfolder!
			if ($directory_newfolder == 0)
			{
				$directory_subfolderCount++;
				while (-d "$directory_target/$directory_subfolder - $directory_subfolderCount")
					{ $directory_subfolderCount++; }	# In case of multiple syncs at the same day
				mkdir "$directory_target/$directory_subfolder - $directory_subfolderCount";
				$directory_newfolder = 1;
			}
			
			# Start the conversion to the requested file format
			convert(	"$directory/$file",
					"TOP",

					"$directory_target/$directory_subfolder - $directory_subfolderCount/$1.$Layout{'Output_format'}",
					$Layout{'Output_format'}
				);
			
			# Delete the original file
			unlink "$directory/$file" unless ($Opt_NoDelete);
		}
	}
	closedir(DIR);
	
	&log(3, "Finished processing $directory");
	
	# Clean up the processed directory (only if we are allowed to!)
	rmdir $directory if (	  ($directory_nonTop == 0)			# If we have no non-.TOP files
				&&($Opt_NoDelete != 1)				# If the user hasn't said otherwise
				&&($directory ne $directory_source)	);	# If the directory isn't the top directory
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
	if ($log_level <= $output_level)	# $output_level is the maximum level we want to see
	{
		print $prefix[$log_level + 3], " ", $log_msg, "\n";
	}
	
	return;
}

__END__

