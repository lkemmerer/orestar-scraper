#!/usr/bin/perl

=pod

primary key is in $in{'pk'}
FILE ITSELF is in the data somehow

Then we offer a variety of links to go to next -- enterimage for
all entries with a text entry but no photo entry?

=cut

sub mydie($){
	print <<"BLAM";
<hr>
<b>Error</b> $_[0]
BLAM

exit
}
flock LOG,LOCK_EX;
seek(LOG,0,2);
print LOG
scalar(localtime)," $ENV{'REMOTE_ADDR'} $0 $$;$S\n";

print <<"FOO";
Content-Type: text/html

FOO

foreach $C (split(';',$ENV{'HTTP_COOKIE'})){
        $C =~ m/^(\w+)=(.*)$/ and $Cookies{$1} = $2;
};

&ReadParsearoni;

=pod
 $in{'filename'} is the file name.

while(my ($k,$v) = each %in){
	print "$k --> $v <hr>\n";
};
=cut

# adjust path to your xlhtml executable in the next line
print `/opt/bin/xlhtml -a $in{'filename'}`;
unlink $in{'filename'};  # comment out to keep uploaded files


exit;

sub PrintForm(){
print <<FORM;

<html><head>
<title>Excel Spreadsheet Converter</title></head>
<body>
<br>
<form method=post enctype="multipart/form-data" action="uploader.pl">
<TABLE><TR>
<TD>What Excel file to display?</TD>
<TD><INPUT TYPE="file" SIZE=40 NAME="filename"></TD>
</TR>
<TR><TD>&nbsp;</TD><TD>&nbsp;</TD></TR>
<TR><TD>&nbsp;</TD>
<TD align="center"><INPUT TYPE="submit" VALUE="upload"></TD>
</TR></TABLE>
</form></body></html>


FORM

exit;
};

=pod

file uploading code ripped from

# Perl Routines to Manipulate CGI input
# cgi-lib@pobox.com
# $Id: uploader.pl,v 1.1.1.1 2002/03/20 15:33:03 slidedraw Exp $
#
# Copyright (c) 1993-1998 Steven E. Brenner  
# For more information, see:
#     http://cgi-lib.stanford.edu/cgi-lib/

and modified heavily.
(david nicol davidnicol@acm.org 02/22/1999)

and yet again
(david nicol davidnicol@acm.org 09/27/1999)

=cut

