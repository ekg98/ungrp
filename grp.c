// grp.c - Contains various functions to handle .grp files designed by Ken Silverman.
//
// by William English

#include "grp.h"
#include <string.h>

// isKenSilvermanHeader(): *kenSilverman is a pointer to a 12 byte array.  This function will evaluate the array and determine if it's the correct header for a .grp file.
// returns true or false on respective condition.
bool isKenSilvermanHeader(char *kenSilverman)
{
	if(strncmp(kenSilverman,"KenSilverman", 12) == 0)
		return true;
	else
		return false;
}
