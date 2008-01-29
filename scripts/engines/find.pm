
# Title
# ~~~~~
#  Medion Digital Ink Pad Convertion and Synchronisation - Data finding engines
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

# Find extrema's in data points
sub findExtremas
{
	# Input values
	my $data_points_ref = shift;
	
	my @data_points = @$data_points_ref;
	
	# Output values
	my ($x_min, $y_min, $x_max, $y_max);
	
	# Find extrema's
	foreach my $data_stroke (@data_points)
	{
		# Duplicate the array (through references, arrays get modified, what we don't want in here);
		my @data_stroke_dup = @{$data_stroke};
		
		# Get the type of path
		my $type = shift(@data_stroke_dup);
		
		# Line calculus
		if ($type eq "L")
		{
			# Loop all points
			while(((my $xn = shift(@data_stroke_dup))&&(my $yn = shift(@data_stroke_dup))))
			{
				$x_min = $xn if ((!defined $x_min)||($xn < $x_min));
				$y_min = $yn if ((!defined $y_min)||($yn < $y_min));
				$x_max = $xn if ((!defined $x_max)||($xn > $x_max));
				$y_max = $yn if ((!defined $y_max)||($yn > $y_max));
			}
		}
	}
	
	return ($x_min, $y_min, $x_max, $y_max);
}






1;
