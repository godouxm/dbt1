#!/usr/bin/perl -w

# This file is released under the terms of the Artistic License.  Please see
# the file LICENSE, included in this package, for details.
#
# Copyright (C) 2004 Mark Wong & Open Source Development Lab, Inc.
#
# 20 May 2004

# For use with the Web driver.

use strict;
# perl -MCPAN -e 'install Statistics::Descriptive'
use Statistics::Descriptive;

my $usmd = Statistics::Descriptive::Full->new();
my $tt = Statistics::Descriptive::Full->new();
my %rt;
my %status;

$rt{ "ADMIN_CONFIRM" } = Statistics::Descriptive::Full->new();
$rt{ "ADMIN_REQUEST" } = Statistics::Descriptive::Full->new();
$rt{ "BEST_SELLERS" } = Statistics::Descriptive::Full->new();
$rt{ "BUY_CONFIRM" } = Statistics::Descriptive::Full->new();
$rt{ "BUY_REQUEST" } = Statistics::Descriptive::Full->new();
$rt{ "CUSTOMER_REGISTRATION" } = Statistics::Descriptive::Full->new();
$rt{ "HOME" } = Statistics::Descriptive::Full->new();
$rt{ "NEW_PRODUCTS" } = Statistics::Descriptive::Full->new();
$rt{ "ORDER_DISPLAY" } = Statistics::Descriptive::Full->new();
$rt{ "ORDER_INQUIRY" } = Statistics::Descriptive::Full->new();
$rt{ "PRODUCT_DETAIL" } = Statistics::Descriptive::Full->new();
$rt{ "SEARCH_REQUEST" } = Statistics::Descriptive::Full->new();
$rt{ "SEARCH_RESULT" } = Statistics::Descriptive::Full->new();
$rt{ "SHOPPING_CART" } = Statistics::Descriptive::Full->new();

$status{ "ADMIN_CONFIRM" } = Statistics::Descriptive::Full->new();
$status{ "ADMIN_REQUEST" } = Statistics::Descriptive::Full->new();
$status{ "BEST_SELLERS" } = Statistics::Descriptive::Full->new();
$status{ "BUY_CONFIRM" } = Statistics::Descriptive::Full->new();
$status{ "BUY_REQUEST" } = Statistics::Descriptive::Full->new();
$status{ "CUSTOMER_REGISTRATION" } = Statistics::Descriptive::Full->new();
$status{ "HOME" } = Statistics::Descriptive::Full->new();
$status{ "NEW_PRODUCTS" } = Statistics::Descriptive::Full->new();
$status{ "ORDER_DISPLAY" } = Statistics::Descriptive::Full->new();
$status{ "ORDER_INQUIRY" } = Statistics::Descriptive::Full->new();
$status{ "PRODUCT_DETAIL" } = Statistics::Descriptive::Full->new();
$status{ "SEARCH_REQUEST" } = Statistics::Descriptive::Full->new();
$status{ "SEARCH_RESULT" } = Statistics::Descriptive::Full->new();
$status{ "SHOPPING_CART" } = Statistics::Descriptive::Full->new();

my $argnum;
foreach $argnum (0 .. $#ARGV) {
    open(FH, "< $ARGV[$argnum]")
        or die "Couldn't open '$ARGV[$argnum]' for reading: $!\n";

    my $line;
    while (defined($line = <FH>) ) {
        chomp $line;
        my @word = split / /, $line;

        if ( $word[0] eq "***" ) {
            # Do nothing.
            next;
        } elsif ( $word[0] eq "USMD" ) {
            $usmd->add_data( $word[1] );
        } elsif ( $word[0] eq "THINKTIME" ) {
            $tt->add_data( $word[1] );
        } elsif ( $word[0] eq "RESPONSE_TIME" ) {
            $rt{ $word[ 2 ] }->add_data( $word[ 3 ] );
        } elsif ( $word[0] eq "STATUS" ) {
            $status{ $word[ 2 ] }->add_data( $word[ 1 ] );
        } else {
            # Do nothing.
            next;
            #print "unknown output: $line\n";
        }
    }
}

# Display statistics.

printf( "%-9s %5s %8s\n", "---------", "-----", "--------" );
printf( "%-9s %-5s %-8s\n", "", "Count", "Mean (s)" );
printf( "%-9s %5s %8s\n", "---------", "-----", "--------" );
printf( "%-9s %5d %8.1f\n", "USMD", $usmd->count(), $usmd->mean() );
printf( "%-9s %5d %8.1f\n", "Thinktime", $tt->count(), $tt->mean()/1000 );
printf( "%-9s %5s %8s\n", "---------", "-----", "--------" );
print "\n";

my $id;
my $total = 0;

foreach $id ( sort( keys %rt ) ) {
    $total += $rt{ $id }->count();
}

printf( "%21s %5s %8s %6s\n", "---------------------", "-----", "--------",
    "------" );
printf( "%-21s %-5s %-8s %-6s\n", "Interaction", "Mix %", "Response",
    "Errors" );
printf( "%36s\n", "Time (s)" );
printf( "%21s %5s %8s %6s\n", "---------------------", "-----", "--------",
    "------" );
foreach $id ( sort( keys %rt ) ) {
    if ( $rt{ $id }->count() > 0 ) {
        printf( "%-21s %5.1f %8.3f %6d %4.1f %5d\n", "$id",
            $rt{ $id }->count() / $total * 100,
            $rt{ $id }->mean(), $status{ $id }->count(),
            $status{ $id }->count() / $rt{ $id }->count() * 100,
            $rt{ $id }->count() );
    } else {
        printf( "%-21s %5s %8s %6d\n", "$id", "N/A", "N/A",
            $status{ $id }->count() );
    }
}
printf( "%21s %5s %8s %6s\n", "---------------------", "-----", "--------",
    "------" );
