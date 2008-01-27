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
#  * Three values need user modification:
#    [ $directory_source = mount point of the Ink Pad (not needed if you use the UDEV rule) ]
#      $directory_target = Target directory for the images
#      @months = localisation for the folder naming
#


# Copyright (c) 2008 MALEADt <maleadt@gmail.com>
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

# Counter variables
my $directory_subfolderCount = 0;

##TODO: Bereik (width height), effectief full A4?
# Layout properties
my %Layout = (
	'Colour_foreground'	 =>	'black',
	'Colour_background'	 =>	'white',
	'Thickness'		 =>	10,		# Should be larger then 3px
	'Width'			 =>	"210mm",
	'Height'		 =>	"279.4mm"
);

# Binaries
my $bin_compress = "gzip";


#
# Command-line parameters
#

# Read parameters
my ($Opt_Source, $Opt_Target, $Opt_NoCompress, $Opt_NoDelete, $Opt_Verbose, $Opt_Quiet, $Opt_ReallyQuiet, $Opt_Help);
my $Opt_Result = GetOptions(
	"source=s"	=>	\$Opt_Source,
	"target=s"	=>	\$Opt_Target,
	"no-compress"	=>	\$Opt_NoCompress,
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
  --target=PATH     Target directory for the .SVG(Z) files.
                      Subdirectories will be created based on
                      the current date and the subfolders relative
                      to the source directory.
  --no-compress     No compression will be used, resulting files
                      will be raw SVG/XML.
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

# Other debug statements
&log(1, "Compression has been disabled") if ($Opt_NoCompress);
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
			if ($Opt_NoCompress)
			{
				top2svg("$directory/$file", "$directory_target/$directory_subfolder - $directory_subfolderCount/$1.svg");
			} else {
				top2svgz("$directory/$file", "$directory_target/$directory_subfolder - $directory_subfolderCount/$1.svgz");
			}
			
			# Delete the original file
			unlink "$directory/$file" unless ($Opt_NoDelete);
		}
	}
	closedir(DIR);
	
	&log(3, "Finished processing $directory");
	
	# Clean up the processed directory (only if we are allowed to!)
	rmdir $directory if (($directory_nonTop == 0)&&($Opt_NoDelete != 1));
}

#
# Top to SVG routines
#

