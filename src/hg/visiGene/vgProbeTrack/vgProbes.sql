-- MySQL dump 9.11
--
-- Host: localhost    Database: mm7
-- ------------------------------------------------------
-- Server version	4.0.24-standard-log

--
-- Table structure for table `vgProbes`
--

CREATE TABLE vgProbes (
  bin smallint(5) unsigned NOT NULL default '0',
  matches int(10) unsigned NOT NULL default '0',
  misMatches int(10) unsigned NOT NULL default '0',
  repMatches int(10) unsigned NOT NULL default '0',
  nCount int(10) unsigned NOT NULL default '0',
  qNumInsert int(10) unsigned NOT NULL default '0',
  qBaseInsert int(10) unsigned NOT NULL default '0',
  tNumInsert int(10) unsigned NOT NULL default '0',
  tBaseInsert int(10) unsigned NOT NULL default '0',
  strand char(2) NOT NULL default '',
  qName varchar(255) NOT NULL default '',
  qSize int(10) unsigned NOT NULL default '0',
  qStart int(10) unsigned NOT NULL default '0',
  qEnd int(10) unsigned NOT NULL default '0',
  tName varchar(255) NOT NULL default '',
  tSize int(10) unsigned NOT NULL default '0',
  tStart int(10) unsigned NOT NULL default '0',
  tEnd int(10) unsigned NOT NULL default '0',
  blockCount int(10) unsigned NOT NULL default '0',
  blockSizes longblob NOT NULL,
  qStarts longblob NOT NULL,
  tStarts longblob NOT NULL,
  KEY tName (tName(6),bin),
  KEY qName (qName(12))
) TYPE=MyISAM;