sub ReadParsearoni {

# Parameters affecting cgi-lib behavior
# User-configurable parameters affecting file upload.

# Do not change the following parameters unless you have special reasons
$cgi_lib'bufsize  =  8192;    # default buffer size when reading multipart
$cgi_lib'maxbound =   100;    # maximum boundary length to be encounterd

  
	# Get several useful env variables
  $type = $ENV{'CONTENT_TYPE'};
  $len  = $ENV{'CONTENT_LENGTH'};
  $meth = $ENV{'REQUEST_METHOD'};
  $maxdata    = 131072;    # maximum bytes to accept via POST - 2^17
  
  if ($len > $maxdata) { 
      mydie "Excel file is too large, at $len. The maximum size permitted is $maxdata";
  }
  
  unless ($ENV{'CONTENT_TYPE'} =~ m#^multipart/form-data#) {
	# mydie "expecting multipart/form-data to upload an Excel Spreadsheet";
	PrintForm;
  };

    local ($buf, $boundary, $head, @heads, $cd, $ct, $fname, $ctype, $blen);
    local ($bpos, $lpos, $left, $amt, $fn, $ser);
    local ($bufsize, $maxbound) = 
      ($cgi_lib'bufsize, $cgi_lib'maxbound);


    # The following lines exist solely to eliminate spurious warning messages
    $buf = ''; 

    ($boundary) = $type =~ /boundary="([^"]+)"/; #";   # find boundary
    ($boundary) = $type =~ /boundary=(\S+)/ unless $boundary;
    mydie ("Boundary not provided: probably a bug in your server") 
      unless $boundary;
    $boundary =  "--" . $boundary;
    $blen = length ($boundary);

    if ($ENV{'REQUEST_METHOD'} ne 'POST') {
      mydie("Invalid request method for  multipart/form-data: $meth\n");
    }

    $writefiles = './tempdata/';
    stat ($writefiles);
    mydie "Cannot write to directory $writefiles" unless  -d _ && -w _;

    # read in the data and split into parts:
    # put headers in @in and data in %in
    # General algorithm:
    #   There are two dividers: the border and the '\r\n\r\n' between
    # header and body.  Iterate between searching for these
    #   Retain a buffer of size(bufsize+maxbound); the latter part is
    # to ensure that dividers don't get lost by wrapping between two bufs
    #   Look for a divider in the current batch.  If not found, then
    # save all of bufsize, move the maxbound extra buffer to the front of
    # the buffer, and read in a new bufsize bytes.  If a divider is found,
    # save everything up to the divider.  Then empty the buffer of everything
    # up to the end of the divider.  Refill buffer to bufsize+maxbound
    #   Note slightly odd organization.  Code before BODY: really goes with
    # code following HEAD:, but is put first to 'pre-fill' buffers.  BODY:
    # is placed before HEAD: because we first need to discard any 'preface,'
    # which would be analagous to a body without a preceeding head.

    $left = $len;
   PART: # find each part of the multi-part while reading data
    while (1) {
      die $@ if $errflag;

      $amt = ($left > $bufsize+$maxbound-length($buf) 
	      ?  $bufsize+$maxbound-length($buf): $left);
      $errflag = (($got = read(STDIN, $buf, $amt, length($buf))) != $amt);
      mydie "Short Read: wanted $amt, got $got\n" if $errflag;
      $left -= $amt;

      $in{$name} .= "\0" if defined $in{$name}; 
      $in{$name} .= $fn if $fn;

      $name=~/([-\w]+)/;  # This allows $insfn{$name} to be untainted
      if (defined $1) {
        $insfn{$1} .= "\0" if defined $insfn{$1}; 
        $insfn{$1} .= $fn if $fn;
      }
 
     BODY: 
      while (($bpos = index($buf, $boundary)) == -1) {
        if ($left == 0 && $buf eq '') {
	  foreach $value (values %insfn) {
            unlink(split("\0",$value));
	  }
	  mydie("cgi-lib.pl: reached end of input while seeking boundary " .
		  "of multipart. Format of CGI input is wrong.\n");
        }
        die $@ if $errflag;
        if ($name) {  # if no $name, then it's the prologue -- discard
          if ($fn) { print FILE substr($buf, 0, $bufsize); }
          else     { $in{$name} .= substr($buf, 0, $bufsize); }
        }
        $buf = substr($buf, $bufsize);
        $amt = ($left > $bufsize ? $bufsize : $left); #$maxbound==length($buf);
        $errflag = (($got = read(STDIN, $buf, $amt, length($buf))) != $amt);
	die "Short Read: wanted $amt, got $got\n" if $errflag;
        $left -= $amt;
      }
      if (defined $name) {  # if no $name, then it's the prologue -- discard
        if ($fn) { print FILE substr($buf, 0, $bpos-2); }
        else     { $in {$name} .= substr($buf, 0, $bpos-2); } # kill last \r\n
      }
      close (FILE);
      last PART if substr($buf, $bpos + $blen, 2) eq "--";
      substr($buf, 0, $bpos+$blen+2) = '';
      $amt = ($left > $bufsize+$maxbound-length($buf) 
	      ? $bufsize+$maxbound-length($buf) : $left);
      $errflag = (($got = read(STDIN, $buf, $amt, length($buf))) != $amt);
      die "Short Read: wanted $amt, got $got\n" if $errflag;
      $left -= $amt;


      undef $head;  undef $fn;
     HEAD:
      while (($lpos = index($buf, "\r\n\r\n")) == -1) { 
        if ($left == 0  && $buf eq '') {
	  foreach $value (values %insfn) {
            unlink(split("\0",$value));
	  }
	  mydie("cgi-lib: reached end of input while seeking end of " .
		  "headers. Format of CGI input is wrong.\n$buf");
        }
        die $@ if $errflag;
        $head .= substr($buf, 0, $bufsize);
        $buf = substr($buf, $bufsize);
        $amt = ($left > $bufsize ? $bufsize : $left); #$maxbound==length($buf);
        $errflag = (($got = read(STDIN, $buf, $amt, length($buf))) != $amt);
        die "Short Read: wanted $amt, got $got\n" if $errflag;
        $left -= $amt;
      }
      $head .= substr($buf, 0, $lpos+2);
      push (@in, $head);
      @heads = split("\r\n", $head);
      ($cd) = grep (/^\s*Content-Disposition:/i, @heads);
      ($ct) = grep (/^\s*Content-Type:/i, @heads);

      ($name) = $cd =~ /\bname="([^"]+)"/i; #"; 
      ($name) = $cd =~ /\bname=([^\s:;]+)/i unless defined $name;  

      ($fname) = $cd =~ /\bfilename="([^"]*)"/i; #"; # filename can be null-str
      ($fname) = $cd =~ /\bfilename=([^\s:;]+)/i unless defined $fname;
      $incfn{$name} .= (defined $in{$name} ? "\0" : "") . 
        (defined $fname ? $fname : "");

      ($ctype) = $ct =~ /^\s*Content-type:\s*"([^"]+)"/i;  #";
      ($ctype) = $ct =~ /^\s*Content-Type:\s*([^\s:;]+)/i unless defined $ctype;
      $inct{$name} .= (defined $in{$name} ? "\0" : "") . $ctype;

	$fn = $writefiles."temp$$"."a";
	while (-e $fn){ $fn++ };
	open (FILE, ">$fn") || mydie("Couldn't open $fn $!\n");
	#print "Wanting to write " and mydie("Couldn't open $fn $!\n");
      substr($buf, 0, $lpos+4) = '';
      undef $fname;
      undef $ctype;
    }

    if ($errflag) {
      local ($errmsg, $value);
      $errmsg = $@ || $errflag;
      foreach $value (values %insfn) {
        unlink(split("\0",$value));
      }
      mydie($errmsg);
    } else {
      # everything's ok.
    }
  }



