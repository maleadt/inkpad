#!/usr/bin/perl

require "gtk2_routines.pm";

my $window = Gtk2_New_Window("Test window");

# Toolbar
my %toolbar_map = (
	"0 gtk-go-back - Back"		=>	\&GUI_Back,
	"1 gtk-go-forward - Forward"	=>	\&GUI_Forward,
	"2 gtk-go-up - Up"		=>	\&GUI_Up,
	"3 space"			=>	"",
	"4 gtk-convert - Convert"	=>	\&GUI_Convert,
	"5 gtk-clear - Clear"		=>	\&GUI_Clear,
	"6 space"			=>	"",
	"7 gtk-properties - Properties"	=>	\&GUI_Properties,
	"8 space"			=>	"",
	"9 gtk-quit - Quit"		=>	\&GUI_Quit,
	);
my $toolbar = Gtk2_New_Toolbar(\%toolbar_map);

# Container vbox
my $vbox = Gtk2_New_VBox($window);
$vbox->pack_start($toolbar, 0, 0, 0);

Gtk2_Launch($window);
