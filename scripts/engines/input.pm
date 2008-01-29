
# Title
# ~~~~~
#  Medion Digital Ink Pad Convertion and Synchronisation - Data input engines
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

# Read data points from TOP format
sub readTop
{
	#
	# Initialize
	#
	
	# Input values
	my $file_top = shift;
	
	# Read data
	my $data_buffer = "";
	my @data_points;

	# Error check
	return &log(-1, "No input file given") unless ($file_top);

	
	
	#
	# Read data
	#
	
	&log(2, "Reading from \"$file_top\"");
	
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
	my $y1 = 12000 - ($data_begin[1] + $data_begin[2] * 256);
	my $x1 = $data_begin[3] + $data_begin[4] * 256;
	
	# Process actual data	(Item format: 0/135 - Y coörd - 256*Y coörd - X coörd - 256*X coörd - Stroke item)
	&log(3, "Reading and converting data points");
	while (read(TOP, $data_buffer, 6))
	{
		my @data_end = (unpack("C*",$data_buffer));
		my $y2 = 12000 - ($data_end[1] + $data_end[2] * 256);
		my $x2 = $data_end[3] + $data_end[4] * 256;;
		
		# Save data points
		push @data_points, [	"L",
					$x1,
					$y1,
					$x2,
					$y2
					];
		
		if ($data_end[0] == 0)	# First bit was a zero, which means we are at the end of the file
		{
			# Note to self: WHAT is the meaning of this piece of code? When $data_end[0] is zero, we are at the last stroke, so we can't read 6 bytes out anymore... Maybe a 0 index does occur at the beginning of the file?
			read(TOP, $data_buffer, 6);
			next unless $data_buffer;	# Empty buffer, skip this one
			@data_begin = (unpack("C*",$data_buffer));
			$y1 = 12000 - ($data_begin[1] + $data_begin[2] * 256);
			$x1 = $data_begin[3] + $data_begin[4] * 256;
		} else {
			@data_begin = @data_end;
			$y1 = 12000 - ($data_begin[1] + $data_begin[2] * 256);
			$x1 = $data_begin[3] + $data_begin[4] * 256;
		}
	}
	
	# Close file
	&log(3, "Closing input streams");
	close (TOP);
	
	return \@data_points;
}






1;
