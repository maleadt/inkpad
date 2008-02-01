#!/usr/bin/perl

#
# Initialize
#

print "* Initializing\n";

# Modules
use strict;
use warnings;
use LWP;
use HTTP::Cookies;
use File::Copy;

# Browser
my $LWP_UserAgent = 'Mozilla/5.0 (Windows; U; Windows NT 5.1; nl; rv:1.8.1.11) Gecko/20071127 Firefox/2.0.0.11';
my $LWP_Browser = LWP::UserAgent->new;
$LWP_Browser->agent($LWP_UserAgent);
$LWP_Browser->cookie_jar({});

# Target data
my $Save_Folder = '/home/tim/Documenten/De Standaard';
my $Save_Date;
my $Save_File;
my $Save_PdfList;
my @Save_PdfArray;

# Relocate active directory
chdir $Save_Folder;


#
# Login page
#

print "* Login page\n";

my $URL_Login = 'http://www.standaard.be/Registratie/Aanmelden.aspx';
my $Response_Login = $LWP_Browser->get($URL_Login);
my $Content_Login = $Response_Login->content;

my $Data_ViewState;
if ($Content_Login =~ m/__VIEWSTATE" value="([^"]+)"/)
{
	$Data_ViewState = $1;
} else { die("Could not parse view state"); }



#
# Authenticate
#

print "* Authenticating\n";

my $Response_Auth = $LWP_Browser->post( $URL_Login,
	[	"__VIEWSTATE"	=>	$Data_ViewState,
		"_parts:midContainer:aanmeldBox:inCode"	=>	"",
		"_parts:midContainer:aanmeldBox:tbEmail"	=>	'paul.besard@telenet.be',
		"_parts:midContainer:aanmeldBox:tbPassword"	=>	'prbb6r',
		"_parts:midContainer:aanmeldBox:btnLogin"	=>	'Aanmelden',
	]
);

# Fake browser stupidity
$Response_Auth = $LWP_Browser->get("http://www.standaard.be/Registratie/");
$Response_Auth = $LWP_Browser->get("http://www.standaard.be/Registratie");



#
# Parse newspaper index
#

print "* Parsing newspaper index\n";

my $URL_Newspaper = "http://www.standaard.be/Krant/Beeld/";

my $Response_Newspaper = $LWP_Browser->get($URL_Newspaper);
my $Content_Newspaper = $Response_Newspaper->content;

while ($Content_Newspaper =~ s/new NewsPage\(\d+, (\d+), '([^']+)', '([^']+)', \d+, '([^']+)'\);//)
{	
	# Save extracted values
	my $Date = $1;
	my $Title = $2;
	my $Type = $3;
	my $URL = $4;
	
	# Extract filename
	my $File = $1 if ($URL =~ m/\/([^\/]+)$/);
	
	# Add to array
	push @Save_PdfArray, [ $Date, $Title, $Type, $File, $URL ];
	
	# Save date for later on
	$Save_Date = $Date unless ($Save_Date);
	$Save_PdfList .=  " $File";

}


#
# Generate filename
#

# List of months
my @months = qw(januari februari maart april mei juni juli augustus september oktober november december);

# Look up month
$Save_Date =~ m/(\d\d\d\d)(\d\d)(\d\d)/;
$Save_File = "De Standaard - $3 " . $months[$2 - 1] . " $1.pdf";

# Check if file does not already exist
if (-e "$Save_Folder/$Save_File")
{
	print "! File (\"$Save_File\") does already exist, not proceeding\n";
	exit;
}


#
# Fetch single PDF's
#

print "* Downloading newspaper pages\n";
mkdir $Save_Date unless (-d $Save_Date);

foreach my $SinglePDF (@Save_PdfArray)
{
	# Read data
	my ( $Date, $Title, $Type, $File, $URL ) = @{ $SinglePDF };

	# Don't double-do work
	next if (-e "$Date/$File");

	# Download it
	print "\t- $File...\n";
	my $Response_PDF = $LWP_Browser->get("http://www.standaard.be/Krant/Beeld/ToonPdf.aspx?file=$URL");
	open(WRITE, ">$Save_Date/$File");
	print WRITE $Response_PDF->content;
	close WRITE;

}


#
# Merge into one big PDF
#

print "* Merging\n";

chdir "$Save_Folder/$Save_Date";
system("/usr/bin/gs -dNOPAUSE -sDEVICE=pdfwrite -sOUTPUTFILE=\"$Save_Folder/$Save_File\" -dBATCH$Save_PdfList 1>/dev/null 2>/dev/null");

chdir "..";
system("rm -rf $Save_Date");

print "* Finished (saved as \"$Save_File\")!\n";

__END__
