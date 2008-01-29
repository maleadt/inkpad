
# Title
# ~~~~~
#  Medion Digital Ink Pad Convertion and Synchronisation - Data output engines
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

# Write data points to SVGZ
sub writeSvgz
{
	# Input values
	my $data_points_ref = shift;
	my $file = shift;
	
	# Write a SVG file
	writeSvg($data_points_ref, "$file.temp");
	
	# Compress file
	compress("$file.temp", $file);
	
	return 1;
}

# Write data points in SVG format
sub writeSvg
{
	#
	# Initialize
	#
	
	# Input values
	my $data_points_ref = shift;
	my @data_points = @$data_points_ref;
	my $file_svg = shift;
	
	# Error check
	return &log(-1, "Output file \"$file_svg\" already exists") if (-e $file_svg);

	
	#
	# Write data
	#
	
	&log(2, "Writing to \"$file_svg\"");

	# Open file
	&log(3, "Opening output stream");
	open (SVG, ">:utf8", $file_svg) or &log(-1, "Could not open output file \"$file_svg\" ($!)");

	# Print SVG header
	&log(3, "Writing SVG header");
	print SVG << "END"
<?xml version="1.0" encoding="utf-8"?> 
<svg xmlns="http://www.w3.org/2000/svg"
	xmlns:xlink="http://www.w3.org/1999/xlink"
	xmlns:ev="http://www.w3.org/2001/xml-events"
	version="1.1" baseProfile="full"
END
;

	# Find the extrema's
	my ($x_min, $y_min, $x_max, $y_max) = findExtremas($data_points_ref);
	my $width = ( $x_max - $x_min ) * $Layout{'Scale'};
	my $height = ( $y_max - $y_min ) * $Layout{'Scale'};
	
	# Normal mode
	print SVG qq(\twidth="$width" height="$height" viewBox="$x_min $y_min $x_max $y_max">\n);
	print SVG qq(\t<rect x="$x_min" y="$y_min" width="$x_max" height="$y_max" fill="$Layout{'Colour_background'}" stroke="$Layout{'Colour_background'}" stroke-width="1px"/>\n);

	# Write data points in XML format
	my $debug_paths = 0;
	my $debug_strokes = 0;	
	my $path_data = qq(fill="none" stroke="$Layout{'Colour_foreground'}" stroke-width="$Layout{'Thickness'}px");
	
	
	# Output given path data
	foreach my $data_stroke (@data_points)
	{
		# Get the type of path
		my $type = shift(@{$data_stroke});
		
		# A line stroke
		if ($type eq "L")
		{
			# Starting coördinates
			my $x1 = shift(@{$data_stroke});
			my $y1 = shift(@{$data_stroke});
			print SVG qq(\t<path $path_data d="M$x1,$y1 L);
		
			# Every following couple of coördinates is a path continuation
			while(((my $xn = shift(@{$data_stroke}))&&(my $yn = shift(@{$data_stroke}))))
			{
				print SVG qq($xn,$yn );
			}
			
			# End the path
			print SVG qq("/>\n);
		}
	}

	# Close the file
	&log(3, "Closing output stream");
	print SVG " </svg>\n";
	close (SVG);
	
	return 1;
}

# Write data points in GD supported format
sub writeGD
{
	#
	# Initialize
	#
	
	# Input values
	my $data_points_ref = shift;
	my $type = shift;
	my $file = shift;
	
	# Error check
	return &log(-1, "Output file \"$file\" already exists") if (-e $file);
	
	# Render and output the image
	my $image = imageRender($data_points_ref);
	imageWrite($image, $type, $file);
	
	return;
}

# Render a GD image based on the data points
##TODO: Problem with GDrenderer -> X offset?
sub imageRender
{
	# Input values
	my $data_points_ref = shift;
	my @data_points = @$data_points_ref;

	# Find the extrema's
	my ($x_min, $y_min, $x_max, $y_max) = findExtremas($data_points_ref);
	my $width = ( $x_max - $x_min ) * $Layout{'Scale'};
	my $height = ( $y_max - $y_min ) * $Layout{'Scale'};
	
	# Create a new image
	my $image = new GD::Image($width, $height) or die($!);	
	$image->setThickness($Layout{'Thickness'} * $Layout{'Scale'});

	# Allocate some colours
	my $colour_white = $image->colorAllocate(255,255,255);
	my $colour_black = $image->colorAllocate(  0,  0,  0);
	my $colour_red   = $image->colorAllocate(255,  0,  0);
	my $colour_blue  = $image->colorAllocate(  0,  0,255);
	
	# Build the image
	foreach my $data_stroke (@data_points)
	{
		# Get the type of path
		my $type = shift(@{$data_stroke});
		
		# A line stroke
		if ($type eq "L")
		{
			# The line
			my $polyline = new GD::Polyline;
		
			# Every following couple of coördinates is a path continuation
			while(((my $xn = shift(@{$data_stroke}))&&(my $yn = shift(@{$data_stroke}))))
			{
				$polyline->addPt($xn * $Layout{'Scale'}, $yn * $Layout{'Scale'});
			}
			
			# Add the line to the image
			$image->polydraw($polyline,$colour_black);
		}
	}
	
	return $image;
}

# Output the rendered file
sub imageWrite
{
	# Input values
	my $image = shift;
	my $type = shift;
	my $file = shift;
	
	# Detect extention if not given
	$file =~ /\.([^.]+)$/i;
	$type = $1 unless $type;
	
	# Open file for writing
	open(FILE, ">$file");
	binmode FILE;
	
	# Output the file
	print FILE $image->png if ($type =~ m/^png$/i);
	print FILE $image->gif if ($type =~ m/^gif$/i);
	print FILE $image->jpeg(100) if ($type =~ m/^(jpeg|jpg)$/i);
	
	return;
}






1;
