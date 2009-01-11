#!/usr/bin/perl
open(WRITE, ">test.dat");
for (my $i = 0; $i <= 250000 ; $i += 5000)
{
	print "Elements: $i\r";
	my $mode_speed = 0;
	for (my $j = 0; $j < 5; $j++)
		{
		open(FILE, "./ustd $i |");
		print WRITE "$i";
		while (<FILE>)
		{
			if (m/speed test/)
			{
				$mode_speed = 1;
			}
			elsif (m/verification/)
			{
				$mode_speed = 0;
			}
			elsif ($mode_speed && m/(([\d\.]+)) seconds/)
			{
				print WRITE " ", $1;
			}
		}
		close(FILE);
		print WRITE "\n";
	}
}
close(WRITE);
system("gnuplot plot.gp");