sub top2svg
{
	#
	# Initialize
	#
	
	# Input values
	my $file_top = shift;
	my $file_svg = shift;
	
	&log(2, "Converting \"$file_top\" to \"$file_svg\"");
	
	# Configure
	##TODO: bereik van inkpad, effectief 8800 x 12000?
	my $xmax = 8800;
	my $ymax = 12000;
	my $line = $Layout{'Thickness'} . "px";
	my $data_buffer = "";

	# Error check
	return &log(-1, "No input file given") unless ($file_top);
	return &log(-1, "No output file given") unless ($file_svg);
	return &log(-1, "Output file \"$file_svg\" already exists") if (-e $file_svg);

	
	
	#
	# Read data
	#
	
	# Open the files
	&log(3, "Opening input stream");
	open (TOP, $file_top) or return &log(-1, "Could not open input file \"$file_top\" ($!)");
	binmode(TOP);

	# Check header integrity
	&log(3, "Verifying header");
	read(TOP, $data_buffer, 6);
	if ($data_buffer ne "WALTOP")
		{ &log(-1, "Damaged header: \"$file_top\"") }
	
	# Process header
	&log(3, "Processing header");
	read(TOP, $data_buffer, 26);
	read(TOP, $data_buffer, 6) or &log(-1, "File is empty: \"$file_top\"");
	my @data_begin = (unpack("C*",$data_buffer));
	my $y1 = $ymax - ($data_begin[1] + $data_begin[2] * 256);
	my $x1 = $data_begin[3] + $data_begin[4] * 256;
	
	# Array of arrays, containing all point data
	##TODO: check memory usage on large drawings
	my @data_points;
	
	# Process actual data	(Item format: 0/135 - Y coörd - 256*Y coörd - X coörd - 256*X coörd - Stroke item)
	&log(3, "Reading and converting data points");
	while (read(TOP, $data_buffer, 6))
	{
		my @data_end = (unpack("C*",$data_buffer));
		my $y2 = $ymax - ($data_end[1] + $data_end[2] * 256);
		my $x2 = $data_end[3] + $data_end[4] * 256;
		push @data_points, [$x1, $y1, $x2, $y2];	# Save the data points
		if ($data_end[0] == 0)	# First bit was a zero, which means we are at the end of the file
		{
			# Note to self: WHAT is the meaning of this piece of code? When $data_end[0] is zero, we are at the last stroke, so we can't read 6 bytes out anymore... Maybe a 0 index does occur at the beginning of the file?
			read(TOP, $data_buffer, 6);
			next unless $data_buffer;	# Empty buffer, skip this one
			@data_begin = (unpack("C*",$data_buffer));
			$y1 = $ymax - ($data_begin[1] + $data_begin[2] * 256);
			$x1 = $data_begin[3] + $data_begin[4] * 256;
		} else {
			@data_begin = @data_end;
			$y1 = $ymax - ($data_begin[1] + $data_begin[2] * 256);
			$x1 = $data_begin[3] + $data_begin[4] * 256;
		}
	}
	
	# Close file
	&log(3, "Closing input streams");
	close (TOP);
	
	# Undef variables we'll need later on
	undef $x1, $y1;
	
	
	#
	# Write data
	#

	# Open file
	&log(3, "Opening output stream");
	open (SVG, ">:utf8", $file_svg) or &log(-1, "Could not open output file \"$file_top\" ($!)");

	# Print SVG header
	&log(3, "Writing SVG header");
	print SVG << "END"
<?xml version="1.0" encoding="utf-8"?> 
<svg xmlns="http://www.w3.org/2000/svg"
	xmlns:xlink="http://www.w3.org/1999/xlink"
	xmlns:ev="http://www.w3.org/2001/xml-events"
	version="1.1" baseProfile="full"
	width="$Layout{'Width'}" height="$Layout{'Height'}" viewBox="0 0 $xmax $ymax">
	<rect x="0" y="0" width="$xmax" height="$ymax" fill="$Layout{'Colour_background'}" stroke="$Layout{'Colour_background'}" stroke-width="1px"/>
END
;

	# Write data points in XML format
	my $debug_paths = 0;
	my $debug_strokes = 0;	
	my $path_data = qq(fill="none" stroke="$Layout{'Colour_foreground'}" stroke-width="$line");
	my ($x1, $y1, $x2, $y2) = (undef, undef, undef, undef);
	my ($x1_prev, $y1_prev, $x2_prev, $y2_prev) = (undef, undef, undef, undef);
	
	
	# Start the calculation
	foreach my $data_stroke (@data_points)
	{
		# Save current data
		($x1, $y1, $x2, $y2) = @{ $data_stroke };
		
		# Only calculate path if we got previous data
		if ((defined $x1_prev)&&(defined $y1_prev)&&(defined $x2_prev)&&(defined $y2_prev))
		{
			# Endpoints of previous stroke matches with beginpoints of current stroke...
			if (($x2_prev == $x1)&&($y2_prev == $y1))
			{
				# ...so continue the stroke
				print SVG qq($x2,$y2 );
				
				# and log it
				$debug_strokes++;
			}
				# No match...
			else
			{
				# ...so end the path
				print SVG qq($x2_prev,$y2_prev"/>\n);
				
				# and start a new one
				print SVG qq(\t<path $path_data d="M$x1,$y1 L$x2,$y2 );
				
				# and log it
				$debug_strokes++;
				$debug_paths++;
				}
		}
		
		# We got no previous data, start a new path
		else
		{
			print SVG qq(\t<path $path_data d="M$x1,$y1 L$x2,$y2 );
			
			# and log it
			$debug_paths++;
		}
		
		# Shift data
		($x1_prev, $y1_prev, $x2_prev, $y2_prev) = ($x1, $y1, $x2, $y2);
	}
	
	# End the last stroke
	print SVG qq($x2_prev,$y2_prev"/>\n);
		
	# Verbose log
	&log(3, "Got $debug_paths paths, all together $debug_strokes strokes");

	# Close the file
	&log(3, "Closing output stream");
	print SVG " </svg>\n";
	close (SVG);
	
	return 1;
}

sub top2svgz
{
	# Input values
	my $file_top = shift;
	my $file_svgz = shift;
	
	# Temporary SVG file
	my $file_svg;
	if ($file_svgz =~ m/^(.+)\.svgz/)
		{ $file_svg = "$1.svg"; }
	elsif ($file_svgz =~ m/^(.+)\.top/)	# SVGZ file doesn't end on SVGZ, let's check the .top file
		{ $file_svg = "$1.svg"; }
	else					# TOP file doesn't end on TOP (in fact an impossible situation),
		{ $file_svg = "$file_top.svg" }	# let's name it .top.gz
	
	# Process file
	top2svg($file_top, $file_svg);
	
	# Compress file
	compress($file_svg, $file_svgz);
	
	return 1;
}

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

