#!/usr/bin/env perl
use strict;
use warnings;

if (@ARGV < 1) {
    die "Usage: $0 <commit_hash> [<commit_hash> ...]\n";
}

foreach my $commit (@ARGV) {
    print "----------------------------------------\n";
    print "Checking commit: $commit\n";

    my $commit_msg = `git show -s --format=%B $commit`;
    if ($? != 0) {
        die "Error retrieving commit message for $commit.\n";
    }

    chomp $commit_msg;
    my @lines = split /\n/, $commit_msg;

    my $error = 0;

    # 1. Check that the commit has a subject line.
    my $subject = $lines[0] // '';
    if ($subject eq '') {
        print "Error: Commit $commit is missing a subject line.\n";
        $error = 1;
    }

    # 2. Check for a body message.
    # A proper commit message has a blank line after the subject.
    my @body = ();
    if (scalar(@lines) > 1) {
        if ($lines[1] eq '') {
            @body = @lines[2..$#lines];
        } else {
            @body = @lines[1..$#lines];
        }
    }

    # Exclude Signed-off-by: line
    my @non_body = grep { $_ !~ /^\s*Signed-off-by:/i} @body;
    # Remove any blank lines
    @non_body = grep { $_ !~ /^\s*$/ } @non_body;

    if (!@non_body) {
        print "Error: Commit $commit is missing a proper body message (excluding Signed-off-by line).\n";
        $error = 1;
    }

    # 3. Check for a "Signed-off-by:" line (DCO check).
    my $found_signed_off = 0;
    foreach my $line (@lines) {
        if ($line =~ /Signed-off-by:/) {
            $found_signed_off = 1;
            last;
        }
    }
    if (!$found_signed_off) {
        print "Error: Commit $commit is missing a 'Signed-off-by:' line.\n";
        $error = 1;
    }

    # 4. Check that each line does not exceed 72 characters.
    my $line_number = 1;
    foreach my $line (@lines) {
        if (length($line) > 72) {
            printf "Error: Commit %s, line %d exceeds 72 characters (length %d).\n", 
                $commit, $line_number, length($line);
            $error = 1;
        }
        $line_number++;
    }

    if ($error) {
	print "Commit $commit has issues. Please review the above messages.\n";
    } else {
        print "Commit $commit passed all checks.\n";
    }
    print "\n";
}
