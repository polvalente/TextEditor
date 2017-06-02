package TextModule;

use 5.018002;
use strict;
use warnings;

require Exporter;
#use AutoLoader qw(AUTOLOAD);
use List::Util qw(sum);

our @ISA = qw(Exporter);

# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.

# This allows declaration	use TextModule ':all';
# If you do not need this, moving things directly into @EXPORT or @EXPORT_OK
# will save memory.
our %EXPORT_TAGS = ( 'all' => [ qw(unique autocomplete matchCharacters findNext findAll replaceNext replaceAll textCount capitalize) ] );

our @EXPORT_OK = ( @{ $EXPORT_TAGS{'all'} } );

our @EXPORT = qw(

);

our $VERSION = '0.01';


# Preloaded methods go here.
sub unique {
    my %k;
    for my $w (@_){
         $k{$w} = ();
    }
    return keys(%k);
}

sub autocomplete {
    my $txt = $_[0];
    my $word = $_[1];
    my $dictionary = $_[2];

    #Creating list of all unique words found in text sources for autocompletion.
    $txt =~ s/[^a-zA-Z0-9 _-]/ /g; # remove todos os caracteres que possam atrapalhar o reconhecimento de palavras
    my @word_list = unique( split(/ /, $txt) );
    push(@word_list, unique( split(/ /, $dictionary)));

    #Creating output list with candidates sorted
    my @output_list;
    foreach my $w (@word_list) {
        if ($w =~ /^\Q$word\E/) {
            if ($w ne $word){
                push(@output_list, $w);
            }
        }
    }

    #return a list sorted by length and alphabetical order
    return sort { length($a) cmp length($b) or $a cmp $b} @output_list;
}

sub matchCharacters {

    #THIS FUNCTION RETURNS INDEX -1 IN CASE OF SUCESS

    my $txt = $_[0];
    my $char = $_[1];
    #stack for opening chars
    my @openStack;

    #translation table for matching chars
    my %charType = ('(' => ')', '[' => ']', '{' => '}');
    my $c;
    my $numel = 0;

    foreach my $index (0..length($txt)-1){
        $c = substr($txt,$index,1);

        if ($c eq $char){
            push(@openStack, [$char, $index]);
            $numel++;
        }
        elsif ($c eq $charType{$char}){
            if ($numel == 0){
                return ($char, $index);
            }
            pop(@openStack);
            $numel--;
        }
    }

    if ($numel != 0){
        return @{$openStack[-1]};
    }

    return ($char, -1);
}

sub findNext {
    my $txt = $_[0];
    my $word = $_[1];
    my $current = $_[2];
    return index($txt, $word, $current);
}

sub findAll {
    my $txt = $_[0];
    my $word = $_[1];
		my @matches;
		while($txt =~ m/($word)/g){
			push(@matches, pos($txt)-length($1));
		}
		return @matches;
}

sub replaceNext {
    my $txt = $_[0];
    my $word = $_[1];
    my $replace = $_[2];
    my $current = $_[3];

    my $position = findNext($txt, $word, $current);
    if ($position == -1) {
        return (1, $txt);
    }

    #return substr($txt, $position, length($word)) . $replace . substr($txt, $position + length($word));
    return (0, substr($txt, 0, $position) . $replace . substr($txt, $position + length($word)));
}

sub replaceAll {
    my $txt = $_[0];
    my $word = $_[1];
    my $replace = $_[2];
    my $count = ($txt =~ s/$word/$replace/g);
    if($count == 0){
        $count = 1;
    }
    else{
        $count = 0;
    }
    return ($count, $txt);
}

sub textCount {
    my $txt = $_[0];
    my $chars = length $txt;
		my $wordCount = 0;
		my $nw_chars = 0;
		my $lines = 0;
		
		if ($chars == 0){
		}
		else{
			my @words = split " ", $txt;
			$wordCount = scalar(@words);
			$nw_chars = sum(map({length($_)} @words));
			$lines = scalar(split(/\n/, $txt."."));
		}

    return ($chars, $wordCount, $nw_chars, $lines);
}

sub capitalize {
    my $txt = $_[0];
    my @phrases = split ( /([.?!])/, $txt);
    foreach my $p (@phrases){
        $p =~ s/\b(\w)/\U$1/;
    }

    return join('', @phrases);
}

# Autoload methods go after =cut, and are processed by the autosplit program.

1;
__END__
# Below is stub documentation for your module. You'd better edit it!

=head1 NAME

TextModule - Perl extension for blah blah blah

=head1 SYNOPSIS

  use TextModule;
  blah blah blah

=head1 DESCRIPTION

Stub documentation for TextModule, created by h2xs. It looks like the
author of the extension was negligent enough to leave the stub
unedited.

Blah blah blah.

=head2 EXPORT

None by default.



=head1 SEE ALSO

Mention other useful documentation such as the documentation of
related modules or operating system documentation (such as man pages
in UNIX), or any relevant external documentation such as RFCs or
standards.

If you have a mailing list set up for your module, mention it here.

If you have a web site set up for your module, mention it here.

=head1 AUTHOR

Paulo Valente, E<lt>valente@sd.apple.comE<gt>

=head1 COPYRIGHT AND LICENSE

Copyright (C) 2017 by Paulo Valente

This library is free software; you can redistribute it and/or modify
it under the same terms as Perl itself, either Perl version 5.18.2 or,
at your option, any later version of Perl 5 you may have available.


=cut
