/* tagStormCheck - Check that a tagStorm conforms to a schema.. */
#include "common.h"
#include "linefile.h"
#include "hash.h"
#include "options.h"
#include "sqlNum.h"
#include "errAbort.h"

int clMaxErr = 10;
int gErrCount = 0;  // Number of errors so far


void usage()
/* Explain usage and exit. */
{
errAbort(
  "tagStormCheck - Check that a tagStorm conforms to a schema.\n"
  "usage:\n"
  "   tagStormCheck schema.txt tagStorm.txt\n"
  "The format of schema.txt is one line per allowed tag, with blank lines and lines\n"
  "starting with the # char treated as comments. Each line begins with the tag name\n"
  "and is followed by a symbol to indicate the tag type, one of\n"
  "    # - integer\n"
  "    %% - floating point number\n"
  "    $ - string\n"
  "Integer and floating point types may be followed by the min and max allowed vals if desired\n"
  "String types may be followed by a space-separated list of allowed values.  If the values\n"
  "themselves have spaces, they should be quoted.  Values may include * and ? wildcards.\n"
  "The tag names can also include wildcards.\n"
  "options:\n"
  "   -maxErr=N - maximum number of errors to report, defaults to %d\n"
  , clMaxErr);
}

/* Command line validation table. */
static struct optionSpec options[] = {
   {"maxErr", OPTION_INT},
   {"ignore", OPTION_STRING},
   {NULL, 0},
};

struct tagSchema
/* Represents schema for a single tag */
    {
    struct tagSchema *next;
    char *name;   // Name of tag
    char type;   // # for integer, % for floating point, $ for string
    double minVal, maxVal;  // Bounds for numerical types
    struct slName *allowedVals;  // Allowed values for string types
    };

struct tagSchema *tagSchemaFromFile(char *fileName)
/* Read in a tagSchema file */
{
struct lineFile *lf = lineFileOpen(fileName, TRUE);
char *line;
struct tagSchema *schema, *list = NULL;
while (lineFileNextReal(lf, &line))
    {
    /* Parse out first two fields */
    char *name = nextWord(&line);
    char *typeString = nextWord(&line);
    if (typeString == NULL)
        errAbort("truncated line %d of %s", lf->lineIx, lf->fileName);
    char type = typeString[0];

    /* Allocate schema struct and fill in first two fields. */
    AllocVar(schema);
    schema->name = cloneString(name);
    schema->type = type;

    /* Parse out rest of it depending on field type */
    if (type == '#' || type == '%') // numeric
        {
	char *minString = nextWord(&line);
	char *maxString = nextWord(&line);
	if (maxString == NULL)
	    {
	    schema->minVal = -BIGDOUBLE;
	    schema->maxVal = BIGDOUBLE;
	    }
	else
	    {
	    schema->minVal = sqlDouble(minString);
	    schema->maxVal = sqlDouble(maxString);
	    }
	}
    else if (type == '$')
        {
	char *val;
	while ((val = nextWordRespectingQuotes(&line)) != NULL)
	    {
	    char c = val[0];
	    if (c == '"' || c == '\'')
	        {
		val += 1;
		trimLastChar(val);
		}
	    slNameAddHead(&schema->allowedVals, val);
	    }
	slReverse(&schema->allowedVals);
	if (schema->allowedVals == NULL)
	    schema->allowedVals = slNameNew("*");
	}
    else
	{
        errAbort("Unrecognized type character %s line %d of %s", 
	    typeString, lf->lineIx, lf->fileName);
	}
    slAddHead(&list, schema);
    }
slReverse(&list);
return list;
}

void reportError(struct lineFile *lf, char *message, char *tag)
/* Report error and abort if there are too many errors. */
{
warn("%s %s line %d of %s", message, tag, lf->lineIx, lf->fileName);
if (++gErrCount >= clMaxErr)
    noWarnAbort();
}

void tagStormCheck(char *schemaFile, char *tagStormFile)
/* tagStormCheck - Check that a tagStorm conforms to a schema.. */
{
/* Load up schema from file.  Make a hash of all non-wildcard
 * tags, and a list of wildcard tags. */
struct tagSchema *schema, *next, *schemaList = tagSchemaFromFile(schemaFile);
struct tagSchema *wildSchemaList = NULL;

struct hash *hash = hashNew(0);
for (schema = schemaList; schema != NULL; schema = next)
    {
    next = schema->next;
    if (anyWild(schema->name))
        slAddHead(&wildSchemaList, schema);
    else
	hashAdd(hash, schema->name, schema);
    }
slReverse(&wildSchemaList);
schemaList = NULL;

/* Stream through tagStorm file */
struct lineFile *lf = lineFileOpen(tagStormFile, TRUE);
char *line;
while (lineFileNextReal(lf, &line))
    {
    /* Break out tag and value */
    char *tag = nextWord(&line);
    char *val = skipLeadingSpaces(line);

    /* Find schema in hash or wildSchemaList */
    struct tagSchema *schema = hashFindVal(hash, tag);
    if (schema == NULL)
        {
	for (schema = wildSchemaList; schema != NULL; schema = schema->next)
	    {
	    if (wildMatch(schema->name, tag))
	        break;
	    }
	}

    /* Do checking on tag */
    if (schema == NULL)
        reportError(lf, "Unrecognized tag", tag);
    else
        {
	if (schema->type == '#')
	    {
	    char *end;
	    long long v = strtoll(val, &end, 10);
	    if (end == val || *end != 0)	// val is not integer
	        reportError(lf, "Non-integer value for ", tag);
	    else if (v < schema->minVal)
	        reportError(lf, "Value too low for ", tag);
	    else if (v > schema->maxVal)
	         reportError(lf, "Value too high for ", tag);
	    }
	else if (schema->type == '%')
	    {
	    char *end;
	    double v = strtod(val, &end);
	    if (end == val || *end != 0)	// val is not just a floating point number
		reportError(lf, "Non-numerical value for ", tag);
	    else if (v < schema->minVal)
	        reportError(lf, "Value too low for ", tag);
	    else if (v > schema->maxVal)
	        reportError(lf, "Value too high for ", tag);
	    }
	else
	    {
	    boolean gotMatch = FALSE;
	    struct slName *okVal;
	    for (okVal = schema->allowedVals; okVal != NULL; okVal = okVal->next)
	        {
		if (wildMatch(okVal->name, val))
		    {
		    gotMatch = TRUE;
		    break;
		    }
		}
	    if (!gotMatch)
	        reportError(lf, "Unrecognized value for", tag);
	    }
	}
    }
if (gErrCount > 0)
    noWarnAbort();
else
    verbose(1, "No problems detected.\n");
}

int main(int argc, char *argv[])
/* Process command line. */
{
optionInit(&argc, argv, options);
if (argc != 3)
    usage();
clMaxErr = optionInt("maxErr", clMaxErr);
tagStormCheck(argv[1], argv[2]);
return 0;
}