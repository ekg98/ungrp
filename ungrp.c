// ungrp.c - Ungrp
//
// A simple program to open Ken Silvermans .grp files and extract the files.
// Thanks to Ken for a wonderful engine and inspration to stay in C programming.
//
// By William English

#include <stdio.h>
#include <stdlib.h>
#include "grp.h"

int main(int argc, char *argv[])
{
	// check to see if you have entered in a filename to the program
	if(argc < 2)
	{
		printf("You must enter a file name to extract.\n");
		return EXIT_FAILURE;
	}

	// program loop to open file and do work
	for(int intFileCounter = 1; intFileCounter < argc; intFileCounter++)
	{
		FILE *fileToExtract;
		fileToExtract = fopen(argv[intFileCounter], "rb");
		
		// opening error check
		if(fileToExtract == NULL)
		{
			printf("Could not open file: %s\n", argv[intFileCounter]);
			return EXIT_FAILURE;
		}

		// do some work - extractor part here
		char kenSilverman[12];
		uint32_t NumberOfFilesInGrp;

		fread(kenSilverman, sizeof(kenSilverman[0]), 12, fileToExtract);

		// determine if file has the magic KenSilverman header and extract files.	
		if(isKenSilvermanHeader(kenSilverman))
		{
			printf(".grp file detected.\n");

			fread(&NumberOfFilesInGrp, sizeof(NumberOfFilesInGrp), 1, fileToExtract);

			printf("%d file(s) found.\n", NumberOfFilesInGrp);
			printf("Extracting...\n");

			struct grpFileStructure fileList[NumberOfFilesInGrp];
			long firstOffset;
			fileList[0].offset = ((NumberOfFilesInGrp + 1) * 16);

			// populate grpFileStructure
			for(int intCounter = 0; intCounter < NumberOfFilesInGrp; intCounter++)
			{
				// get filenames and enter them into the structure.  NULL terminate the string
				fread(fileList[intCounter].fileName, sizeof(fileList[intCounter].fileName[0]), 12, fileToExtract);
				fileList[intCounter].fileName[12] = '\0';

				// get filesize and enter them into the structure
				fread(&fileList[intCounter].fileSize, sizeof(fileList[intCounter].fileSize), 1, fileToExtract);

				// get offsets and enter them into the structure
				if(intCounter > 0)
					fileList[intCounter].offset = fileList[intCounter - 1].offset + fileList[intCounter - 1].fileSize;

				// NULL the next structure pointer
				fileList[intCounter].next = NULL;
			
			};
			
			// extract the files
			for(int intCounter = 0; intCounter < NumberOfFilesInGrp; intCounter++)
			{	
				// print the file name and bytes being extracted.
				printf("%s %d bytes at offset %d\n", fileList[intCounter].fileName, fileList[intCounter].fileSize, fileList[intCounter].offset);

				fseek(fileToExtract, fileList[intCounter].offset,SEEK_SET);
				
				FILE *extractedFile;
				extractedFile = fopen(fileList[intCounter].fileName, "wb");

				uint8_t buffer;
				for(long byteCounter = 0; byteCounter < fileList[intCounter].fileSize; byteCounter++)
				{
					fread(&buffer, sizeof(buffer), 1, fileToExtract);
					fwrite(&buffer, 1, 1, extractedFile);
				};

				fclose(extractedFile);
			};
				

		}
		else
			printf("%s is not a valid .grp file.  Skipping...\n", argv[intFileCounter]);

		// closing error check
		if((fclose(fileToExtract) == EOF))
		{
			printf("Could not close file: %s\n", argv[intFileCounter]);
			return EXIT_FAILURE;	
		}
	};

	return EXIT_SUCCESS;
}
