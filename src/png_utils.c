#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "png_utils.h"


// ----------helper functions------------------------
static int check_header(char* header) {
	
	char pngHeader[9] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
	
	return (memcmp(header, pngHeader, 8));

}



//----------------public functions---------------------------

// opens a png file, handling the error if needed
int open_png(char* address, png_image* png) {
	FILE* file = fopen(address, "rb");
	if (file == NULL) {

		char out[255];
		snprintf(out, 255, "file not found %s", address);
		perror(out);
		exit(1);
	}
	
	// check header
	char header[9];
	fread(header, 1, 8, file);
	if (check_header(header)) {
		char out[255];
		snprintf(out, 255, "%s is not a png file", address);
		perror(out);
		fclose(file);
		exit(1);
	}
	printf("opened png file: %d\n", address);	

	fclose(file);
}

// cleans a png file
void clean_png(png_image* png) {
	free(png);
}

