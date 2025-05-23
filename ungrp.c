// ungrp.c - ungrp
//
// A simple program to open Ken Silverman's .grp files and extract the files.
// Thanks to Ken for a wonderful engine and inspiration to stay in C programming.
//
// By William English

#include <stdio.h>
#include <stdlib.h>

#include "libgrp.h"

int main(int argc, char *argv[])
{
	// check to see if you have entered a filename to the program
	if(argc < 2)
	{
		printf("You must enter a file name to extract.\n");
		exit(EXIT_FAILURE);
	}

	// program loop to open file and do work
	for(int intFileCounter = 1; intFileCounter < argc; intFileCounter++)
	{
		FILE *fileToExtract = fopen(argv[intFileCounter], "rb");
		
		// opening error check
		if(fileToExtract == NULL)
		{
			fprintf(stderr, "Could not open file: %s\n", argv[intFileCounter]);
			continue;
		}

		// do some work - extractor part here
		char kenSilverman[12];
		uint32_t NumberOfFilesInGrp;

		fread(kenSilverman, sizeof(kenSilverman[0]), 12, fileToExtract);

		// determine if file has the magic KenSilverman header and extract files.	
		if(isKenSilvermanHeader(kenSilverman))
		{
			printf("Valid .grp file detected\n");

			fread(&NumberOfFilesInGrp, sizeof(NumberOfFilesInGrp), 1, fileToExtract);

			printf("%d file(s) found in %s\n", NumberOfFilesInGrp, argv[intFileCounter]);
			printf("Extracting...\n\n");

			struct grpFileStructure fileList[NumberOfFilesInGrp];

			// assign the first offset to the first element of the array of file data
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
			for (int intCounter = 0; intCounter < NumberOfFilesInGrp; intCounter++)
			{
				// print the file name and bytes being extracted. Max file size is like 4GB on FAT16 and FAT32.  This is a old format ya know.
				printf("%-12s\t%16d bytes", fileList[intCounter].fileName, fileList[intCounter].fileSize);

				// seek to the offset in the grp
				fseek(fileToExtract, fileList[intCounter].offset, SEEK_SET);

				// open the blank file to copy data to
				FILE* extractedFile = fopen(fileList[intCounter].fileName, "wb");

				// error check if it opened correctly and print a error if it didn't
				if (extractedFile == NULL)
				{
					fprintf(stderr, "Cannot create file %d\n", fileList[intCounter].fileName);
					perror("Error");
					exit(EXIT_FAILURE);
				}

				// read a byte from source grp to destination file
				uint8_t buffer;
				size_t bytesWritten = 0;
				size_t bytesRead = 0;

				for (long byteCounter = 0; byteCounter < fileList[intCounter].fileSize; byteCounter++)
				{
					bytesRead += fread(&buffer, sizeof(buffer), 1, fileToExtract);
					bytesWritten += fwrite(&buffer, 1, 1, extractedFile);
				};

				// verification that files are written correctly
				if ((bytesRead == bytesWritten) && (fileList[intCounter].fileSize == bytesWritten))
					printf("\tSUCCESS!\n");
				else
					printf("\tFAILURE!\n");

				// close the file that was being written
				if (fclose(extractedFile) == EOF)
				{
					fprintf(stderr, "Error closing file %s exiting...\n", argv[intFileCounter]);
					exit(EXIT_FAILURE);
				}
			};
				

		}
		else
			fprintf(stderr, "%s is not a valid .grp file.  Skipping...\n", argv[intFileCounter]);

		// closing error check
		if((fclose(fileToExtract) == EOF))
		{
			fprintf(stderr, "Could not close file: %s\n", argv[intFileCounter]);
			exit(EXIT_FAILURE);
		}
	};

	exit(EXIT_SUCCESS);
}
