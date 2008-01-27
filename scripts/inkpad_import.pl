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
#  * Sixth bit of the item is the stroke ID, pack that in the SVG with group tags (<g> & </g>)?
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

# Needed modules
use strict;
use warnings;

# Counter variables
my $directory_subfolderCount = 0;


#
# User-configurable variables
#

# Layout properties
my %Layout = (
	'Colour_foreground'	 =>	'black',
	'Colour_background'	 =>	'white',
	'Thickness'		 =>	10,		# Should be larger then 3px
	'Width'			 =>	600,
	'Height'		 =>	900
);

# Binaries
my $bin_compress = "gzip";

# Directory setup
my $directory_source = '/media/disk';	# This is an override value, if you don't use the udev rule
my $directory_target = '/home/tim/Afbeeldingen/Tekeningen';



##########
###MAIN###
##########

print "* Initializing\n";

# Find out mount point
open(MOUNTS, "/bin/mount |");
while (<MOUNTS>)
{
	next unless (m/PenPadStorage on ([^ ]+)/);	
	$directory_source = $1;
	print "- Detected mounted Pen Pad storage on \"$1\"\n";
}
close MOUNTS;

# Generate a subfolder tag
my @months = qw (januari februari maart april mei juni juli augustus september oktober november december);
my @timeData = localtime(time);
my $date_year = $timeData[5] + 1900;
my $date_day = $timeData[3];
my $date_month = $months[$timeData[4]];
my $directory_subfolder = "$date_day $date_month $date_year";

# Process directory if valid
$directory_source =~ s/\/$//;	# Remove ending "/"
if (-e "$directory_source/mynote.cfg")
{
	process($directory_source);
} else {
	print "! Cannot use \"$directory_source\"\n";
	exit;
}

# Bye-bye
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
	print "* Processing \"$directory\"\n";
	
	# Directory handling, did we already create a new folder?
	my $directory_newfolder = 0;
	my $directory_nonTop = 0;
	
	# Open the directory
	opendir(*DIR, $directory) or die "cannot open directory ($!)";
	
	# List all files in the directory
	while (defined(my $file = readdir(*DIR)))
	{
		# We got a directory
		if (-d "$directory/$file")
		{
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
			
			# Start the conversion
			top2svgz("$directory/$file", "$directory_target/$directory_subfolder - $directory_subfolderCount/$1.svgz");
			
			# Delete the original file
			unlink "$directory/$file";
		}
	}
	closedir(*DIR);
	
	# Clean up the processed directory (only if we are allowed to!)
	rmdir $directory if ($directory_nonTop == 0);
}

#
# Top to SVG routines
#

sub top2svg
{
	# Input values
	my $file_top = shift;
	my $file_svg = shift;
	print "\t- Converting \"$file_top\"\n";
	
	# Configure
	my $xmax = 8800;
	my $ymax = 12000;
	my $line = $Layout{'Thickness'} . "px";
	my $data_buffer = "";

	# Error check
	return (0, "no input file given") unless ($file_top);
	return (0, "could not parse filename") unless ($file_svg);
	return (0, "target file already exists") if (-e $file_svg);
	
	# Open the files
	open (TOP, $file_top) or return (0, "could not open input file ($!)");
	binmode(TOP);
	open (SVG, ">:utf8", $file_svg) or return (0, "could not open output file ($!)");

	# Prine SVG header
	print SVG << "END"
<?xml version="1.0" encoding="utf-8"?> 
<svg xmlns = "http://www.w3.org/2000/svg"
	xmlns:xlink = "http://www.w3.org/1999/xlink"
	xmlns:ev = "http://www.w3.org/2001/xml-events"
	version = "1.1" baseProfile = "full"
	width = "$Layout{'Width'}" height = "$Layout{'Height'}" viewBox = "0 0 $xmax $ymax">
	<rect x="0" y="0" width="$xmax" height="$ymax" fill="$Layout{'Colour_background'}" stroke="$Layout{'Colour_background'}" stroke-width="1px"/>
END
;

	# Check header integrity
	read(TOP, $data_buffer, 6);
	if ($data_buffer ne "WALTOP")
		{ return (0, "unknown file format"); }
	
	# Process header
	read(TOP, $data_buffer, 26);
	read(TOP, $data_buffer, 6) or return (0, "file is empty");
	my @data_begin = (unpack("C*",$data_buffer));
	my $y1 = $ymax - ($data_begin[1] + $data_begin[2] * 256);
	my $x1 = $data_begin[3] + $data_begin[4] * 256;
	
	# Process actual data	(Item format: 0/135 - Y coörd - 256*Y coörd - X coörd - 256*X coörd - Stroke item)
	while (read(TOP, $data_buffer, 6))
	{
		my @data_end = (unpack("C*",$data_buffer));
		my $y2 = $ymax - ($data_end[1] + $data_end[2] * 256);
		my $x2 = $data_end[3] + $data_end[4] * 256;
		print SVG qq(\t<line x1 = "$x1" y1 = "$y1" x2 = "$x2" y2 = "$y2" fill = "none" stroke = "$Layout{'Colour_foreground'}" stroke-width = "$line"/>\n);
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
	
	# Close the files
	close (TOP);
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
	
	# Compress file
	system($bin_compress, "-f9", $file_uncompressed);
	
	# Rename file
	rename("$file_uncompressed.gz", $file_compressed);
	
	return 1;
}

__END__

