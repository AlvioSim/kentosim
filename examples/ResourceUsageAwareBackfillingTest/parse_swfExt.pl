#!/usr/bin/perl
#
# example Perl script to extract job info from a log in SWF.
# then you can tabulate various statistics about something of interest.
#
# the data format is one line per job, with 18 fields:
#  0 - Job Number
#  1 - Submit Time
#  2 - Wait Time
#  3 - Run Time
#  4 - Number of Processors
#  5 - Average CPU Time Used
#  6 - Used Memory
#  7 - Requested Number of Processors
#  8 - Requested Time
#  9 - Requested Memory
# 10 - status (1=completed, 0=killed)
# 11 - User ID
# 12 - Group ID
# 13 - Executable (Application) Number
# 14 - Queue Number
# 15 - Partition Number
# 16 - Preceding Job Number
# 17 - Think Time from Preceding Job
#

use warnings;
use Time::Local;
use POSIX;

#this script generate a jobRequiremenets file
#the current version only generates the memory required, we consider that the amont of memory and GPS is not specified

die("input outputfileswf outputfilereq percBIG percNORM percSMALL") if $#ARGV < 5;

$input = shift;
$outputReq = shift;
$outputSWF = shift;

$perBIG = shift;
$perNORM = shift;

$jobs = 10000;

open INPUT,"<$input" or die("can not open the input file");
open OUTPUTREQ,">$outputReq" or die("can not open the output req file");
open OUTPUTSWF,">$outputSWF" or die("can not open the output swf file");

# count bad things
my $cnt_fmt  = 0;
my $cnt_t0   = 0;
my $cnt_p0   = 0;
my $cnt_stat = 0;
my $cnt_bad  = 0;

# some useful globals
my $start;
my $procs;
my $nodes;

#
# scan trace and collect job info
#
while (<INPUT>) {

    #
    # empty or comment line
    #
    # such lines are skipped, but note that some header comments
    # may include useful data.
    #
    if (/^\s*$|^;/) {

        print print OUTPUTSWF $_;

	# maintain data about log start time
	if (/^;\s*UnixStartTime:\s*(\d+)$/) {
	    $start = $1;
	}
	if (/^;\s*TimeZoneString:\s*([\w\/]+)$/) {
	    $ENV{TZ} = $1;
	    POSIX::tzset();
	}
	# and about system size
	if (/^;\s*MaxProcs:\s*(\d+)$/) {
	    $procs = $1;
	}
	if (/^;\s*MaxNodes:\s*(\d+)$/) {
	    $nodes = $1;
	}

        next;
    }

    #
    # parse job line
    #
    $_ =~ /^\s*(.*)\s*$/;
    my $line = $1;

    my @fields = split(/\s+/,$line);
    if ($#fields != 17) {
	warn "bad format at $line";
	$cnt_fmt++;
    }

    # or alternatively
    my ($job, $sub, $wait, $t, $p, $cpu, $mem, $preq, $treq, $mreq,
	$status, $u, $gr, $app, $q, $part, $prec, $think) = split(/\s+/,$line);

    #
    # skip if this job is not meaningful
    #
    if ( ! ($job =~ /^\s*\d/)) {
	# not a job at all -- line does not start with job ID.
	$cnt_fmt++;
	next;
    }

    if ($t == 0) {
	# someting potentially fishy, as job took 0 time.
	# but this can also be a resolution problem.
	$cnt_t0++;
	#next;
    }

    if ($p == 0) {
	# someting really fishy: job did not use any processors.
	# could mean job was cancelled before running.
	$cnt_p0++;
	next;
    }

    if (($sub == -1) || ($t == -1) || ($p == -1)) {
	# something very fishy: job arrival, runtime, or processors undefined.
	$cnt_bad++;
	next;
    }

    if ($status != 1) {
	# another fishy:
	# job failed (status 0)
	# job was cancelled (status 5)
	# or job is only part of a whole job (status 2, 3, 4)
	$cnt_stat++;
	next;
    }

    print OUTPUTSWF "$job $sub $wait $t $p $cpu $mem $preq $treq $mreq $status $u $gr $app $q $part $prec $think\n";
        
    my $memory = 0;
    my $perc2use = rand(100);

    if($perc2use < $perBIG)
    {
      $memory = 2000;
    }
    elsif($perc2use < $perBIG+$perNORM)
    {
      $memory = 1000;
    }
    else
    {
      $memory = 500;
    }    

    print OUTPUTREQ "$job $memory 0 0 0\n";

    $jobs--;
 
    GOTO end if $jobs == 0;

    #printing the 

    #
    # COLLECT DATA
    #

    # your code here...
}


end:    print "bad status $cnt_stat";
 
