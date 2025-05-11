#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <zlib.h>

#include "png_utils.h"


// ----------helper functions------------------------
static int check_header(char* header) {
	
	char pngHeader[9] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
	
	return (memcmp(header, pngHeader, 8));

}

static int invert_endianness(int in) {
	int out = ((in>>24)&0xff) | // move byte 3 to byte 0
        ((in<<8)&0xff0000) | // move byte 1 to byte 2
        ((in>>8)&0xff00) | // move byte 2 to byte 1
        ((in<<24)&0xff000000); // byte 0 to byte 3
	return out;
}

static int handle_next_chunk(FILE* file, png_image* png) {
	
	// get length
	int length;
	fread(&length, 4, 1, file);
	
	// read as the correct endianness
	length = invert_endianness(length);

	// get chunk type
	char chunkTypeStr[5];
	fread(chunkTypeStr, 1, 4, file);
	chunkTypeStr[4] = '\0';
	
	printf("Chunk type:%s\n", chunkTypeStr);
	printf("Length: %d\n", length);
	
	// for IHDR chunk
	if (strncmp(chunkTypeStr, "IHDR", 4) == 0) {
		if (length != 13){
			perror("File may be corrupted, wrong length IHDR chunk");
			return 0;
		}
		fread(&png->width, 4, 1, file);
		png->width = invert_endianness(png->width);
		fread(&png->height, 4, 1, file);
		png->height = invert_endianness(png->height);
		fread(&png->bit_depth, 1, 1, file);
		fread(&png->color_type, 1, 1, file);
		fread(&png->compression, 1, 1, file);
		fread(&png->filter, 1, 1, file);
		fread(&png->interlace, 1, 1, file);
	}
	// for PLTE chunk
	else if (strncmp(chunkTypeStr, "PLTE", 4) == 0) {

	}		
	// for IDAT chunk
	else if (strncmp(chunkTypeStr, "IDAT", 4) == 0) {
		if (png->data == NULL) {
			png->size = length;
			png->data = malloc(length*sizeof(char));	
			fread(png->data, 1, length, file);
		} else {
			png->data = realloc(png->data, (length + png->size)*sizeof(char));
			fread(png->data + png->size, 1, length, file);
			png->size += length;
		}
	}
	// for IEND chunk
	else if (strncmp(chunkTypeStr, "IEND", 4) == 0) {
		// just return
		return 0;
	}	
	else if (strncmp(chunkTypeStr, "\0\0\0\0", 4) == 0) {
		return 0;
	}	
	else {	
		char dump[length];
		fread(dump, 1, length, file);
		printf("unknown chunk: %s\n", chunkTypeStr);
	}
	// skip the CRC bytes
	char crc[4];
	fread(crc, 1, 4, file);
	
	return 1;
}

//----------------public functions---------------------------

// opens a png file, returns 0 if file is read correctly, 1 otherwise 
int open_png(char* address, png_image* png) {
	FILE* file = fopen(address, "rb");
	if (file == NULL) {

		char out[255];
		snprintf(out, 255, "file not found %s\n", address);
		perror(out);
		return 1;
	}
	
	// check header
	char header[9];
	fread(header, 1, 8, file);
	if (check_header(header)) {
		char out[255];
		snprintf(out, 255, "%s is not a png file\n", address);
		perror(out);
		fclose(file);
		return 1;
	}
	printf("opened png file: %s\n", address);
	

	// initiate data
	png->data = NULL;
	
	// handle all the chunks
	int resp = 1;
	int i = 0;
	while(resp == 1){
		resp = handle_next_chunk(file, png);
	}

	fclose(file);
	printf("width: %d\n", png->width);
	printf("height: %d\n", png->height);
	printf("png->bit_depth: %d\n", png->bit_depth);
	printf("png->color_type: %d\n", png->color_type);
	printf("png->compression: %d\n", png->compression);
	printf("png->filter: %d\n", png->filter);
	printf("png->interlace: %d\n", png->interlace);
	return 0;
}

// [O
// cleans a png file
void clean_png(png_image* png) {
	free(png->data);
	free(png);
}

