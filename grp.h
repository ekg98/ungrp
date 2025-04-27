// grp.h - Header file for definitions to handle grp files designed by Ken Silverman
//
// by William English

#include <stdbool.h>
#include <stdint.h>

#define MAXFILESINGRP	4096

// isKenSilvermanHeader(): *kenSilverman is a pointer to a 12 byte ASCII array.  Function determines if array contains the header for Ken Silverman's .grp files.
bool isKenSilvermanHeader(char *kenSilverman);

// grpFileStructure:  Structure containing information about the file located inside a .grp file.
struct grpFileStructure
{
	char fileName[13];
	uint32_t fileSize;
	long offset;

	// for sorting later
	struct grpFileStructure *next;
	
};
