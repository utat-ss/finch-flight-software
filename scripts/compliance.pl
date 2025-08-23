#!/usr/bin/env perl

# Copyright (c) 2025 The FINCH CubeSat Project Flight Software Contributors
# SPDX-License-Identifier: Apache-2.0

use strict;
use warnings;

my $exit_code = 0;

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
    my $sign_off_line;
    foreach my $line (@lines) {
        if ($line =~ /Signed-off-by:/) {
            $found_signed_off = 1;
            $sign_off_line = $line;
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

    # 5. Check that Git author identity matches the DCO identity
    if ($found_signed_off) {
        my ($sign_off_name, $sign_off_email)
            = $sign_off_line =~ /Signed-off-by:\s*(.+?)\s+<([^>]+)>/;

        unless (defined $sign_off_name && defined $sign_off_email) {
            print "Error: A malformed DCO signoff line is found.\n";
            $error = 1;
        }
        else {
            chomp(my $author_name  = `git log -1 --format=%an $commit`);
            chomp(my $author_email = `git log -1 --format=%ae $commit`);

            if ($sign_off_name ne $author_name || $sign_off_email ne $author_email) {
                print "Error: Git and DCO identity mismatch for $commit:\n";
                $error = 1;
            }
        }
    }

    if ($error) {
	print "Commit $commit has issues. Please review the above messages.\n";
	$exit_code = 1;
    } else {
        print "Commit $commit passed all checks.\n";
    }
    print "\n";
}

exit($exit_code);
