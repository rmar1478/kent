/* knownMore.h was originally generated by the autoSql program, which also 
 * generated knownMore.c and knownMore.sql.  This header links the database and the RAM 
 * representation of objects. */

#ifndef KNOWNMORE_H
#define KNOWNMORE_H

struct knownMore
/* Lots of auxiliary info about a known gene */
    {
    struct knownMore *next;  /* Next in singly linked list. */
    char *name;	/* The name displayed in the browser: OMIM, gbGeneName, or transId */
    char *transId;	/* Transcript id. Genie generated ID. */
    char *geneId;	/* Gene (not transcript) Genie ID */
    unsigned gbGeneName;	/* Connect to geneName table. Genbank gene name */
    unsigned gbProductName;	/* Connects to productName table. Genbank product name */
    char *gbProteinAcc;	/* Genbank accession of protein */
    char *gbNgi;	/* Genbank gi of nucleotide seq. */
    char *gbPgi;	/* Genbank gi of protein seq. */
    unsigned omimId;	/* OMIM ID or 0 if none */
    char *omimName;	/* OMIM primary name */
    unsigned hugoId;	/* HUGO Nomeclature Committee ID or 0 if none */
    char *hugoSymbol;	/* HUGO short name */
    char *hugoName;	/* HUGO descriptive name */
    char *hugoMap;	/* HUGO Map position */
    unsigned pmId1;	/* I have no idea - grabbed from a HUGO nomeids.txt */
    unsigned pmId2;	/* Likewise, I have no idea */
    char *refSeqAcc;	/* Accession of RefSeq mRNA */
    char *aliases;	/* Aliases if any.  Comma and space separated list */
    unsigned locusLinkId;	/* Locus link ID */
    char *gdbId;	/* NCBI GDB database ID */
    };

void knownMoreStaticLoad(char **row, struct knownMore *ret);
/* Load a row from knownMore table into ret.  The contents of ret will
 * be replaced at the next call to this function. */

struct knownMore *knownMoreLoad(char **row);
/* Load a knownMore from row fetched with select * from knownMore
 * from database.  Dispose of this with knownMoreFree(). */

struct knownMore *knownMoreCommaIn(char **pS, struct knownMore *ret);
/* Create a knownMore out of a comma separated string. 
 * This will fill in ret if non-null, otherwise will
 * return a new knownMore */

void knownMoreFree(struct knownMore **pEl);
/* Free a single dynamically allocated knownMore such as created
 * with knownMoreLoad(). */

void knownMoreFreeList(struct knownMore **pList);
/* Free a list of dynamically allocated knownMore's */

void knownMoreOutput(struct knownMore *el, FILE *f, char sep, char lastSep);
/* Print out knownMore.  Separate fields with sep. Follow last field with lastSep. */

#define knownMoreTabOut(el,f) knownMoreOutput(el,f,'\t','\n');
/* Print out knownMore as a line in a tab-separated file. */

#define knownMoreCommaOut(el,f) knownMoreOutput(el,f,',',',');
/* Print out knownMore as a comma separated list including final comma. */

#endif /* KNOWNMORE_H */

