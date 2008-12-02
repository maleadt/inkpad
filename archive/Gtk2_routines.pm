
# Needed modules
use strict;
use warnings;
use Gtk2;


#
# Main functions
#

sub Gtk2_New_Window
{
	# Input values
	my $title = shift;
	
	# Initialize Gtk2
	Gtk2->init;
	
	# Create window
	my $window = Gtk2::Window->new;
	$window->set_title($title);
	$window->set_border_width(3);
	
	# Signal handlers
	$window->signal_connect (destroy => sub { Gtk2->main_quit; });
	
	return $window
}

sub Gtk2_Launch
{
	# Input values
	my $window = shift;
	
	# Display all widgets and launch main
	$window->show_all;
	Gtk2->main;
	
	return;
}

sub Gtk2_New_VBox
{
	# Input values
	my $parent = shift;
	
	# Create widget
	my $vbox = Gtk2::VBox->new (0, 0);
	
	# Add vbox to parent
	$parent->add($vbox);
	
	return $vbox;
}

sub Gtk2_New_HBox
{
	# Input values
	my $parent = shift;
	
	# Create widget
	my $hbox = Gtk2::HBox->new (0, 0);
	
	# Add hbox to parent
	$parent->add($hbox);
	
	return $hbox;
}

sub Gtk2_New_Toolbar
{
	# Input values
	my $map_ref = shift;
	my %map = %$map_ref;
	
	# Create widget
	my $toolbar = Gtk2::Toolbar->new ();
	
	# Loop map (format: "gtk-go-forward - Forward"	=>	\&Forward)
	foreach my $key (sort keys %map)
	{
		my $value = $map{$key};
		
		if ($key =~ /^(\d+) space$/)
		{
			$toolbar->insert_space ($1);
			next;
		}
		
		
		$key =~ m/^(\d+) (.+) - (.+)$/;
		next unless (($2) && ($3));
		$toolbar->insert_stock ($2,
					$3,
					'tt private text',
					$value,
					'user_data',
					$1);
	}
	
	return $toolbar;
}




1;
