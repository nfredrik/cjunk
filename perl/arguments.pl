#!/usr/bin/perl -w
sub pretty_print {
    my ($args) = @_;

#    $args->{nisse} = 0 if (!(defined($args->{nisse})));
    $args->{nisse} = 0 unless defined($args->{nisse});
    print "$($args->{text_width})";
    # Format $args->{text} to $args->{text_width} somehow.
    # If $args->{justification} is set, justify appropriately.
    # If $args->{indent} is set, indent the first line by one tab.
    # If $args->{sentence_lead} is set, make sure all sentences start with
    # two spaces.

    open my $fh, '>', $args->{filename}
        or die "Cannot open '$args->{filename}' for writing: $!\n";

   if(defined($args->{indent})) {
            print "Hej\n";
            print $fh '\t' .$args->{text};    

   }
   else {
            print $fh $args->{text};    
   }

    close $fh;

    print "$args->{nisse}";
    return;
}



$long_text ="Hej hopp i galopp!";

$c = 4;

pretty_print({
    filename      => 'filename',
    text          => $long_text,
    text_width    => \$c,
    justification => 'full',
    sentence_lead => 1,
    indent => 1,
    nisse => 31,

});
