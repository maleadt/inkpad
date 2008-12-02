
# Title
# ~~~~~
#  Medion Digital Ink Pad Convertion and Synchronisation - Data processing engines
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

# Detect multipaths
sub processMultipath
{
	# Input values
	my $data_points_ref = shift;
	my @data_points = @$data_points_ref;
	
	# Output values
	my ($type, $x1, $y1, $x2, $y2, $x1_prev, $y1_prev, $x2_prev, $y2_prev);
	my @data_points_processed;
	
	# Start the calculation
	foreach my $data_stroke (@data_points)
	{
		# Save current data
		($type, $x1, $y1, $x2, $y2) = @{ $data_stroke };
		
		# Only calculate path if we got previous data
		if ((defined $x1_prev)&&(defined $y1_prev)&&(defined $x2_prev)&&(defined $y2_prev))
		{
			# Endpoints of previous stroke matches with beginpoints of current stroke...
			if (($x2_prev == $x1)&&($y2_prev == $y1))
			{
				# ...so continue the stroke
				push @{ $data_points_processed[-1] }, $x2;
				push @{ $data_points_processed[-1] }, $y2;
			}
			
			# No match...
			else
			{
				# ...so end the path
				push @{ $data_points_processed[-1] }, $x2_prev;
				push @{ $data_points_processed[-1] }, $y2_prev;
				
				# and start a new one
				push @data_points_processed, ["L", $x1, $y1, $x2, $y2];
			}
		}
		
		# We got no previous data, start a new path
		else
		{
			push @data_points_processed, ["L", $x1, $y1, $x2, $y2];
		}
		
		# Shift data
		($x1_prev, $y1_prev, $x2_prev, $y2_prev) = ($x1, $y1, $x2, $y2);
	}
	
	return \@data_points_processed
}

# Rotate data points by given angle
sub processRotate
{
	# Input values
	my $data_points_ref = shift;
	my @data_points = @$data_points_ref;
	my $angle = shift;
	
	# Degree to radian conversion
	my $pi = ( atan2(1,1) *4);
	my $angle_rad = ($angle/180)*$pi;
	
	# Output values
	my @data_points_processed;
	
	# Calculate new coördinates
	foreach my $data_stroke (@data_points)
	{
		# Temporary array
		my @data_stroke_processed;
		
		# Get the type of path
		push @data_stroke_processed, shift(@{$data_stroke});

		while(((my $xn = shift(@{$data_stroke}))&&(my $yn = shift(@{$data_stroke}))))
		{
			push @data_stroke_processed, ($xn * cos($angle_rad) - $yn * sin($angle_rad));
			push @data_stroke_processed, ($xn * sin($angle_rad) + $yn * cos($angle_rad));
		}
		
		# Save temporary array in final array
		push @data_points_processed, [ @data_stroke_processed ];
	}
	
	# Adjust layout settings
	my $x_min2 = ($Layout{'X_min'} * cos($angle_rad) - $Layout{'Y_min'} * sin($angle_rad));
	my $y_min2 = ($Layout{'X_min'} * sin($angle_rad) + $Layout{'Y_min'} * cos($angle_rad));
	my $x_max2 = ($Layout{'X_max'} * cos($angle_rad) - $Layout{'Y_max'} * sin($angle_rad));
	my $y_max2 = ($Layout{'X_max'} * sin($angle_rad) + $Layout{'Y_max'} * cos($angle_rad));
	if ($x_max2 < $x_min2)
		{	my $temp = $x_min2; $x_min2 = $x_max2; $x_max2 = $temp; }
	if ($y_max2 < $y_min2)
		{	my $temp = $y_min2; $y_min2 = $y_max2; $y_max2 = $temp; }
	$Layout{'X_min'} = $x_min2;
	$Layout{'Y_min'} = $y_min2;
	$Layout{'X_max'} = $x_max2;
	$Layout{'Y_max'} = $y_max2;
	
	return \@data_points_processed;
}

# Translate data points by given X and Y offset
sub processTranslate
{
	# Input values
	my $data_points_ref = shift;
	my @data_points = @$data_points_ref;
	my $x = shift;
	my $y = shift;
	
	# Output values
	my @data_points_processed;

	# Calculate new coördinates
	foreach my $data_stroke (@data_points)
	{
		# Temporary array
		my @data_stroke_processed;
		
		# Get the type of path
		push @data_stroke_processed, shift(@{$data_stroke});

		while(((my $xn = shift(@{$data_stroke}))&&(my $yn = shift(@{$data_stroke}))))
		{
			push @data_stroke_processed, ($xn + $x);
			push @data_stroke_processed, ($yn + $y);
		}
		
		# Save temporary array in final array
		push @data_points_processed, [ @data_stroke_processed ];
	}
	
	
	# Adjust layout settings - latitude keys
	foreach my $key (qw(X_min X_max))
	{
		$Layout{$key} = $Layout{$key} + $x;
	}
	
	# Adjust layout settings - altitude keys
	foreach my $key (qw(Y_min Y_max))
	{
		$Layout{$key} = $Layout{$key} + $y;
	}
	
	return \@data_points_processed;
}

# Relocate the coördinates to minize the offset from the (0,0) coördinate (with a 5% spacing)
sub processRelocate
{
	# Input values
	my $data_points_ref = shift;
	
	# Find extrema's
	my ($x_min, $y_min, $x_max, $y_max) = findExtremas($data_points_ref);
	
	# Calculate spacing
	my $spacing = $Layout{'Border'} / 100;
	my $spacing_x = int ($x_max - $x_min) * $spacing;
	my $spacing_y = int ($y_max - $y_min) * $spacing;
		
	# Remove the minimum extremas by translating the coördinates to the (0, 0) coördinate
	$data_points_ref = processTranslate($data_points_ref, (-$x_min) + $spacing_x, (-$y_min) + $spacing_y);
	
	# Reconfigure image viewbox
	$Layout{'X_min'} = 0;
	$Layout{'Y_min'} = 0;
	$Layout{'X_max'} = $x_max - $x_min + (2 * $spacing_x);
	$Layout{'Y_max'} = $y_max - $y_min + (2 * $spacing_y);
	
	return $data_points_ref;
}

# Scale every value which should be adjuster according to a given percent
sub processScale
{
	# Input values
	my $data_points_ref = shift;
	my @data_points = @$data_points_ref;
	my $scale = (shift) / 100;
	
	# Output values
	my @data_points_processed;

	# Calculate new coördinates
	foreach my $data_stroke (@data_points)
	{
		# Temporary array
		my @data_stroke_processed;
		
		# Get the type of path
		push @data_stroke_processed, shift(@{$data_stroke});

		while(((my $xn = shift(@{$data_stroke}))&&(my $yn = shift(@{$data_stroke}))))
		{
			push @data_stroke_processed, ($xn * $scale);
			push @data_stroke_processed, ($yn  * $scale);
		}
		
		# Save temporary array in final array
		push @data_points_processed, [ @data_stroke_processed ];
	}
	
	# Adjust layout settings - common keys
	foreach my $key (qw(X_min Y_min X_max Y_max Thickness))
	{
		$Layout{$key} = $Layout{$key} * $scale;
	}
	
	return \@data_points_processed;
}





1;
