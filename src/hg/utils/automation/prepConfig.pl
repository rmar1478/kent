#!/usr/bin/env perl

use strict;
use warnings;

my $argc = scalar(@ARGV);

sub usage() {
  printf STDERR "usage: prepConfig.pl <db> <clade> <trackDbDir> ./refseq/*_assembly_report.txt\n";
  printf STDERR "must specify a <db> UCSC database id\n";
  printf STDERR "where clade is one of:\n";
  print STDERR `hgsql -N -e 'select name from clade order by priority;' hgcentraltest | xargs echo | fold -s -w 60 | sed -e 's/^/# /;'`;
  printf STDERR "and <trackDbDir> is a directory in trackDb/ directory\n";
  printf STDERR "for example: birds bats chicken etc...\n";
  printf STDERR "existing directories:\n";
  print STDERR `(cd ~/kent/src/hg/makeDb/trackDb; ls -ogrt) | grep "^d" | awk '{print \$NF}'  | sort | xargs echo | fold -s -w 70 | sed -e 's/^/# /;'`;
}

if ($argc != 4) {
  usage;
  exit 255;
}

if ( ! -s "photoReference.txt" ) {
  printf STDERR "#################################################\n";
  printf STDERR "ERROR: must have file photoReference.txt present\n";
  printf STDERR "#################################################\n";
  usage;
  exit 255;
}

my $db = shift;
my $clade = shift;
my $trackDbDir = shift;
my $asmReport = shift;
my $mitoChr2Acc = $asmReport;
$mitoChr2Acc =~ s/_assembly_report.txt//;
$mitoChr2Acc .= "_assembly_structure/non-nuclear/assembled_chromosomes/chr2acc";

my @monthNumber = qw( Zero Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec );
my $asmDate = "notFound";
my $asmName = "notFound";
my $sciName = "notFound";
my $commonName = "notFound";
my $submitter = "notFound";
my $genBankAccessionID = "notFound";
my $taxId = "notFound";
my $ncbiBioProject = "notFound";
my $ncbiGenomeId = "notFound";
my $ncbiAssemblyId = "notFound";

open (FH, "<$asmReport") or die "can not read $asmReport";
while (my $line = <FH>) {
  chomp $line;
  if ($line =~ m/^#\s+Assembly name:/) {
    $asmName = $line;
    $asmName =~ s/.*y name:\s+//;
    $ncbiAssemblyId = `wget -O /dev/stdout "http://eutils.ncbi.nlm.nih.gov/entrez/eutils/esearch.fcgi?db=assembly&term=$asmName" 2> /dev/null | grep "<Id>" | head -1 | sed -e 's/[<>Id/]//g;'`;
    chomp $ncbiAssemblyId;
  } elsif ($line =~ m/^#\s+Organism name:/) {
    if ($line =~ m/\s+\(/) {
       $commonName = $line;
       $commonName =~ s/.*\(//;
       $commonName =~ s/\)//;
    }
    $sciName = $line;
    $sciName =~ s/.*m name:\s+//;
    $sciName =~ s/\s+\(.*//;
#    my $searchTerm = $sciName;
#    $searchTerm =~ s/ /%20/g;
#    $ncbiGenomeId = `wget -O /dev/stdout "http://eutils.ncbi.nlm.nih.gov/entrez/eutils/esearch.fcgi?db=genome&term=$searchTerm" 2> /dev/null | grep "<Id>" |  sed -e 's/[<>Id/]//g;'`;
    chomp $ncbiGenomeId;
  } elsif ($line =~ m/^#\s+Date:/) {
    $asmDate = $line;
    $asmDate =~ s/.*Date:\s+//;
    my ($year, $month, $day) = split('-', $asmDate);
    $asmDate = sprintf("%s %s", $monthNumber[$month], $year);
  } elsif ($line =~ m/^#\s+Submitter:/) {
    $submitter = $line;
    $submitter =~ s/.*Submitter:\s+//;
  } elsif ($line =~ m/^#\s+RefSeq assembly accession:/) {
    $genBankAccessionID = $line;
    $genBankAccessionID =~ s/.*accession:\s+//;
    $genBankAccessionID =~ s/ .*//;
  } elsif ($line =~ m/^#\s+Taxid:/) {
    $taxId = $line;
    $taxId =~ s/.*Taxid:\s+//;
  } elsif ($line =~ m/^#\s+BioProject:/) {
    $ncbiBioProject = $line;
    $ncbiBioProject =~ s/.*BioProject:\s+//;
    $ncbiGenomeId = `wget -O /dev/stdout "http://eutils.ncbi.nlm.nih.gov/entrez/eutils/esearch.fcgi?db=genome&term=$ncbiBioProject" 2> /dev/null | grep "<Id>" |  sed -e 's/[<>Id/]//g;'`;
    $ncbiBioProject =~ s/PRJNA//;
    chomp $ncbiGenomeId;
  }
}

close (FH);

my $orderKey = `(printf "%s\t%s\n" "$db" "$commonName"; hgsql -N -e 'select name,organism,orderKey from dbDb order by orderKey;' hgcentraltest) | sort -k2 | grep -C 1 "$commonName" | cut -f3 | xargs echo | awk '{printf "%d", \$1+(\$2-\$1)/2}'`;
chomp $orderKey;

my $mitoAcc = "notFound";
if ( -s $mitoChr2Acc ) {
  $mitoAcc = `tail -1 $mitoChr2Acc | awk '{print \$NF}'`;
  chomp $mitoAcc;
}

printf "# config parameters for makeGenomeDb.pl:\n";
printf "db %s\n", $db;
printf "clade %s\n", $clade;
# printf "genomeCladePriority xxx\n";
printf "scientificName %s\n", $sciName;
printf "commonName %s\n", $commonName;
printf "assemblyDate %s\n", $asmDate;
printf "assemblyLabel %s\n", $submitter;
printf "assemblyShortLabel %s\n", $asmName;
printf "orderKey %s\n", $orderKey;
printf "mitoAcc %s\n", $mitoAcc;
printf "fastaFiles /hive/data/genomes/$db/ucsc/*.fa.gz\n";
printf "agpFiles /hive/data/genomes/$db/ucsc/*.agp\n";
printf "# qualFiles none\n";
printf "dbDbSpeciesDir %s\n", $trackDbDir;
print `cat photoReference.txt`;
# printf "photoCreditURL xxx\n";
# printf "photoCreditName xxx\n";
printf "ncbiGenomeId %s\n", $ncbiGenomeId;
printf "ncbiAssemblyId %s\n", $ncbiAssemblyId;
printf "ncbiAssemblyName %s\n", $asmName;
printf "ncbiBioProject %s\n", $ncbiBioProject;
printf "genBankAccessionID %s\n", $genBankAccessionID;
printf "taxId %s\n", $taxId;