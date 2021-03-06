# txGraph.sql was originally generated by the autoSql program, which also 
# generated txGraph.c and txGraph.h.  This creates the database representation of
# an object which can be loaded and saved from RAM in a fairly 
# automatic way.

#A transcription graph. Includes alt-splicing info.
CREATE TABLE txGraph (
    tName varchar(255) not null,	# name of target sequence, often a chrom.
    tStart int not null,	# First bac touched by graph.
    tEnd int not null,	# Start position in first bac.
    name varchar(255) not null,	# Human readable name.
    strand char(2) not null,	# + or - strand.
    vertexCount int unsigned not null,	# Number of vertices in graph.
    vertices longblob not null,	# Splice sites and soft ends.
    edgeCount int unsigned not null,	# Number of edges in graph.
    edgeList longblob not null,	# Edges (introns and exons) in graph.
    sourceCount int not null,	# Number of sources of evidence.
    sources longblob not null,	# Sources of evidence.
              #Indices
    PRIMARY KEY(tName)
);
